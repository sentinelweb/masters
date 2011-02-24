/*
 * Stats.h
 *
 *  Created on: 01-Jul-2010
 *      Author: robm
 */

#ifndef STATS_H_
#define STATS_H_

class Stats {
public:
	static float abs(float val);
	static void abs(short *_src,float *_tgt,int _size,int _offset);
	static void abs(float *_src,float *_tgt,int _size,int _offset);
	static float max(float _1,float _2);
	static short max(float *_src,int _size);
	static short max(short *_src,int _size);
	static float mean(float* arr,int size);
	static float median(float* arr,int size,int meanWidth);
	static float mean(short* arr,int size);
	static float mean(short* arr,int size,int start,int end);
	static float mean(float* arr,int size,int start,int end);
	static float sum(float* buf,int size);
	static float sumabs(float* buf,int size);
	friend int compare_floats( const void* a, const void* b );
	Stats();
	virtual ~Stats();
};

#endif /* STATS_H_ */
