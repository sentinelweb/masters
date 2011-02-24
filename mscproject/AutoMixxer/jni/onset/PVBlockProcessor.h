/*
 * PVBlockProcessor.h
 *
 *  Created on: 16-Jul-2010
 *      Author: robm
 */

#ifndef PVBLOCKPROCESSOR_H_
#define PVBLOCKPROCESSOR_H_

#include "FFT.h"

class PVBlockProcessor {
private:
	float getNextHopSize();

public:
	FFT* fft;
	int blockCounter;// counter for total number of blocks for timekeeping
	float timeScaleFactor;
	float scaleFactor;
	float blockPos;// the buffer position of the start of the block
	//float initialBlockPos;
	int hopCounter;// the hop number within the block
	int totalHopCounter;// the hop number within the block
	int readCount;
	float nextHopSize;
	// the whole block .. not for the hop
	int inputBlockSize;
	int outputBlockSize;
	int outputBufferSize;
	int remainderLength;
	// the size of the hop.
	float inputHopSize;
	int outputHopSize;

	// processing buffers
	short* inputBuffer;// the input audio
	short* outputBuffer;// the output overlapp added output
	short* remainder;// the remainder the previous input block need to start off the current block

	void newBlock();
	void writeBlock();
	bool hasMoreBlocks();
	void prepareForNextBlock();
	void setBlock(float* _block);
	int getOutputBlockSize();
	void setTimeScaleFactor(float _timeScaleFactor);

	PVBlockProcessor(FFT* _f, int _inputBlockSize, int _outputHopSize,float _timeScaleFactor);

	virtual ~PVBlockProcessor();
};

#endif /* PVBLOCKPROCESSOR_H_ */
