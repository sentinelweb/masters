/*
 * PVBlockProcessor.cpp
 *
 *  Created on: 16-Jul-2010
 *      Author: robm
 */

#include "PVBlockProcessor.h"
#include "math.h"
//#include <iostream>
//#include <cmath>
//#include "Util.h"
using namespace std;
/*
 * inputBlockSize and outputHopSize should stay fixed.
 * inputHopSize varies with stretch factor (and = outputHopSize for transient grains)
 * outPutBlockSize is fixed multiple of inputBlockSize*timeScaleFactor
 *
 * for the time being i am going to ignore the compensation for transitent frames
 * i.e. just going to count transient frames in the block to make the
 */
PVBlockProcessor::PVBlockProcessor(FFT *_f, int _inputBlockSize, int _outputHopSize,float _timeScaleFactor){
	fft = _f;
	inputBlockSize = _inputBlockSize;
	outputBlockSize = _inputBlockSize;
	outputHopSize = _outputHopSize;
	blockCounter = 0;
	this->inputBuffer = new short[inputBlockSize];
	this->remainder = new short[this->fft->fftSize+outputHopSize];
	this->outputBuffer=0;
	blockPos=0;
	readCount=0;
	remainderLength=0;
	hopCounter=0;
	//initialBlockPos=blockPos;
	scaleFactor=this->fft->fftSize/outputHopSize*sqrt(2);
	setTimeScaleFactor(_timeScaleFactor);
}

void PVBlockProcessor::setBlock(float *_block){
	for (int i=0;i<inputBlockSize;i++) {
		this->inputBuffer[i]=_block[i];
	}
}

bool PVBlockProcessor::hasMoreBlocks(){
	//also need to check weather the output buffer isnt overfilled here.
	// actually output should only ever underfill...
	//cout << "hasMoreBlocks:"<<hopCounter <<":"<<outputHopSize << ":" << outputBlockSize<<":"<<blockPos<<":"<<getNextHopSize()<< endl;

	//if (hopCounter*outputHopSize>outputBlockSize) {
	//	return false;
	//}

	bool val = blockPos+fft->fftSize<=readCount-inputHopSize;
	//cout << "hasMoreBlocks:"<<blockPos<<":"<<blockPos+fft->fftSize <<":"<<readcount << ":" << val <<endl;
	return val;
}

void PVBlockProcessor::newBlock(){
	if (blockPos<0) {
		int bp=(int)blockPos;
		int remStart=remainderLength+bp;
		int i=0;
		//Util::arrop(this->remainder,remainderLength,"remainder");
		//Util::arrop(this->fft->processBuffer,this->fft->fftSize,"fftrem b4 copy");
		for (i=0;i+bp<0;i++) {
			this->fft->processBuffer[i] = this->remainder[remStart+i];
		}
		//cout<<i<<endl;
		//cout<<"newBlock(-):"<<":"<<hopCounter <<":"<<blockCounter+(int)blockPos<<":"<<blockPos<<":"<<remainderLength+bp<<":"<<remainderLength<<":"<<remStart+i<< ":"<< i-bp << endl;
		//cout<<0-bp<<endl;
		int buffEnd=remainderLength+bp;
		for (i=0;i<buffEnd;i++) {

			//cout << i << ":" << i-bp <<endl;
			this->fft->processBuffer[i-bp] = this->inputBuffer[i];
		}
		//Util::arrop(this->fft->processBuffer,this->fft->fftSize,"fftrem af copy");
	} else {
		//cout<<"newBlock(+):"<<blockCounter+(int)blockPos<<endl;
		//Util::arrop(this->fft->processBuffer,this->fft->fftSize,"fftbuf b4 copy" );
		for (int i=0;i<fft->fftSize;i++) {
			this->fft->processBuffer[i] = this->inputBuffer[(int)blockPos+i];
		}
		//Util::arrop(this->fft->processBuffer,this->fft->fftSize,"fftbuf af copy" );
	}

}
// write to output buffer via overlap add
void PVBlockProcessor::writeBlock(){
	//Util::arrop(this->fft->processBuffer,this->fft->fftSize,"fftbuf af fft" );
	for (int i=0;i<fft->fftSize;i++) {
		int idx = hopCounter*outputHopSize+i;
		if (idx<outputBufferSize) {
			this->outputBuffer[idx]+=fft->processBuffer[i]/scaleFactor;// hopefully overlap add;
		} else {
			//cout << "filled output ... shouldn't get here ..." << idx << ":" << outputBlockSize<<":"<<this->fft->fftSize <<":"<<i<<endl;
			//break;
		}
	}
	this->hopCounter++;
	//cout<<"writeBlock:"<<totalHopCounter*outputHopSize<<endl;
	totalHopCounter++;
	blockPos+=inputHopSize;
}

