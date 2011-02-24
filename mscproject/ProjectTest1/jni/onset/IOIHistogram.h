/*
 * IOIHistogram.h
 *
 *  Created on: 07-Jul-2010
 *      Author: robm
 */

#ifndef IOIHISTOGRAM_H_
#define IOIHISTOGRAM_H_

class IOIHistogram {
public:
	int *values;
	int indexPointer;
	int size;// should be the filter size. though that may be variable
	int numValues;
	int accumulationThreshold;// onset below this threshold will be accumulated in larger values, the minimum tempo that can be detected
	int histoSpread;// spread of histogram values
	void addElement(float val);
	int estimateTempo();

	IOIHistogram(int _size);
	virtual ~IOIHistogram();
};

#endif /* IOIHISTOGRAM_H_ */
