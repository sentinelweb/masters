/*
 * OnsetArray.h
 *
 *  Created on: 01-Jul-2010
 *      Author: robm
 */

#ifndef ONSETARRAY_H_
#define ONSETARRAY_H_

class OnsetArray {
public:
	float *values;
	int indexPointer;
	int size;// should be the filter size. though that may be variable
	int numValues;
	int lastOnset;// the last onset can be any type of number (block,sample,ms,etc)
	int lastOnsetInterval;
	float lastVal;
	float filterVal;
	float minLevel;
	void addElement(float val);
	bool doFilter();
	bool checkForOnset(float thresh);
	bool markOnset(int number,int thresh);
	//testing functions
	//float mean();
	//float median();

	OnsetArray(int size);
	virtual ~OnsetArray();
};

#endif /* ONSETARRAY_H_ */
