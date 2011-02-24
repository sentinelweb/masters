/*
 * VariableArray.h
 *
 *  Created on: 06-Aug-2010
 *      Author: robm
 *  this class stores data in an expanding (by blocksize) array (up to 1000 blocks).
 */

#ifndef VARIABLEARRAY_H_
#define VARIABLEARRAY_H_

class VariableArray {
	int size;
	int rows;
	void checkIncrement();
public:
	int blockSize;
	float **data;

	void add(int _val);
	void add(float _val);
	float mean();
	float max();
	int getSize();
	void get(float* _block,int _start, int _end);
	float get(int _idx);
	VariableArray(int _blockSize);
	virtual ~VariableArray();
};

#endif /* VARIABLEARRAY_H_ */
