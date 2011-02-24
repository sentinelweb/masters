/*
 * OnsetArray.cpp
 *
 *  Created on: 01-Jul-2010
 *      Author: robm
 */

#include "OnsetArray.h"
#include "Stats.h"
#include <android/log.h>
OnsetArray::OnsetArray(int _size) {
	this->size=_size;
	this->values=new float[this->size];
	this->indexPointer=-1;
	this->lastOnset=0;
	this->lastOnsetInterval=0;
}

void OnsetArray::addElement(float val){
	++this->indexPointer%=this->size;
	this->values[this->indexPointer]=val;
	if (this->numValues<size) {this->numValues++;}
}

bool OnsetArray::checkForOnset(float thresh){
	float filterVal=Stats::median(this->values,this->numValues,-1);//median filter
	//float filterVal=Stats::mean(this->values,this->numValues);//mean filter
	if (thresh*filterVal<this->values[this->indexPointer]) {return true;}
	return false;
}
/*
float OnsetArray::mean(){
	return Stats::mean(this->values,this->numValues);//median filter
}
float OnsetArray::median(){
	return Stats::median(this->values,this->numValues,-1);//median filter
}
*/
bool OnsetArray::markOnset(int number,int threshold){
	int intervalSinceLast = number-this->lastOnset;
	//__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "markOnset: %d %d %d (%d)",this->lastOnset,threshold,intervalSinceLast,number);

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
