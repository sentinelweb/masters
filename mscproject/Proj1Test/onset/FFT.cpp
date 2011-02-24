/*
 * FFT.cpp
 *
 *  Created on: 21-Jun-2010
 *      Author: robm
 */

#include "FFT.h"
#include "math.h"
//#include <iostream>

using namespace std;

FFT::FFT(int size) {
	this->magPhaseValid=1;
	this->deltaBufferValid=1;
	this->pddBufferValid=1;
	this->buffersFilled=0;
	this->fftSize=size;
	initBuffer();
	makeWindow();
}

void FFT::initBuffer() {
	this->processBuffer=new float[this->fftSize];
	this->imagfftBuffer=new float[this->fftSize];
	this->realfftBuffer=new float[this->fftSize];
	this->buffers=new float*[6];

	for (int i=0;i<6;i++) {
		this->buffers[i] = new float[this->fftSize];
	}
	this->magBuffer=this->buffers[0];
	this->magM1Buffer=this->buffers[1];
	this->magM2Buffer=this->buffers[2];

	//cout << "in:mag:" << this->magBuffer <<":" << this->magM1Buffer <<":" << this->magM2Buffer << endl;

	this->phaseBuffer=this->buffers[3];
	this->phaseM1Buffer=this->buffers[4];
	this->phaseM2Buffer=this->buffers[5];
	//cout << "in:phase:" << this->phaseBuffer <<":" << this->phaseM1Buffer <<":" << this->phaseM2Buffer << endl;

	this->phaseDD=new float[this->fftSize];

	//this->phaseD=new float[this->fftSize];
	this->deltaBuffer=new float[this->fftSize];
	this->omega = new float[this->fftSize];
	this->omegaHop = new float[this->fftSize];
	// cumulative phase Buffer for phaseVocoding
	this->psi = new float[this->fftSize];

	for (int k=0;k<fftSize;k++) {
		this->omega[k] = 2*PI*k/this->fftSize;
	}
}


void FFT::cycleBuffers() {
	this->magPhaseValid=1;

	this->pddBufferValid=1;
	this->deltaBufferValid=1;

	if (this->buffersFilled<2) {this->buffersFilled++;}

	float* tmp = this->magM2Buffer;
	this->magM2Buffer=this->magM1Buffer;
	this->magM1Buffer=this->magBuffer;
	this->magBuffer=tmp;

	//cout << "cy:af:mag:" << this->magBuffer <<":" << this->magM1Buffer <<":" << this->magM2Buffer << endl;

	tmp = this->phaseM2Buffer;
	this->phaseM2Buffer=this->phaseM1Buffer;
	this->phaseM1Buffer=this->phaseBuffer;
	this->phaseBuffer=tmp;


	//cout << "cy:phase:" << this->phaseBuffer <<":" << this->phaseM1Buffer <<":" << this->phaseM2Buffer << endl;

}

void FFT::makeWindow() {
	this->windowBuffer=new float[this->fftSize];
	int N = this->fftSize;
	for (int i=0;i<this->fftSize;i++) {
		float s = 0.5 * (1-cos(2*PI*i/(float)(N-1)));
		this->windowBuffer[i] = s;
	}
}

void FFT::applyWindow() {
	for (int i=0;i<this->fftSize;i++) {
		this->processBuffer[i] = this->windowBuffer[i]*(float)this->processBuffer[i];
	}
}

void FFT::fft() {
	dofft(processBuffer,(float*) 0,realfftBuffer,imagfftBuffer,-1);
	cycleBuffers();
}

void FFT::ifft() {
	dofft(realfftBuffer,imagfftBuffer,processBuffer,0,1);
	for (int i=0;i<this->fftSize;i++) {
		processBuffer[i]/=fftSize;
	}
}

