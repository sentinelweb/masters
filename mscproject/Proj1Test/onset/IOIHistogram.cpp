/*
 * IOIHistogram.cpp
 *
 *  Created on: 07-Jul-2010
 *      Author: robm
 */

#include "IOIHistogram.h"
#include "Stats.h"
#include <iostream>
using namespace std;

IOIHistogram::IOIHistogram(int _size) {
	this->size=_size;
	this->values=new int[this->size];
	this->indexPointer=-1;
	//this->accumulationThreshold=5;
	this->histoSize=200;
	this->numValues=0;
	this->sumPrevious=5;
}

void IOIHistogram::addElement(float val){
	++this->indexPointer%=this->size;
	this->values[this->indexPointer]=val;
	if (this->numValues<size) {this->numValues++;}
}

int* IOIHistogram::genHistogram(int histoSize){
    int *histo = new int[histoSize];
    for(int i = 0;i < histoSize;i++){ histo[i] = 0;  }
    for(int i = indexPointer;i < numValues + indexPointer;i++){
        int sum = 0;
        int localSumPrevious = sumPrevious;
        if (localSumPrevious>numValues-1) {localSumPrevious=numValues-1;}
        for(int j = 0;j < localSumPrevious;j++){
            int testVal = i - j;
            if(testVal < 0){
                testVal += numValues;
            }
            //if (testval<0) {continue;}
            int index = testVal % numValues;
            if (index<0 || index>=numValues) {
            	cout<<"ioi:oversz index:"<<index<<":numval:"<<numValues;
            }
            sum += this->values[index];
            if(sum < histoSize && sum > 20){
                histo[sum]++;
            }
        }
    }
    return histo;
}

float* IOIHistogram::genWeightedHisto(int histoSize, int  *histo){
    float *whisto = new float[histoSize];
    for(int k = 0;k < histoSize;k++){
        whisto[k] = 0;
    }
    for(int k = 5;k < histoSize;k++){
        float sum = 0;
        for(int i = -2;i < 2;i++){
            float thisVal = k;
            float mul = 2;
            if(i < 0){
                mul = 0.5;
            }
            for(int j = 0;j < Stats::abs(i);j++){
                thisVal *= mul;
            }
            if(thisVal < 5 || thisVal >= histoSize - 1){
                continue;
            }
            float theBinValue = 0;
            if(((int)((thisVal))) != thisVal){
                float fractionalPart = thisVal - ((int)((thisVal)));
                float last = histo[(int)((thisVal))];
                float next = histo[(int)((thisVal)) + 1];
                theBinValue = (last + ((next - last) * fractionalPart));
            }else{
                theBinValue = ((float)((histo[(int)((thisVal))])));
            }
            sum += theBinValue * histo[k];
        }

        whisto[k] = sum;
    }

    return whisto;
}

int IOIHistogram::estimateTempo(){
	//int histoSize=200;
	int* histo = genHistogram(histoSize);
    int tempo = 0;
    int max = 0;
    float *whisto = genWeightedHisto(histoSize, histo);
    for(int i = 5;i < histoSize;i++){
        if(whisto[i] > max){
            max = whisto[i];
            tempo = i;
        }
    }

    if(tempo == 0){
        tempo = 1;
    }
    delete[]whisto;
	delete[]histo;
	return tempo;
}

IOIHistogram::~IOIHistogram() {
	// TODO Auto-generated destructor stub
}
