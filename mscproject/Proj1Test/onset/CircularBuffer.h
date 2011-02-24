/*
 * CircularBuffer.h
 *
 *  Created on: 27-Jul-2010
 *      Author: robm
 */

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

class CircularBuffer {
public:
	short* buffer;
	int readPointer;
	int writePointer;
	int size;

	int incRead(int num);
	int decRead(int num);
	int incWrite(int num);
	int decWrite(int num);
	int usedSpace();// distance beetween read and write pointers
	void write(short* block,int _size);
	void write(float* block,int _size);
	void writeAdd(short* block,int _size,float _scaleFactor);
	void writeAdd(float* block,int _size,float _scaleFactor);
	void read(short* block,int _size,bool _clearAfter,float _scale);
	void read(float* block,int _size,bool _clearAfter,float _scale);
	void readAdd(short *block, int _size,bool _clearAfter,float _scale);
	void resize(int _newSize);
	void clearFreeSpace();
	void clear();
	CircularBuffer(int _size);
	virtual ~CircularBuffer();
};

#endif /* CIRCULARBUFFER_H_ */
