 /*
 * TestFFT.h
 *
 *  Created on: 21-Jun-2010
 *      Author: robm
 */

#ifndef FFT_H_
#define FFT_H_

#define     PI          3.14159265358979
#define     TWOPI       6.28318530717958
class FFT {
private:
	char magPhaseValid;
	char pddBufferValid;
	char deltaBufferValid;

	float **buffers;
	/*
	float *wr,*wi;
	float *wr_inv,*wi_inv;
	*/
	void dofft(float *inputreal,float *inputimag,float* outputreal,float* outputimag, const int isign);
public:
	int fftSize;
	int buffersFilled;
	float* processBuffer;
	float* realfftBuffer;
	float* imagfftBuffer;
	float* windowBuffer;
	float* magBuffer;
	float* magM1Buffer;
	float* magM2Buffer;
	float* phaseBuffer;
	float* phaseM1Buffer;
	float* phaseM2Buffer;
	//float* phaseD;// first phase difference
	float* phaseDD;// second phase difference
	float* deltaBuffer;// frequency delta for phase vocoder.
	float* omega;
	float* omegaHop;
	float* psi;

	void fft();
	void ifft();
	void swap(float* a, float* b);
	FFT(int size);
	void initBuffer();
	void makeWindow();
	void applyWindow();
	void cycleBuffers();
	void cartToPolar();
	void polarToCart();
	void fftshift();
	void polarToCart(float* mag,float* phase);
	float princarg(float phase);
	float* princarg(float* phase_in,int size);
	float* absFFT();
	float* phaseFFT();
	float* getPhaseDashDash();
	//float* getPhaseDash();
	float* getDelta(int hopSize);
	// for phase vocoding
	void incrementPsi(float ratio);
	virtual ~FFT();
};

#endif /* TESTFFT_H_ */
