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
	static float mean(float* arr,int size);
	static float median(float* arr,int size,int meanWidth);
	static float mean(float* arr,int size,int start,int end);
	static float sum(float* buf,int size);
	static float sumabs(float* buf,int size);
	friend int compare_floats( const void* a, const void* b );
	Stats();
	virtual ~Stats();
};

#endif /* STATS_H_ */
