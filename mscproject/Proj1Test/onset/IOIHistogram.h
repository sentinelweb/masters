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
	int histoSize;// spread of histogram values
	int sumPrevious;
	void addElement(float val);
	int estimateTempo();
	int* genHistogram(int histoSize);
	float* genWeightedHisto(int histoSize, int *histo);
	IOIHistogram(int _size);
	virtual ~IOIHistogram();
private:

};

#endif /* IOIHISTOGRAM_H_ */
