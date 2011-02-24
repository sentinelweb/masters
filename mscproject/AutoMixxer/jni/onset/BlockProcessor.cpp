/*
 * BlockProcessor.cpp
 *
 *  Created on: 25-Jun-2010
 *      Author: robm
 */
#include "BlockProcessor.h"
#include "math.h"
//#include <iostream>
using namespace std;

BlockProcessor::BlockProcessor(FFT* _f,int _blockSize,int _hopSize){
	fft = _f;
	inputBlockSize = _blockSize;
	outputBlockSize = _blockSize;
	inputHopSize = outputHopSize = _hopSize;
	this->inputBuffer = new short[inputBlockSize];
	this->outputBuffer = new short[outputBlockSize+this->fft->fftSize];
	this->remainder = new short[this->fft->fftSize+outputHopSize];

	hopCount=0;
	hopFactor=this->fft->fftSize/outputHopSize*sqrt(2);;// ;//
	blockPos=0;
	readcount=0;
	initialBlockPos=blockPos;
	this->hopCounter=0;
}

void BlockProcessor::resetCounter(){
	hopCounter=0;
}

long BlockProcessor::getTime(int fs){// get time since last block counter reset in ms.
	return getTime(fs, hopCounter) ;
}
long BlockProcessor::getTime(int fs, long hops){// get time since last block counter reset in ms.
	return (hops * (long)inputHopSize) / (float)fs * 1000l  ;
}
/*
long BlockProcessor::getBPM(int fs, long hops){// get time since last block counter reset in ms.
	return 60/((hops * (long)inputHopSize)  / (float)fs) ;
}
*/
float BlockProcessor::getBPM(float fs, float hops){// get time since last block counter reset in ms.
	return 60/((hops * inputHopSize)  / (float)fs) ;
}
int BlockProcessor::bpmToHops(int fs, int tempo){// get time since last block counter reset in ms.
	//return 60/((blocks * (long)hopSize)  / (float)fs) ;
	return 60*fs/((float)tempo*inputHopSize);
}
long BlockProcessor::getSamps(){// get # samps since last block counter reset.
	return (hopCounter * inputHopSize)  ;
}

void BlockProcessor::setBlock(float *_block){
	for (int i=0;i<inputBlockSize;i++) {
		this->inputBuffer[i]=_block[i];
	}
}

void BlockProcessor::newBlock(){
	//cerr << "newBlock:"<<endl;
	if (this->blockPos<0) {
		int remStart=this->fft->fftSize+blockPos;//-inputHopSize;
		for (int i=0;i+blockPos<0;i++) {
			this->fft->processBuffer[i] = this->remainder[remStart+i];
		}
		int buffEnd=fft->fftSize+blockPos;
		for (int i=0;i<buffEnd;i++) {
			this->fft->processBuffer[i-blockPos] = this->inputBuffer[i];
		}
	} else {
		for (int i=0;i<fft->fftSize;i++) {
			this->fft->processBuffer[i] = this->inputBuffer[blockPos+i];
		}
	}
	this->hopCounter++;
}

void BlockProcessor::writeBlock(){
	//cout << "writeBlock:" << hopCount*hopSize << "-" << hopCount*hopSize+fft->fftSize << ":" << hopCount << ":" <<outputBlockSize <<endl;
	for (int i=0;i<fft->fftSize;i++) {
		int idx = hopCount*outputHopSize+i;
		if (idx<fft->fftSize+outputBlockSize+outputHopSize) {
			this->outputBuffer[idx]+=fft->processBuffer[i]/hopFactor;// hopefully overlap add;
		} else {
			//cout << "filled output ... shouldn't get here ..." << idx << ":" << outputBlockSize<<":"<<this->fft->fftSize <<":"<<i<<endl;

			//break;
		}
	}
	hopCount++;
	blockPos+=inputHopSize;
}

bool BlockProcessor::hasMoreBlocks(){
	bool val = blockPos+fft->fftSize<=readcount-inputHopSize;
	//cout << "hasMoreBlocks:"<<blockPos<<":"<<blockPos+fft->fftSize <<":"<<readcount << ":" << val <<endl;
	return val;
}
// the remainder will be a variable size
void BlockProcessor::prepareForNextBlock(){
	int nextBlockStart=readcount-blockPos;
	//cout << "prepareForNextBlock:" << readcount << ":" << blockPos << ":" << readcount - blockPos << endl;
	for (int i=0;i<nextBlockStart;i++) {
		int idx=blockPos+i;
		//cerr << i << endl;
		this->remainder[i]=this->inputBuffer[idx];
	}
	blockPos=blockPos-readcount;
	for (int i=0;i<fft->fftSize;i++) { // move end buffer to start
		this->outputBuffer[i]=this->outputBuffer[outputBlockSize+i];
	}
	for (int i=fft->fftSize;i<fft->fftSize+outputBlockSize;i++) { // move end buffer to start
		this->outputBuffer[i]=0;
	}
	hopCount=0;
	initialBlockPos=blockPos;
}

// should only be called after prepareForNextBlock (well, before next to call to writeBlock)
void BlockProcessor::setOutputBlockSizeRatio(float _inputRatio){
	setOutputBlockSize(((int)inputBlockSize*_inputRatio)+1);
}

void BlockProcessor::setOutputBlockSize(int _oBlockSize){
	short* oldopbbuf = this->outputBuffer;
	int oldOutputBlockSize = outputBlockSize;
	if (_oBlockSize%outputHopSize!=0) {
		_oBlockSize-=_oBlockSize%outputHopSize;
	}
	outputBlockSize = _oBlockSize;
	this->outputBuffer = new short[outputBlockSize+this->fft->fftSize+outputHopSize];
	for (int i=0;i<oldOutputBlockSize && i<outputBlockSize;i++) {
		this->outputBuffer[i]=oldopbbuf[i];
	}
	delete [] oldopbbuf;
}

BlockProcessor::~BlockProcessor(){
	delete [] this->inputBuffer;
	delete [] this->outputBuffer;
	delete [] this->remainder;
}
