/*
 * Onset.h
 *
 *  Created on: 22-Jun-2010
 *      Author: robm
 */

#ifndef ONSET_H_
#define ONSET_H_

#include "FFT.h"

class Onset {
private:
	FFT* f;
public:
	float spectralFlux();
	float phaseDeviation();
	float weightedPhaseDeviation(bool normalised);
	float complexDomain();
	bool checkReady(int i);
	float isTransient(float threshold,float stretch_ratio,float hopSize);

	Onset(FFT* f);
	virtual ~Onset();
};
#endif /* ONSET_H_ */
