/*
 * OnsetArray.cpp
 *
 *  Created on: 01-Jul-2010
 *      Author: robm
 */

#include "OnsetArray.h"
#include "Stats.h"
//#include <iostream>
using namespace std;
OnsetArray::OnsetArray(int _size) {
	this->size=_size;
	this->values=new float[this->size];
	this->indexPointer=-1;
	this->lastOnset=0;
	this->lastVal=0;
	this->lastOnsetInterval=0;
	this->numValues=0;
	this->filterVal=-1;
	this->minLevel=2000;
}

void OnsetArray::addElement(float val){
	++this->indexPointer%=this->size;
	this->values[this->indexPointer]=val;
	if (this->numValues<size) {this->numValues++;}
	filterVal=-1;
}
/*
bool OnsetArray::checkForOnset(float thresh){
	float filterVal=Stats::median(this->values,this->numValues,-1);//median filter
	//float filterVal=Stats::mean(this->values,this->numValues);//mean filter
	if (thresh*filterVal<this->values[this->indexPointer]) {return true;}
	return false;
}
*/
bool OnsetArray::doFilter(){
	this->filterVal=Stats::median(this->values,this->numValues,-1);
	//this->filterVal=Stats::mean(this->values,this->numValues);//mean filter
}
// this now checks return true when the previous value is an onset to check for paeks
bool OnsetArray::checkForOnset(float thresh){
	float val = this->values[this->indexPointer];
	if (this->lastVal>0 ) {
		//cout << val << lastVal;
		if (val>lastVal) {
			this->lastVal=this->values[this->indexPointer];
			//cout << "wait"<<endl;
			return false;
		} else {
			this->lastVal=0;
			//cout << "good"<<endl;
			return true;
		}
	}

	//median filter
	if (filterVal==-1) {doFilter();	}

	if (thresh*filterVal<this->values[this->indexPointer] && this->values[this->indexPointer]>this->minLevel) {
		this->lastVal=this->values[this->indexPointer];
	}
	return false;
}


bool OnsetArray::markOnset(int number,int threshold){
	int intervalSinceLast = number-this->lastOnset;
	if (intervalSinceLast>threshold) {
		this->lastOnsetInterval=intervalSinceLast;
		this->lastOnset=number;
		return true;
	}
	return false;
}

OnsetArray::~OnsetArray() {
	delete [] this->values;
}
