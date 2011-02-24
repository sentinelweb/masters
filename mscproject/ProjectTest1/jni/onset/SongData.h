/*
 * SongData.h
 *
 *  Created on: 23-Jul-2010
 *      Author: robm
 */
#include "Onset.h"
#include "OnsetArray.h"
#include "IOIHistogram.h"

#ifndef SONGDATA_H_
#define SONGDATA_H_

class SongData {
public:
	int tempo;


	Onset* onset ;
	OnsetArray* sfArr ;
	IOIHistogram* ioih ;

	SongData();
	virtual ~SongData();
};

#endif /* SONGDATA_H_ */
