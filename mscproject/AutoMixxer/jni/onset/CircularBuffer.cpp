/*
 * CircularBuffer.cpp
 *
 *  Created on: 27-Jul-2010
 *      Author: robm
 */
#include "CircularBuffer.h"
#include "Stats.h"
//#include <iostream>
using namespace std;

CircularBuffer::CircularBuffer(int _size) {
	this->size=_size;
	this->readPointer=0;
	this->writePointer=0;
	this->buffer=new short[this->size];
}

int CircularBuffer::incRead(int num){
	//cout << "incRead:" <<readPointer << ":" <<num;
	this->readPointer+=num;
	//cout << " 1:" <<readPointer ;
	this->readPointer%=size;
	//cout << " af:" <<readPointer<<endl;
	return this->readPointer;
}

int CircularBuffer::decRead(int num){
	this->readPointer-=num;
	if (this->readPointer<0) {this->readPointer+=size;}
	this->readPointer%=size;
	return this->readPointer;
}

int CircularBuffer::incWrite(int num){
	this->writePointer+=num;
	this->writePointer%=size;
	return this->writePointer;
}

int CircularBuffer::decWrite(int num){
	this->writePointer-=num;
	if (this->writePointer<0) {this->writePointer+=size;}
	this->writePointer%=size;
	return this->writePointer;
}

int CircularBuffer::usedSpace(){// not the buffer should never fill up so if 0 freespace means the bffer is empty - not full
	float val =  Stats::abs(this->writePointer-this->readPointer);
	//if (val>0) {
		return val;
	//}
	//return size;
}

void CircularBuffer::write(short *block, int _size){
	for (int i=0;i<_size;i++) {
		this->buffer[(this->writePointer+i)%size]=block[i];
	}
}
void CircularBuffer::write(float *block, int _size){
	for (int i=0;i<_size;i++) {
		this->buffer[(this->writePointer+i)%size]=block[i];
	}
}
void CircularBuffer::writeAdd(short *block, int _size,float _scaleFactor){
	for (int i=0;i<_size;i++) {
		this->buffer[(this->writePointer+i)%size]+=block[i]*_scaleFactor;
	}
}
void CircularBuffer::writeAdd(float *block, int _size,float _scaleFactor){
	for (int i=0;i<_size;i++) {
		this->buffer[(this->writePointer+i)%size]+=block[i]*_scaleFactor;
	}
}
void CircularBuffer::clearFreeSpace(){
	for (int i=0;i<usedSpace();i++) {
		this->buffer[(this->writePointer+i)%size]=0;
	}
}

void CircularBuffer::clear(){
	for (int i=0;i<size;i++) {this->buffer[i]=0;}
}

void CircularBuffer::read(short *block, int _size,bool _clearAfter,float _scale){
	for (int i=0;i<_size;i++) {
		//cout << i << ":" << (this->readPointer+i) <<":" << (this->readPointer+i)%size <<endl;
		block[i]=this->buffer[(this->readPointer+i)%size]*_scale;
	}
	if (_clearAfter) {
		for (int i= 0;i<_size;i++) {
			this->buffer[(this->readPointer+i)%size]=0;
		}
	}
}

void CircularBuffer::readAdd(short *block, int _size,bool _clearAfter,float _scale){
	for (int i=0;i<_size;i++) {
		//cout << i << ":" << (this->readPointer+i) <<":" << (this->readPointer+i)%size <<endl;
		block[i]+=this->buffer[(this->readPointer+i)%size]*_scale;
	}
	if (_clearAfter) {
		for (int i= 0;i<_size;i++) {
			this->buffer[(this->readPointer+i)%size]=0;
		}
	}
}
void CircularBuffer::read(float *block, int _size,bool _clearAfter,float _scale){
	for (int i=0;i<_size;i++) {
		//cout << i << ":" << (this->readPointer+i) <<":" << (this->readPointer+i)%size <<endl;
		block[i]=this->buffer[(this->readPointer+i)%size]*_scale;
	}
	if (_clearAfter) {
		for (int i= 0;i<_size;i++) {
			this->buffer[(this->readPointer+i)%size]=0;
		}
	}
}
void CircularBuffer::resize(int newSize){
	//not implemented
}

CircularBuffer::~CircularBuffer() {
	delete [] this->buffer;
}