void FFT::dofft(float* inputreal,float* inputimag,float* outputreal,float* outputimag, const int isign) {
	//int isign = -1;
	 int n=fftSize;
	 int nn = n << 1;
	 float data[nn];
	 int mmax,m,j,istep,i;
	 float wtemp,wr,wpr,wpi,wi,theta,tempr,tempi;
	 //if (n<2 || n&(n-1)) throw("n must be power of 2 in four1");
	 // setup data buffer
	 j=0;
	 if (inputimag!=0) {
		for (i=0;i<nn;i+=2){
			data[i]=inputreal[j];
			data[i+1]=inputimag[j];
			j=j+1;
		}
	 } else {
		for (i=0;i<nn;i+=2){
			data[i]=inputreal[j];
			data[i+1]=0;
			j=j+1;
		}
	 }
	 j = 1;
	 //This is the bit-reversal section of the routine.
	 for (i=1;i<nn;i+=2) {
		 if (j > i) {
			 //Exchange the two complex numbers.
			 swap(&data[j-1],&data[i-1]);
			 swap(&data[j],&data[i]);
		 }
		 m=n;
		 while (m >= 2 && j > m) {
			 j -= m;
			   m >>= 1;
		   }
		   j += m;
	  }
	  // Here begins the Danielson-Lanczos section of the routine.
	  mmax=2;
	  // Outer loop executed log2 n times.
	  while (nn > mmax) {
		  istep=mmax << 1;
		  //Initialize the trigonometric recurrence.
		  theta=isign*(2*PI/mmax);
		  wtemp=sin(0.5*theta);
		  wpr = -2.0*wtemp*wtemp;
		  wpi=sin(theta);
		  wr=1.0;
		  wi=0.0;
		  //Here are the two nested inner loops.
		  for (m=1;m<mmax;m+=2) {
			  for (i=m;i<=nn;i+=istep) {
				   //This is the Danielson-Lanczos formula:
				   j=i+mmax;
				   tempr=wr*data[j-1]-wi*data[j];
				   tempi=wr*data[j]+wi*data[j-1];
				   data[j-1]=data[i-1]-tempr;
				   data[j]=data[i]-tempi;
				   data[i-1] += tempr;
				   data[i] += tempi;
			  }
			  //Trigonometric recurrence.
			  wr=(wtemp=wr)*wpr-wi*wpi+wr;
			  wi=wi*wpr+wtemp*wpi+wi;
		  }
		  mmax=istep;
	  }
	  if (outputimag!=0) {
			for (i=0;i<fftSize;i++){
				outputreal[i]=data[i<<1];
				outputimag[i]=data[(i<<1)+1];
			}
	  } else {
			for (i=0;i<fftSize;i++){
				outputreal[i]=data[i<<1];
			}
	  }
}

void FFT::swap(float* a, float* b){
	float tempr=0;
	tempr= *a;
	*a=*b;
	*b=tempr;
}

/*
 * Wouldn't be efficient if phase_in was large - but it wont be.
 * rewritten 25/7/10: method by Carlo Drioli -  see tricks of  pahse vocoder paper
 */
float FFT::princarg(float phase_in) {
	/*
	float inc=2*PI;
	if (phase_in<=-PI) {while (phase_in<=-PI) {phase_in+=inc;}
	} else if (phase_in>PI) {
		while (phase_in>PI) {	phase_in-=inc;}
	}
	return phase_in;
	*/
	int a=floor(phase_in/TWOPI+0.5);
	return phase_in-a*TWOPI;
}

float* FFT::princarg(float* phase_in,int size) {
	float *newValues = new float[size];
	for (int i=0;i<size;i++) {
		newValues[i]=princarg(phase_in[i]);
	}
	return newValues;
}
void FFT::cartToPolar() {
	for (int i1=0;i1<this->fftSize;i1++){
		double thisVal = sqrt(this->realfftBuffer[i1]*this->realfftBuffer[i1]+this->imagfftBuffer[i1]*this->imagfftBuffer[i1]);
		this->magBuffer[i1]=thisVal;
	}
	for (int i1=0;i1<this->fftSize;i1++){
		//double thisVal = 0;

		//if (this->realfftBuffer[i1]!=0) {
		//	thisVal = atan2( this->imagfftBuffer[i1],this->realfftBuffer[i1]);
		//} else {
		//	thisVal=PI/2*(this->imagfftBuffer[i1]<0?-1:1);
		//}
		//this->phaseBuffer[i1]=thisVal;
		this->phaseBuffer[i1] = atan2( this->imagfftBuffer[i1],this->realfftBuffer[i1]);
	}
}

