/*
 * VariableArray.cpp
 *
 * A simple self-expanding array - android doesn't have support for the C++ STL objects
 *  Created on: 06-Aug-2010
 *      Author: robm
 */

#include "VariableArray.h"
#include "Stats.h"

VariableArray::VariableArray(int _blockSize){
	this->blockSize=_blockSize;
	this->size=0;
	this->data=new float*[1000];
	this->data[0]=new float[this->blockSize];
	for (int i=0;i<this->blockSize;i++) {this->data[0][i]=0;}
	this->rows=1;
	//this->rows++;
}
void VariableArray::checkIncrement() {
	this->size++;
	if (this->rows<this->size/this->blockSize+1) {
		this->data[this->rows]=new float[this->blockSize];
		for (int i=0;i<this->blockSize;i++) {this->data[this->rows][i]=0;}
		this->rows=this->size/this->blockSize+1;
	}
}
int VariableArray::add(float _val){
	this->checkIncrement();
	this->data[this->rows-1][(this->size-1)%this->blockSize]=_val;
}

int VariableArray::add(int _val){
	this->checkIncrement();
	this->data[this->rows-1][(this->size-1)%this->blockSize]=_val;
}
int VariableArray::getSize(){
	return this->size;
}
int VariableArray::get(float *_block, int _start, int _end){
	for (int i=0;i<_end-_start;i++) {
		int idx=_start+i;
		if (idx<this->size) {
			_block[i]=this->data[idx/this->blockSize][idx%this->blockSize];
		} else {
			_block[i]=0;
		}
	}
}

float VariableArray::get(int _idx){
	return this->data[_idx/this->blockSize][_idx%this->blockSize];
}
float VariableArray::mean(){
	float mean=0;
	if (size>blockSize) {
		int rem = this->size%this->blockSize;
		for (int i=0;i<this->rows-1;i++) {
			mean+=Stats::mean(this->data[i],this->blockSize)*this->blockSize;
		}
		mean+=Stats::mean(this->data[this->rows-1],rem);
		mean/=(float)size;
	} else {
		mean=Stats::mean(this->data[0],this->size);
	}
	return mean;
}
float VariableArray::max(){
	float max=0;
	for (int i=0;i<this->rows;i++) {
		max=Stats::max(max,Stats::max(this->data[i],this->blockSize));
	}
	return max;
}
VariableArray::~VariableArray() {
	for (int i=0;i<this->rows;i++) {
		delete [] this->data[i];
	}
	delete [] this->data;
}
