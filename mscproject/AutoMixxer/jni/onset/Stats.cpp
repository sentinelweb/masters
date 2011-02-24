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
	return mySum/(end-start+1);
}
float Stats::mean(short* arr,int size) {
	return mean(arr,size,0,size-1);
}
float Stats::mean(short* arr,int size,int start,int end) {
	float mySum = 0;
	for ( int i=start;i<=end;i++) {
		mySum +=arr[i];
	}
	return mySum/(end-start+1);
}
// comparator
int compare_floats( const void* a, const void* b ) {  // compare function
	 float diff=*(float*)b-*(float*)a;
	 return (int) diff+0.5;
}

// also does alphaTrimmed mean if meanWidth>0
float Stats::median(float* _arr,int _size,int _meanWidth) {
	float* sorted = new float[_size];
	for (int i=0;i<_size;i++) {sorted[i]=_arr[i];}
	//using namespace outer;
	qsort(sorted,_size,sizeof(float),compare_floats);
	int middle=_size/2;
	float value=0;
	if (_meanWidth<=0) {// striaght median
		value = sorted[middle];
		if (_size%2==0){//even
			value=(value+sorted[middle+1])/2;
		}
	} else {// alphaTrimmed median
		int wid = _meanWidth / 2;
		int start = middle - wid;
		int end = middle+wid+(_size%2==0?1:0);
		if (start>0 && end <_size) {
			value= mean(sorted,_size,start,end);
		}
	}
	delete [] sorted;
	return value;
}

float Stats::sum(float* _data,int _size) {
	float sum = 0;
	//unsigned int sz = sizeof(data);
	//cout << "sum:sz:" << size << endl;
	for ( int i=0;i<_size;i++) {
		sum +=_data[i];
	}
	return sum;
}

float Stats::sumabs(float* _data,int _size) {
	float sum = 0;
	for ( int i=0;i<_size;i++) {
		sum +=abs(_data[i]);
	}
	return sum;
}

void Stats::abs(short *_src,float *_tgt,int _size,int _offset) {
	for (int i=_offset;i<_size;i++) {
		if (_src[i]>=0) {_tgt[i-_offset]= _src[i];}
		else {_tgt[i-_offset]=-1*_src[i];}
	}
}
void Stats::abs(float *_src,float *_tgt,int _size,int _offset) {
	for (int i=_offset;i<_offset+_size;i++) {
		if (_src[i]>=0) {_tgt[i-_offset]= _src[i];}
		else {_tgt[i-_offset]=-1*_src[i];}
	}
}
float Stats::abs(float num) {
	if (num>=0) {return num;} else {return -1*num;}
}
short Stats::max(float *_src,int _size) {
	int max=0;
	for (int i=0;i<_size;i++) {
		if (_src[i]>=max) {max=_src[i];}
	}
	return max;
}
short Stats::max(short *_src,int _size) {
	int max=0;
	for (int i=0;i<_size;i++) {
		if (_src[i]>=max) {max=_src[i];}
	}
	return max;
}
float Stats::max(float _1,float _2) {
	 if (_1>_2) {return _1;}
	 return _2;
}