void FFT::polarToCart() {
	for (int i=0;i<this->fftSize;i++){
		this->realfftBuffer[i]=this->magBuffer[i]*cos(this->phaseBuffer[i]);
		this->imagfftBuffer[i]=this->magBuffer[i]*sin(this->phaseBuffer[i]);
	}
}

void FFT::polarToCart(float *mag, float *phase){
	for (int i=0;i<this->fftSize;i++){
		this->realfftBuffer[i]=mag[i]*cos(phase[i]);
		this->imagfftBuffer[i]=mag[i]*sin(phase[i]);
	}
}

float* FFT::absFFT() {
	if (this->magPhaseValid == 1) {	this->cartToPolar();this->magPhaseValid=0;}
	return this->magBuffer;
}

float* FFT::phaseFFT() {
	if (this->magPhaseValid == 1) {this->cartToPolar();this->magPhaseValid=0;}
	return this->phaseBuffer;
}

float* FFT::getPhaseDashDash(){
	if (this->pddBufferValid==1) {
		for (int k=0;k<this->fftSize;k++) {
			this->phaseDD[k] = phaseFFT()[k]-2*this->phaseM1Buffer[k]+this->phaseM2Buffer[k];
		}
		this->pddBufferValid=0;
	}
	return this->phaseDD;
}
/*
float* FFT::getPhaseDash(){
	if (!pdBufferValid) {
		for (int k=0;k<this->fftSize;k++) {
			this->phaseD[k] = princarg(phaseFFT()[k]-this->phaseM1Buffer[k]);
		}
		pdBufferValid=true;
	}
	return this->phaseD;
}
*/

float* FFT::getDelta(int hopSize){
	if (this->deltaBufferValid ==1) {
		for (int k=0;k<fftSize;k++) {
			this->omegaHop[k] = this->omega[k]*hopSize;
			//float om= this->omega[k];
			this->deltaBuffer[k] = this->omegaHop[k] + princarg(phaseFFT()[k]-this->phaseM1Buffer[k]-this->omegaHop[k]);
		}
		this->deltaBufferValid=0;
	}
	return this->deltaBuffer;
}

void FFT::incrementPsi(float ratio) {
	//if (this->deltaBufferValid ==1) {
	//	getDelta(hopSize);
	//}
	for (int k=0;k<fftSize;k++) {
		this->psi[k]=princarg(this->psi[k]+this->deltaBuffer[k]*ratio);
	}
}
FFT::~FFT() {
	//cout << "start desctruct" << endl;
	delete [] processBuffer;
	//cout << " desctruct1" << endl;
	delete [] this->imagfftBuffer;
	//cout << " desctruct2" << endl;
	delete [] this->realfftBuffer;
	//cout << " desctruct3" << endl;
	for (int i=1;i<6;i++) {
		delete [] this->buffers[i];
		//cout << " desctruct4-" << i<< endl;
	}

	delete[] this->phaseDD;

	//delete[] this->phaseD;
	delete[] this->deltaBuffer;
	delete[] this->omega;
	delete[] this->psi;
	//cout << "end desctruct" << endl;
}

void FFT::fftshift() {
	for (int k=0;k<fftSize/2;k++) {
		float tmp = this->processBuffer[fftSize/2+k];
		this->processBuffer[fftSize/2+k]=this->processBuffer[k];
		this->processBuffer[k]=tmp;
	}
}
