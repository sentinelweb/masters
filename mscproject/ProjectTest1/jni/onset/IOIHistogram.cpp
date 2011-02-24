/*
 * IOIHistogram.cpp
 *
 *  Created on: 07-Jul-2010
 *      Author: robm
 */

#include "IOIHistogram.h"

using namespace std;

IOIHistogram::IOIHistogram(int _size) {
	this->size=_size;
	this->values=new int[this->size];
	this->indexPointer=-1;
	//this->accumulationThreshold=5;
	this->histoSpread=5;
}

void IOIHistogram::addElement(float val){
	++this->indexPointer%=this->size;
	this->values[this->indexPointer]=val;
	if (this->numValues<size) {this->numValues++;}
}

int IOIHistogram::estimateTempo(){
	int histoSize=200;
	int accumulator = 0;
	int* histo = new int[histoSize];
	for (int i=0;i<histoSize;i++) {histo[i]=0;}
	// build histogram - could actually just maintain a histogram by subtracting the removed value each time you add a new one - doesn't allow for accumulation()
	//for (int i=0;i<numValues;i++) {
	for (int i=indexPointer;i<numValues+indexPointer;i++) {
		int index = i%numValues;
		if (this->values[index]<histoSize) {
			histo[this->values[index]]++;
		}
		//if (this->values[index]<accumulationThreshold) {
		//	accumulator+=this->values[index];
		//}
		//if (accumulator>accumulationThreshold) {
		//	histo[accumulator]++;
		//	accumulator=0;
		//}
	}

	int tempo=0;
	int max=0;
//	for (int i=accumulationThreshold;i<histoSize;i++) {
//		if (histo[i]>max) {max=histo[i];tempo = i;}
//	}
	for (int i=5;i<histoSize-histoSpread;i++) {//accumulationThreshold
		int testVal = histo[i];
		for (int j=0;j<histoSpread;j++) {
			testVal += histo[i-j];
			testVal += histo[i+j];
		}
		if (testVal>max) {max=testVal;tempo = i;}
	}
	delete[]histo;
	return tempo;
}

IOIHistogram::~IOIHistogram() {
	// TODO Auto-generated destructor stub
}
