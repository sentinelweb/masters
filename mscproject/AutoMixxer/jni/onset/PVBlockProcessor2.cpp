/*
 * PVBlockProcessor2.cpp
 *
 *  Created on: 16-Jul-2010
 *      Author: robm
 */

#include "PVBlockProcessor2.h"
#include "CircularBuffer.h"
#include "math.h"

//#include <iostream>
//#include <cmath>
//#include "Util.h"
using namespace std;
/*
 *
 * for the time being i am going to ignore the compensation for transitent frames
 * i.e. just going to count transient frames in the block to make the
 */
PVBlockProcessor2::PVBlockProcessor2(FFT *_f, int _inputBlockSize, int _outputBufferSize, int _outputHopSize,float _timeScaleFactor){
	fft = _f;
	inputBlockSize = _inputBlockSize;
	actualInputHopSize = _inputBlockSize;
	outputBlockSize = _inputBlockSize;
	outputHopSize = _outputHopSize;

	this->inputBuffer = new CircularBuffer(_inputBlockSize+this->fft->fftSize);
	this->outputBuffer = new CircularBuffer(_outputBufferSize);
	hopCounter=0;
	totalHopCounter=0;
	targetPosition=0;
	actualPosition=0;
	isTransient=false;
	//initialBlockPos=blockPos;
	scaleFactor=outputHopSize/(float)this->fft->fftSize*2;//*sqrt(2);
	//cout << "scaleFactor: " <<scaleFactor<<endl;
	setTimeScaleFactor(_timeScaleFactor);
}

void PVBlockProcessor2::setBlock(short *_block){
	this->inputBuffer->write(_block, inputBlockSize);
	this->inputBuffer->incWrite(inputBlockSize);
}
void PVBlockProcessor2::setBlock(float *_block){
	this->inputBuffer->write(_block, inputBlockSize);
	this->inputBuffer->incWrite(inputBlockSize);
}

void PVBlockProcessor2::setInputHopSize() {
	if (isTransient && timeScaleFactor>1) {
		actualInputHopSize=outputHopSize;
	} else if (actualPosition>targetPosition) {
		actualInputHopSize=inputHopSize/2;
	} else {
		actualInputHopSize=inputHopSize;
	}
	isTransient=false;

}
bool PVBlockProcessor2::hasMoreBlocks(){
	//cout <<"----------------"<< hopCounter<<"---------------------------"<<endl;
	//cout <<"hasMoreBlocks:"<< "rp:" << this->inputBuffer->readPointer<<" wp:" << this->inputBuffer->writePointer<< " fs:"<<this->inputBuffer->freeSpace() << ": hopSize:" <<inputHopSize <<endl;
	//return this->inputBuffer->freeSpace()>=inputHopSize;

	if (this->inputBuffer->readPointer<this->inputBuffer->writePointer) {
		return this->inputBuffer->readPointer+this->fft->fftSize<this->inputBuffer->writePointer;
	} else {
		return this->inputBuffer->readPointer+this->fft->fftSize<this->inputBuffer->writePointer+this->inputBuffer->size;
	}
}

void PVBlockProcessor2::newBlock(){
	///cout <<"newBlock:"<< this->inputBuffer->readPointer <<endl;
	this->inputBuffer->read(this->fft->processBuffer,this->fft->fftSize,false,1);
	setInputHopSize();
	this->inputBuffer->incRead(actualInputHopSize);
	//cout <<"buffi:"<< this->fft->processBuffer[0] << ":"<< this->fft->processBuffer[2] << ":"<<this->fft->processBuffer[this->fft->fftSize-1] <<endl;

	actualPosition+=actualInputHopSize;// modify this when doing transients
	targetPosition+=inputHopSize;
	//cout <<"newBlock:"<< this->inputBuffer->readPointer <<endl;

}
// write to output buffer via overlap add
void PVBlockProcessor2::writeBlock(){
	//cout <<"writeBlock:"<< "rp:" << this->outputBuffer->readPointer<<" wp:" << this->outputBuffer->writePointer<< " fs:"<<this->outputBuffer->freeSpace() << ": hopSize:" <<outputHopSize <<endl;
	//cout <<"buff:"<< this->fft->processBuffer[0] << ":"<< this->fft->processBuffer[2] << ":"<<this->fft->processBuffer[this->fft->fftSize-1] <<endl;
	this->outputBuffer->writeAdd(this->fft->processBuffer,this->fft->fftSize,scaleFactor);//0.2
	this->outputBuffer->incWrite(outputHopSize);
	this->hopCounter++;
	this->totalHopCounter++;

}

void PVBlockProcessor2::prepareForNextBlock(){
	//cout <<"newBlock:"<< this->inputBuffer->writePointer <<endl;
	hopCounter=0;
}


/* should only be called after preparenextBlock*/
void PVBlockProcessor2::setTimeScaleFactor(float _timeScaleFactor){
	timeScaleFactor=_timeScaleFactor;
	inputHopSize=((float)outputHopSize/_timeScaleFactor)+0.5;
}
void PVBlockProcessor2::setInputHopSize(int _inputHopSize){
	inputHopSize=_inputHopSize;
	timeScaleFactor=outputHopSize/(float)actualInputHopSize;
}
float PVBlockProcessor2::getActualTimeScaleFactor(){
	float actualTimeScaleFactor=outputHopSize/(float)actualInputHopSize;
	//cout << "actualTimeScaleFactor:" << actualTimeScaleFactor << endl;
	return actualTimeScaleFactor;
}
int PVBlockProcessor2::getOutputBlockSize(){
	return hopCounter*outputHopSize;
}

int PVBlockProcessor2::checkOutputAvailable( int _size){
	//return (outputBuffer->size - outputBuffer->usedSpace())>_size;
	return (outputBuffer->usedSpace())>_size;
}

void PVBlockProcessor2::getOutputBlock(short *buffer, int _size,float _scale){
	outputBuffer->read(buffer,_size,true,_scale);
	outputBuffer->incRead(_size);
}

void PVBlockProcessor2::getOutputBlockAdd(short *buffer, int _size,float _scale){
	outputBuffer->readAdd(buffer,_size,true,_scale);
	outputBuffer->incRead(_size);
}

PVBlockProcessor2::~PVBlockProcessor2() {
	delete inputBuffer;
	delete outputBuffer;
}
