/*
 * BlockProcess.h
 *
 *  Created on: 25-Jun-2010
 *      Author: robm
 */

#ifndef BLOCKPROCESS_H_
#define BLOCKPROCESS_H_

#include "FFT.h"

class BlockProcessor {
private:
	FFT* fft;
public:
	int hopFactor;//=fftLen/hopSize;
	int hopCounter;// counter for total number of hops for timekeeping
	int hopCount;// the hop number within the block
	int readcount;
	int blockPos;// the buffer position of the start of the block
	int initialBlockPos;
	int inputBlockSize;
	int outputBlockSize;
	int outputHopSize;
	int inputHopSize;
	short* inputBuffer;
	short* outputBuffer;
	short* remainder;

	void newBlock();
	void resetCounter();
	long getTime(int fs);
	long getTime(int fs, long blocks);
	long getBPM(int fs, long blocks);
	int bpmToHops(int fs, int tempo);
	long getSamps();
	void writeBlock();
	bool hasMoreBlocks();
	void prepareForNextBlock();
	void setBlock(float* _block);
	void setOutputBlockSizeRatio(float _inputRatio);
	void setOutputBlockSize(int _oBlockSize);
	BlockProcessor(FFT* _f, int _blockSize, int _hopSize);
	virtual ~BlockProcessor();
};//<-- IMPORTANT !!!

#endif /* BLOCKPROCESS_H_ */
