/*
 * Plot.h
 *
 *  Created on: 07-Aug-2010
 *      Author: robm
 */
#include "onset/SongData.h"
#ifndef PLOT_H_
#define PLOT_H_

class Plot {
public:
	static void plot(SongData* _s,char* fileName);
	static void plot(SongData* _s,char* fileName,float* onsetData, int onsetDataSize);
	Plot();
	virtual ~Plot();
};

#endif /* PLOT_H_ */
