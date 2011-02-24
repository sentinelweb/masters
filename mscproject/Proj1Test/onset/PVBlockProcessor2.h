/*
 * PVBlockProcessor.h
 *
 *  Created on: 16-Jul-2010
 *      Author: robm
 */

#ifndef PVBLOCKPROCESSOR2_H_
#define PVBLOCKPROCESSOR2_H_

#include "FFT.h"
#include "CircularBuffer.h"
class PVBlockProcessor2 {
private:
	float getNextHopSize();

public:
	FFT* fft;
	int blockCounter;// counter for total number of blocks for timekeeping
	float timeScaleFactor;
	float scaleFactor;
	int targetPosition;// the target global position form start
	int actualPosition;// the actual global position form start
	//float initialBlockPos;
	int hopCounter;// the hop number within the block
	int totalHopCounter;
	// the whole block .. not for the hop
	int inputBlockSize;
	int outputBlockSize;

	// the size of the hop.
	int inputHopSize;
	int actualInputHopSize;
	int outputHopSize;
	bool isTransient;
	// processing buffers
	CircularBuffer* inputBuffer;// the input audio
	CircularBuffer* outputBuffer;// the output overlapp added output

	void newBlock();
	void writeBlock();
	bool hasMoreBlocks();
	void prepareForNextBlock();
	void setBlock(short* _block);
	void setBlock(float* _block);
	int getOutputBlockSize();
	void getOutputBlock(short* buffer,int _size,float _scale);
	void getOutputBlockAdd(short *buffer, int _size,float scale);
	int checkOutputAvailable( int _size);
	void setTimeScaleFactor(float _timeScaleFactor);
	void setInputHopSize(int _inputHopSize);
	float getActualTimeScaleFactor();
	void setInputHopSize();
	PVBlockProcessor2(FFT* _f, int _inputBlockSize,int _outputBufferSize, int _outputHopSize,float _timeScaleFactor);

	virtual ~PVBlockProcessor2();
};

#endif /* PVBLOCKPROCESSOR2_H_ */
