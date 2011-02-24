/*
 * Onset.cpp
 *
 *  Created on: 22-Jun-2010
 *      Author: robm
 */
#include "Onset.h"
#include "math.h"
#include "stdlib.h"
#include "Stats.h"
//#include <iostream>
using namespace std;

float Onset::spectralFlux(){
	float* mag =  this->f->absFFT();
	//float* phase = this->f->phaseFFT();
	float val = 0;
	if (checkReady(1)) {
		float* fluxBuf = new float[f->fftSize];
		//cout << "fft:";
		for (int k=0; k<this->f->fftSize; k++) {
			//cout << mag[k] <<":";
			fluxBuf[k]=mag[k]-this->f->magM1Buffer[k];
			fluxBuf[k]=(fluxBuf[k]+abs(fluxBuf[k]))/2.0;  // half wave rectifier
		}
		//cout << endl;
		val = Stats::sum(fluxBuf,this->f->fftSize);
		delete [] fluxBuf;
	}
	//this->prevMagBuffer = mag;
	//this->prevPhaseBuffer = phase;
	return val;
}

bool Onset::checkReady(int i){
	return f->buffersFilled>=i;
}


float Onset::phaseDeviation(){
	float val=0;
	if (checkReady(2)) {
		val = 1.0/f->fftSize*Stats::sumabs(f->princarg(f->getPhaseDashDash(),f->fftSize),f->fftSize);
	}
	return val;
}

float Onset::weightedPhaseDeviation(bool normalised){
	float val=0;
	if (checkReady(2)) {
		float* wdevBuf = new float[f->fftSize];
		float* mag=f->absFFT();
		//float* phase=f->phaseFFT();
		float* dev=f->princarg(f->getPhaseDashDash(),f->fftSize);
		for (int k=0; k<this->f->fftSize; k++) {

			wdevBuf[k]=mag[k]*dev[k];
		}
		float multiplier = 1.0/f->fftSize;
		if (normalised) {
			multiplier=1.0/Stats::sum(mag,f->fftSize);
		}
		val = multiplier*Stats::sumabs(wdevBuf,f->fftSize);
	}
	return val;
}

float Onset::complexDomain(){
	float val=0;
	if (checkReady(2)) {
		//float* xt = new float[f->fftSize];
		float xt_mag_k=0;
		float xt_phase_k=0;
		float xt_real_k=0;
		float xt_imag_k=0;
		float cd_real_k=0;
		float cd_imag_k=0;
		float* cd_mag = new float[f->fftSize];
		//float* mag = f->absFFT();
		//float* pahse = f->phaseFFT();
		for (int k=0;k<f->fftSize;k++) {
			float phaseDashM1=(f->phaseM1Buffer[k]-f->phaseM2Buffer[k]);// phase diff at n-1
			xt_mag_k = f->magM1Buffer[k];
			xt_phase_k = f->princarg(f->phaseM1Buffer[k]+phaseDashM1);
			xt_real_k = xt_mag_k*cos(xt_phase_k);
			xt_imag_k = xt_mag_k*sin(xt_phase_k);
			cd_real_k = f->realfftBuffer[k]-xt_real_k;
			cd_imag_k = f->imagfftBuffer[k]-xt_imag_k;
			cd_mag[k] = sqrt(cd_real_k*cd_real_k + cd_imag_k*cd_imag_k);
		}
		val = Stats::sumabs(cd_mag,f->fftSize);
	}
	return val;
}

Onset::Onset(FFT* _f){
	this->f = _f;
}

Onset::~Onset(){

}
/* clacs the ratoi of transient bins using 2nd deviation*/
float Onset::isTransient(float threshold,float stretch_ratio,float hopSize){
	float val=0;
	float testValue = threshold*2*PI*hopSize/f->fftSize/stretch_ratio;
	if (checkReady(2)) {
		float* pdd= f->getPhaseDashDash();
		for (int i=0;i<f->fftSize;i++) {
			if (Stats::abs(pdd[i])>testValue) {	val++; }
		}
	}
	return val/f->fftSize;
}

