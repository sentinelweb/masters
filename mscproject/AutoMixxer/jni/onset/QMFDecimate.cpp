/*
 * QMF.cpp
 *
 *  Created on: 12-Jul-2010
 *      Author: robm
 */

#include "QMFDecimate.h"
//#include <iostream>
using namespace std;

void QMFDecimate::filterAndDecimate(bool residual){
	// the values shoudl be multiplied by SQRT2 but it is causing issues when the values are converted back to short.
	// possibly need to mkae the blockprocessor inputBuffer float
	// /2=0.5 is the filter coefficient
	if (residual) {
		for (int i=0;i<this->size;i+=2) {
			this->decimated[i/2]=(this->block[i+1]+this->block[i])/2.0;//*SQRT2;
			this->residual[i/2]=(-this->block[i+1]+this->block[i])/2.0;//*SQRT2;
		}
	} else {
		for (int i=0;i<this->size;i+=2) {
			this->decimated[i/2]=(this->block[i+1]+this->block[i])/2.0;//*SQRT2;
		}
	}
}

void QMFDecimate::reconstruct(){
	float* lpf=new float[size];
	float* hpf=new float[size];
	for (int i=0;i<this->size/2;i++) {
		lpf[i*2]=this->decimated[i];
		hpf[i*2]=this->residual[i];
	}


	float* lpfo=new float[size];
	float* hpfo=new float[size];
	lpfo[0]=lpf[0]/2;
	hpfo[0]=hpf[0]/2;

	for (int i=1;i<this->size;i++) {
		// filter upsampled vectors
		lpfo[i]=(lpf[i]+lpf[i-1])/2.0;
		hpfo[i]=(hpf[i]-hpf[i-1])/2.0;
		// add together for original
	}

	this->block[0]=(lpfo[0]+hpfo[0])*SQRT2;
	for (int i=0;i<this->size;i++) {
		this->block[i]=(lpfo[i]+hpfo[i])*SQRT2;
	}
	//delete [] lpf;
	//delete [] hpf;
	//delete [] lpfo;
	//delete [] hpfo;
}

QMFDecimate::QMFDecimate(int _size){
	this->size=_size;
	this->block=new float[this->size];
	this->decimated=new float[this->size/2];
	this->residual=new float[this->size/2];
}

void QMFDecimate::copyBlock(float* _block){
	for (int i=0;i<size;i++) {
		this->block[i] = _block[i];
	}
}
void QMFDecimate::copyBlock(short* _block){
	for (int i=0;i<size;i++) {
		this->block[i] = (float)_block[i];
	}
}
QMFDecimate::~QMFDecimate() {
	//cout << "desctructor" << endl;
	delete [] this->block;
	delete [] this->decimated;
	delete [] this->residual;
	//cout << "desctructor end" << endl;
}