void PVBlockProcessor::prepareForNextBlock(){
	remainderLength=readCount-round(blockPos);
	//cout << "prepareForNextBlock:"<<readCount <<":"<<blockPos << ":" << remainderLength << endl;

	for (int i=0;i<remainderLength;i++) {
		int idx=round(blockPos)+i;
		this->remainder[i]=this->inputBuffer[idx];
	}
	//Util::arrop(this->remainder,remainderLength,"pnb:remainder" );
	blockPos=blockPos-readCount;
	int i=0;
	//Util::arrop(this->outputBuffer,outputBufferSize,"pnb:outputbuf 1" );
	for (i=0;i<outputBufferSize-getOutputBlockSize();i++) { // move end buffer to start //-fft->fftSize-outputHopSize
		this->outputBuffer[i]=this->outputBuffer[getOutputBlockSize()+i];
		//cout<<"Loopcopy:"<< i <<":"<<this->outputBuffer[i]<<endl;
	}

	//Util::arrop(this->outputBuffer,outputBufferSize,"pnb:outputbuf 2" );
	while (i<outputBufferSize) { // zero rest
		this->outputBuffer[i++]=0;
		//cout<<"Loopblank:"<< i <<":"<<outputBufferSize<<endl;
	}
	//Util::arrop(this->outputBuffer,outputBufferSize,"pnb:outputbuf 3" );
	blockCounter+=getOutputBlockSize();
	hopCounter=0;
	//initialBlockPos=blockPos;
}

int PVBlockProcessor::getOutputBlockSize(){
	// might be right but not sure
	// outputblocks are fixed size so it should work.... ( i think )
	return hopCounter*outputHopSize;
}
/* should only be called after preparenextBlock*/
void PVBlockProcessor::setTimeScaleFactor(float _timeScaleFactor){
	timeScaleFactor=_timeScaleFactor;
	//short* oldopbbuf = this->outputBuffer;
	int oldoutputBufferSize = outputBufferSize;
	int oldOutputBlockSize = outputBlockSize;
	inputHopSize=outputHopSize*_timeScaleFactor;
	// we only want to do transient correction for slower(>1) timescales as faster (<1) timescales wont be transient smeared.
	// for _timeScaleFactor >1 we stop tranisent smearing so the output buffer will be smaller
	// should always be the same # hops for the same timescaling factor - though there may be irregularities beetween frames if the hops size isnt a direct multiple of the the inputblocksize (there will be at most 1 extra hop)
	// for for _timeScaleFactor <1 - no transient smearing so no correction needed
	outputBlockSize=inputBlockSize/_timeScaleFactor;
	outputBufferSize = outputBlockSize + this->fft->fftSize+outputHopSize;
	//this->outputBuffer = new short[outputBufferSize];
	short* newopbbuf = new short[outputBufferSize];
	//cout<<"opbuf:n:"<< outputBufferSize << ":" <<oldoutputBufferSize<<endl;
	//Util::arrop(this->outputBuffer,outputBufferSize,"outputbuf:init" );
	if (this->outputBuffer!=NULL) {
		//may want to smooth off the end if the buffer shrinks
		for (int i=0;i<oldoutputBufferSize && i<outputBufferSize;i++) {
			//cout << i <<endl;
			newopbbuf[i]=this->outputBuffer[i];
			//this->outputBuffer[i]=oldopbbuf[i];
		}
		//cout << "finished1:" << endl;
		//delete [] oldopbbuf;
		delete [] this->outputBuffer;
	}
	this->outputBuffer=newopbbuf;
	//cout << "finished:" << endl;
}



PVBlockProcessor::~PVBlockProcessor() {
	//cout << "start pv desctruct" << endl;
	delete [] this->inputBuffer;
	//cout << "pv desctruct1" << endl;
	delete [] this->remainder;
	//cout << "pv desctruct2" << endl;
	delete [] this->outputBuffer;
	//cout << "end pv desctruct1" << endl;

}
