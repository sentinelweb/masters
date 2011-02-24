/*
 * Stats.cpp
 *
 *  Created on: 01-Jul-2010
 *      Author: robm
 */

#include "Stats.h"
#include "math.h"
#include "stdlib.h"

Stats::Stats() {
	// TODO Auto-generated constructor stub

}

Stats::~Stats() {
	// TODO Auto-generated destructor stub
}

float Stats::mean(float* arr,int size) {
	return mean(arr,size,0,size-1);
}
float Stats::mean(float* arr,int size,int start,int end) {
	float mySum = 0;
	for ( int i=start;i<=end;i++) {
		mySum +=arr[i];
	}
	return mySum/(end-start);
}
// comparator
int compare_floats( const void* a, const void* b ) {  // compare function
	 float diff=*(float*)b-*(float*)a;
	 return (int) diff;
}

// also does alphaTrimmed mean if meanWidth>0
float Stats::median(float* arr,int size,int meanWidth) {
	float* sorted = new float[size];
	for (int i=0;i<size;i++) {sorted[i]=arr[i];}
	//using namespace outer;
	qsort(sorted,size,sizeof(float),compare_floats);
	int middle=size/2;
	float value=0;
	if (meanWidth<=0) {// striaght median
		value = sorted[middle];
		if (size%2==0){//even
			value=(value+sorted[middle+1])/2;
		}
	} else {// alphaTrimmed median
		int wid = meanWidth / 2;
		int start = middle - wid;
		int end = middle+wid+(size%2==0?1:0);
		if (start>0 && end <size) {
			value= mean(sorted,size,start,end);
		}
	}
	delete [] sorted;
	return value;
}

float Stats::sum(float* data,int size) {
	float sum = 0;
	//unsigned int sz = sizeof(data);
	//cout << "sum:sz:" << size << endl;
	for ( int i=0;i<size;i++) {
		sum +=data[i];
	}
	return sum;
}

float Stats::sumabs(float* data,int size) {
	float sum = 0;
	for ( int i=0;i<size;i++) {
		sum +=abs(data[i]);
	}
	return sum;
}
float Stats::abs(float num) {
	if (num>=0) {return num;} else {return -1*num;}
}


