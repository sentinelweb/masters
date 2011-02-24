/*
 * Test.cpp
 *
 *  Created on: 22-Jun-2010
 *      Author: robm
 */

#include <iostream>
#include "math.h"
#include <cstdlib>
#include <stdio.h>
#include <sndfile.h>
#include "gd.h";
#include "onset/FFT.h"
#include "onset/Onset.h"
#include "onset/BlockProcessor.h"
#include "onset/OnsetArray.h"
#include "onset/Lookup.h"
#include "onset/IOIHistogram.h"
#include "onset/QMFDecimate.h"
#include "onset/PVBlockProcessor.h"
#include "onset/PVBlockProcessor2.h"
#include "onset/Util.h"
#include "onset/VariableArray.h"
#include "onset/SongData.h"
#include "onset/Stats.h"
#include "Plot.h"
#include <ctime>
#include <cstring>
using namespace std;

void testIFFT(){
	int Fs=8;
	int N=8;
	float f= 2.5;
	int dataLen=8;
	FFT* testfft1 = new FFT(N);
	cout << "signal:";
	for (int n=0;n<dataLen;n++) {
		testfft1->processBuffer[n] = sin(2*PI*n*f/Fs);
		cout << testfft1->processBuffer[n]<<" : ";
	}
	cout << endl;
	//testfft1.applyWindow();
	testfft1->fft();
	cout << "real:";
	for (int n=0;n<dataLen;n++) {
		cout << testfft1->realfftBuffer[n]<<" : ";
	}
	cout << endl;
	cout << "imag:";
	for (int n=0;n<dataLen;n++) {
		cout << testfft1->imagfftBuffer[n]<<" : ";
	}
	cout << endl;
	testfft1->ifft();
	cout << "signal out:";
	for (int n=0;n<dataLen;n++) {
		cout << testfft1->processBuffer[n]<<" : ";
	}
	cout << endl;
}

void testLookup() {
		Lookup::init();
		int size=10000000;
		clock_t endwait;
		endwait = clock();
		for (int i=0;i<size;i++) {
			float rnd = rand()/float(RAND_MAX);
			float angle = -10+20*rnd;
			sin(angle);
		}
		cout << clock()-endwait << endl;
		endwait = clock();
		for (int i=0;i<size;i++) {
			float rnd = rand()/float(RAND_MAX);
			float angle = -10+20*rnd;
			Lookup::sinl(angle);
		}
		cout << clock()-endwait << endl;
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		cout << "----------------------- sin lookup ---------------------------------------------------------------------" << endl;
		for (int i=0;i<20;i++) {
			float rnd = rand()/float(RAND_MAX);
			float angle = -10+20*rnd;
			float real = sin(angle);
			float lookup = Lookup::sinl(angle);
			cout << angle <<":\t"<<real<<":\t"<<lookup << ":\t" << (real-lookup) <<endl;
		}
		cout << "----------------------- cos lookup ---------------------------------------------------------------------" << endl;
		for (int i=0;i<20;i++) {
			float rnd = rand()/float(RAND_MAX);
			float angle = -10+20*rnd;
			float real = cos(angle);
			float lookup = Lookup::cosl(angle);
			cout << angle <<":\t"<<real<<":\t"<<lookup << ":\t" << (real-lookup) <<endl;
		}
		cout << "----------------------- tan lookup ---------------------------------------------------------------------" << endl;
		for (int i=0;i<20;i++) {
			float rnd = rand()/float(RAND_MAX);
			float angle = -10+20*rnd;
			float real = tan(angle);
			float lookup = Lookup::tanl(angle);
			cout << angle <<":\t"<<real<<":\t"<<lookup << ":\t" << (real-lookup) <<endl;
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
void testPrincarg(){
	FFT testfft(8);
	cout << "--------------- princarg test -------------------------------------------------------------------" << endl;
	cout << 2 << ":" << testfft.princarg(2) << endl;
	cout << 4 << ":" << testfft.princarg(4) << endl;
	cout << 6 << ":" << testfft.princarg(6) << endl;
	cout << 8 << ":" << testfft.princarg(8) << endl;
	cout << -2 << ":" << testfft.princarg(-2) << endl;
	cout << -PI << ":" << testfft.princarg(-PI) << endl;
	cout << -4 << ":" << testfft.princarg(-4) << endl;
	cout << -6 << ":" << testfft.princarg(-6) << endl;
	cout << -8 << ":" << testfft.princarg(-8) << endl;

	float test[] = {12.9463 , -29.4158 , -14.0163,  -28.5149,  -25.3130,   20.3235,   12.2414 , -11.4920 ,  28.2883};
	Util::arrop(test,9,"op");
	float* iptest = new float[9];
	for (int i=0;i<9;i++) {iptest[i]=testfft.princarg(test[i]);}
	Util::arrop(iptest,9,"op");
	cout << "-------------------------------------------------------------------------------------------------" << endl;
}

void testContBuffer(FFT* testfft,SNDFILE* isf,SNDFILE* osf){
	///////////////// test continuous buffer //////////////////////////////
	/*
	int hopCount=0;
	int read=0;
	//short* opbuffer = new short[dataLen*32];// mod factor for test
	int hopFactor=fftLen/hopSize;
	// algo variables.
	short* buffer = new short[bufferSize];
	short* opbuffer = new short[bufferSize+fftLen];
	//for (int i=0;i<dataLen;i++) {
	//	opbuffer[i]=0;
	//}
	//int remainderLength = 0;
	short* remainder = new short[fftLen];// the remainder of the block may not all be filled
	int blockPos=0;
	while (read<dataLen) {
		//int rc = 2048 + rand()/float(RAND_MAX)*2048;
		//int readcount = sf_read_short(isf, buffer, rc);
		int readcount = sf_read_short(isf, buffer, bufferSize);
		// testing signal code
		//for (int i=0;i<bufferSize;i++) {
		//	ctr++;buffer[i]=10000*sin(2*PI*ctr/100);
			//buffer[i]=10000;
			//buffer[i]=(bufferSize-i)*4;
			////////////
			// if (i<20) {
			//	 buffer[i]=10000;
			// } else if (i>bufferSize-40) {
			//	 buffer[i]=-5000;
			// } else {
			//	 buffer[i]=0;
			// }
			//////////////
		//}
		// int readcount=bufferSize;
		// end testing signal code
		//if (!init) {
		//	blockPos=readcount;
		//	init=true;
		//}
		read+=readcount;
		//blockPos -= readcount;
		cerr<<"st: bp:" << blockPos << " readcount:"<<readcount<<endl;
		hopCount=0;
		while (blockPos+fftLen<=readcount) {// < ?
			//cerr<<"bp:"<<blockPos <<endl;
			//nextBlock
			if (blockPos<0) {
				int remStart=fftLen+blockPos-hopSize;
				for (int i=0;i+blockPos<0;i++) {
					testfft->inputBuffer[i] = remainder[remStart+i];
				}
				int buffEnd=fftLen+blockPos;
				for (int i=0;i<buffEnd;i++) {
					testfft->inputBuffer[i-blockPos] = buffer[i];
				}
			} else {
				for (int i=0;i<fftLen;i++) {
					testfft->inputBuffer[i] = buffer[blockPos+i];
				}
			}
			testfft->applyWindow();
			testfft->fft();
			testfft->ifft();
			testfft->applyWindow();
			// writeBlock
			for (int i=0;i<fftLen;i++) {
				int idx = hopCount*hopSize+i;
				if (idx<fftLen+bufferSize) {
					opbuffer[idx]+=testfft->inputBuffer[i]/hopFactor;// hopefully overlap add;
				} else {
					cout << "filled output ... shouldn't get here ..." << endl;break;
				}
			}
			hopCount++;
			blockPos+=hopSize;
		}
		sf_write_short(osf,opbuffer,bufferSize);
		// prepareForNextBlock
		int nextBlockStart=readcount-blockPos;
		for (int i=0;i<nextBlockStart;i++) {
			int idx=blockPos+i;
			remainder[i]=buffer[idx];
		}
		blockPos=blockPos-readcount;
		for (int i=0;i<fftLen;i++) {// move endbuffer to start
			opbuffer[i]=opbuffer[bufferSize+i];
		}
		for (int i=fftLen;i<fftLen+bufferSize;i++) {// move endbuffer to start
			opbuffer[i]=0;
		}
	}
	//sf_close(isf);
	//cout << "close input" << endl;
	if (osf){
		//sf_write_short(osf,opbuffer,dataLen);
		//cout << "write output" << endl;
		sf_close(osf);
		cout << "close output" << endl;
	}
	delete []opbuffer;
	*/
}

void testFFT (FFT* testfft,SNDFILE* isf,SNDFILE* osf) {
////////////// test sinewave /////////////////////////////
/*
// generated sinewave
float f1 = 400;
float f2 = 250;
short* input = new short[dataLen];
float f = f1;
int modifierIndex = fftLen*9.67;
cout << "modifierIndex:" << modifierIndex << endl;
// generate test sequence
float amplitude = 100;
for (int n=0;n<dataLen;n++) {
	//cout << "n%modifierIndex:" << n%modifierIndex << endl;
	if (n%modifierIndex==0) {
		if (f==f1) {f=f2;} else {f=f1;}
		cout << "change:" << n/hopSize << endl;
		amplitude=400;
	}
	if (amplitude>100) {
		amplitude--;
	}
	input[n] = amplitude*sin(2*PI*n*f1/Fs);
}
*/
////////////////////////////////////////////////////////////
///////////////// test signal //////////////////////////////
/*
int read=0;
short* buffer = new short[bufferSize];
short* opbuffer = new short[dataLen];
for (int i=0;i<dataLen;i++) {
	opbuffer[i]=0;
}
float compareBuffer[fftLen];
int block=0;
float* remainder = new float[hopSize];// the remainder of the block
while (read<dataLen) {
	int readcount = sf_read_short(isf, buffer, bufferSize);
	read+=readcount;
	int hop = 0;
	int hops = readcount/hopSize ;

	//cout << "hops:" << hops <<endl;
	while (hop<hops) {
		// fill input buffer
		for (int i=0;i<fftLen;i++) {
			int idx=hop*hopSize+i;
			testfft->inputBuffer[i] = buffer[idx];
		}
		//cout <<hop*hopSize <<":";
		testfft->applyWindow();
		for (int i=0;i<fftLen;i++) {
			compareBuffer[i]=testfft->inputBuffer[i];
		}
		testfft->fft();
		//onset->spectralFlux();
		cout << hop <<":" << (hopCount*hopSize)*1000/Fs << " ms : " <<  onset->spectralFlux() << " : " <<  onset->complexDomain() << " : " <<  onset->phaseDeviation() << " : " <<  onset->weightedPhaseDeviation(true) << " : " <<  onset->weightedPhaseDeviation(false)  << endl;

		testfft->ifft();
		for (int i=0;i<fftLen;i++) {
			if (compareBuffer[i]!=0) {
				float diff = (testfft->inputBuffer[i]-compareBuffer[i]);
				if (diff>1 ) {
					cout << i << ":" << diff << endl;
				}
			}
		}
		//testfft->applyWindow();
		int blockStart=block*bufferSize+hop*hopSize;
		//cout << blockStart << endl;
		for (int i=0;i<fftLen;i++) {
			int idx = blockStart+i;
			if (idx<dataLen) {
				opbuffer[idx]+=testfft->inputBuffer[i]/2;// hopefully overlap add;
			} else {
				cout << "filled output ..." << endl;break;
			}
		}
		hop++;
	}
	// store remainder for next loop
	for (int i=0;i<hopSize;i++) {
		remainder[i]=buffer[bufferSize-hopSize+i];
	}
	block++;
}

*/
}

void testQMF(){
	int size=8;
	QMFDecimate* d1 = new QMFDecimate(size);
	cout << "------------- decimate & reconstruct ----------------" << endl;
	short test[size];
	for (int i=0;i<size;i++) {
		float rnd = rand()/float(RAND_MAX);
		test[i]=(short)(rnd*70000-35000);

	}
	Util::arrop(test,size,"test::");
	d1->copyBlock(test);
	Util::arrop(d1->block,size,"d1 input::");

	d1->filterAndDecimate(true);
	Util::arrop(d1->decimated,size/2,"decimated::");
	Util::arrop(d1->residual,size/2,"residual::");

	d1->reconstruct();
	Util::arrop(d1->block,size,"d1 output::");
	FFT* testfft = new FFT(8);
	BlockProcessor* bpAnalysis = new BlockProcessor(testfft,8,2);
	bpAnalysis->setBlock(d1->block);
	Util::arrop(d1->block,size,"bp input::");
	cout << "------------- -------------------------------------" << endl ;
}

void testFFTShift(){
	cout << "------------fftshift---------------------------" << endl ;
	FFT fft(8);
	for (int i=0;i<fft.fftSize;i++) {fft.processBuffer[i]=i;}
	Util::arrop(fft.processBuffer,8,"b4");
	fft.fftshift();
	Util::arrop(fft.processBuffer,8,"af");
}

void outputOnset(OnsetArray *sfArr,float thresh,int block,IOIHistogram* ioih){
    if(sfArr->checkForOnset(thresh) && sfArr->markOnset(block,5)){
    	cout << ":X ("<<sfArr->lastOnsetInterval;
    	if (ioih!=NULL) {
    		ioih->addElement(sfArr->lastOnsetInterval);
    		cout <<" : "<<ioih->estimateTempo() ;
    	}
    	cout<<"):";
    }else{
        cout << ": :";
    }
}

void testProcess() {
	const char* ipath ="/home/robm/stevie1ch.wav";
		SF_INFO isi;
		SNDFILE* isf = sf_open(ipath,SFM_READ,&isi);
		if (!isf) {
			cout<< "couldnt open file"<< ipath << endl;
		} else {
			cout << "opened:"<<ipath<<" (" << isi.samplerate <<")"<< endl;
		}
		 sf_command (isf, SFC_SET_SCALE_FLOAT_INT_READ, NULL, 1) ;
		// output file
		const char* opath ="/home/robm/stevie_out.wav";
		SF_INFO osi;
		osi.channels=1;
		osi.samplerate=isi.samplerate;
		osi.format = SF_FORMAT_WAV |(SF_FORMAT_PCM_16 & SF_FORMAT_SUBMASK);
		SNDFILE* osf = sf_open(opath,SFM_WRITE, &osi);
		if (!osf) {
			cout<< "couldnt open file "<< opath << endl;
		} else {
			cout << "opened:"<< opath <<" (" << osi.samplerate <<")"<< endl;
		}

		SF_INFO osi1;
		osi1.channels=1;
		osi1.samplerate=isi.samplerate/4;
		osi1.format = SF_FORMAT_WAV |(SF_FORMAT_PCM_16 & SF_FORMAT_SUBMASK);
		const char* opath1 ="/home/robm/stevie_out1.wav";
		SNDFILE* osf1 = sf_open(opath1,SFM_WRITE, &osi1);
		if (!osf1) {
			cout<< "couldnt open file "<< opath1 << endl;
		} else {
			cout << "opened:"<< opath1 <<" (" << osi1.samplerate <<")"<< endl;
		}
		// block processor test
		cout << "-----------------------blockproc---------------------------------------------------------------------" << endl;
		float Fs = isi.samplerate;
		int dataLen = isi.frames;
		//int dataLen = Fs*60;
		QMFDecimate *decimator1 = new QMFDecimate(4096);
		QMFDecimate *decimator2 = new QMFDecimate(2048);
		sf_seek(isf,0,SEEK_SET);
		int read = 0;//int
		FFT* testfft = new FFT(256);
		FFT* stretchfft = new FFT(2048);
		BlockProcessor* bpAnalysis = new BlockProcessor(testfft,1024,64);
		BlockProcessor* bpResynthesis = new BlockProcessor(stretchfft,8192,128);
		Onset* onset = new Onset(testfft);//,40
		OnsetArray* sfArr = new OnsetArray(10);
		OnsetArray* cdArr = new OnsetArray(10);
		OnsetArray* wpdArr = new OnsetArray(10);
		IOIHistogram* ioih = new IOIHistogram(100);
		int hopCount =0;
		bpAnalysis->resetCounter();
		//short* myblock=new short[4096];
		int targetTempo=250;// for testing.
		int currentTempo = 127;
		int direction = -1;
		//read=isi.frames; //for testing to stop this loop
		while (read < dataLen && read < isi.frames) {//read<dataLen &&
			//bp->readcount = sf_read_short(isf, bp->inputBuf, bp->blockSize);
			bpResynthesis->readcount =  sf_read_short(isf, bpResynthesis->inputBuffer, bpResynthesis->inputBlockSize);
			// (note that bpResynthesis is the same block size as d1)
			//arrop(myblock,1024,"d2");
			decimator1->copyBlock(bpResynthesis->inputBuffer);
			decimator1->filterAndDecimate(false);
			//short tst[decimator1->size/2];
			//for (int l=0;l<decimator1->size/2;l++) {tst[l]=decimator1->decimated[l];}
			//sf_write_short(osf1, tst,decimator1->size/2);
			decimator2->copyBlock(decimator1->decimated);
			decimator2->filterAndDecimate(false);
			bpAnalysis->setBlock(decimator2->decimated);

			bpAnalysis->readcount = decimator2->size/2;
			// = readcount;
			read += bpResynthesis->readcount;//bpanal->readcount;
			/*
			while (bpAnalysis->hasMoreBlocks()) {
				bpAnalysis->newBlock();
				testfft->applyWindow();
				testfft->fft();
				//cout << ":" << bp->getTime(Fs) << " ms : "  << bp->getSamps() << ":";
				int sfVal = onset->spectralFlux();
				sfArr->addElement(sfVal);
				//cout << "sfVal:" << sfVal <<endl;;
				cdArr->addElement(onset->complexDomain());
				wpdArr->addElement(onset->weightedPhaseDeviation(false));
				//outputOnset(sfArr,4,hopCount,ioih);
				//sfArr->checkForOnset(3) // ...for use with median in checkForOnset
				if(sfArr->checkForOnset(3) && sfArr->markOnset(hopCount,5) ){//|| cdArr->checkForOnset(5)|| wpdArr->checkForOnset(5)
					cout << ":" << bpAnalysis->getTime(Fs/4) << " ms : " << hopCount ;
					cout << ":X (" << sfArr->lastOnsetInterval;
					ioih->addElement(sfArr->lastOnsetInterval);
					int currentTempoBlocks = ioih->estimateTempo();
					currentTempo = bpAnalysis->getBPM(Fs/4,currentTempoBlocks);
					cout <<" : "<< currentTempoBlocks <<" : "<< bpAnalysis->getTime(Fs/4,currentTempoBlocks)<<" : "<< currentTempo << " : "<<bpAnalysis->bpmToHops(Fs/4,currentTempo);
					cout<<"):";
					cout << endl;
				}
				//
				//outputOnset(cdArr,5,hopCount,NULL);
				//wpdArr->addElement(onset->weightedPhaseDeviation(false));
				//outputOnset(wpdArr,5,hopCount,NULL);

				testfft->ifft();
				testfft->applyWindow();
				bpAnalysis->writeBlock();
				hopCount++;
			}
			sf_write_short(osf1, bpAnalysis->outputBuffer, bpAnalysis->inputBlockSize);
			bpAnalysis->prepareForNextBlock();
			currentTempo = 125;
			*/
			///// SYNTHESIS STAGE ///////////////
			// bpResynthesis is loaded with values at the start of the loop
			if (bpResynthesis->hopCounter%4==0) {
				targetTempo+=direction;
				if (targetTempo<50) {
					direction=1;
				} else if (targetTempo>150) {
					direction=-1;
				}
			}
			float stretchRatio = ((float)currentTempo)/targetTempo;
			//float stretchRatio = 1;
			bpResynthesis->setOutputBlockSizeRatio(stretchRatio);
			bpResynthesis->inputHopSize=bpResynthesis->outputHopSize/stretchRatio;
			//int targetHopsize = bpResynthesis->outputHopSize/stretchRatio;
			//int targetHopsize = bpResynthesis->hopSize;
			cout << "stretchRatio:" << stretchRatio << ":" << bpResynthesis->outputBlockSize <<":"<<bpResynthesis->inputHopSize<<endl;
			while (bpResynthesis->hasMoreBlocks()) {
				bpResynthesis->newBlock();
				stretchfft->applyWindow();
				stretchfft->fftshift();
				stretchfft->fft();
				stretchfft->getDelta(bpResynthesis->inputHopSize);//targetHopsize
				stretchfft->incrementPsi(stretchRatio);//stretchRatio
				stretchfft->polarToCart(stretchfft->magBuffer,stretchfft->psi);
				stretchfft->ifft();
				stretchfft->fftshift();
				stretchfft->applyWindow();
				bpResynthesis->writeBlock();
				//reset The Position in the block - doesn't allow for origin (-blockPos)
				//bpResynthesis->blockPos = bpResynthesis->initialBlockPos+bpResynthesis->hopCount*targetHopsize;
			}

			cout << "hopcount:"<<bpResynthesis->hopCount<<endl;
			sf_write_short(osf, bpResynthesis->outputBuffer, bpResynthesis->outputBlockSize);
			bpResynthesis->prepareForNextBlock();

		}
		sf_close(isf);
		cout << "close input" << endl;
		if (osf){
			sf_close(osf);
			cout << "close output" << endl;
		}
		if (osf1){
			sf_close(osf1);
			cout << "close output 1" << endl;
		}
}


/***************testPVocBlock **********************************************************/

void testPVocBlock() {
	//const char* ipath ="/home/robm/Documents/uni/project/test/shadow_m.wav";
	const char* ipath ="/home/robm/Documents/uni/project/test/stevie/stevie1ch.wav";

		SF_INFO isi;
		SNDFILE* isf = sf_open(ipath,SFM_READ,&isi);
		if (!isf) {
			cout<< "couldnt open file"<< ipath << endl;
		} else {
			cout << "opened:"<<ipath<<" (" << isi.samplerate <<")"<< endl;
		}
		 sf_command (isf, SFC_SET_SCALE_FLOAT_INT_READ, NULL, 1) ;
		// output file
		const char* opath ="/home/robm/Documents/uni/project/test/out.wav";
		SF_INFO osi;
		osi.channels=1;
		osi.samplerate=isi.samplerate;
		osi.format = SF_FORMAT_WAV |(SF_FORMAT_PCM_16 & SF_FORMAT_SUBMASK);
		SNDFILE* osf = sf_open(opath,SFM_WRITE, &osi);
		if (!osf) {
			cout<< "couldnt open file "<< opath << endl;
		} else {
			cout << "opened:"<< opath <<" (" << osi.samplerate <<")"<< endl;
		}

		SF_INFO osi1;
		osi1.channels=1;
		osi1.samplerate=isi.samplerate/4;
		osi1.format = SF_FORMAT_WAV |(SF_FORMAT_PCM_16 & SF_FORMAT_SUBMASK);
		const char* opath1 ="/home/robm/Documents/uni/project/test/stevie/stevie_out1.wav";
		SNDFILE* osf1 = sf_open(opath1,SFM_WRITE, &osi1);
		if (!osf1) {
			cout<< "couldnt open file "<< opath1 << endl;
		} else {
			cout << "opened:"<< opath1 <<" (" << osi1.samplerate <<")"<< endl;
		}
		// block processor test
		cout << "-----------------------blockproc---------------------------------------------------------------------" << endl;
		float Fs = isi.samplerate;
		//int dataLen = isi.frames;
		int dataLen = Fs*10;
		//int dataLen = 64*7;
		sf_seek(isf,0,SEEK_SET);
		int read = 0;//int
		FFT* stretchfft = new FFT(2048);
		PVBlockProcessor* bpResynthesis = new PVBlockProcessor(stretchfft,8192,128,0.735);
		Onset* onset = new Onset(stretchfft);

		int hopCount =0;
		int targetTempo=250;// for testing.
		int currentTempo = 127;
		int direction = -1;
		int ctr=0;
		//read=isi.frames; //for testing to stop this loop
		while (read < dataLen && read < isi.frames) {//read<dataLen &&
			bpResynthesis->readCount =  sf_read_short(isf, bpResynthesis->inputBuffer, bpResynthesis->inputBlockSize);
			read += bpResynthesis->readCount;

			while (bpResynthesis->hasMoreBlocks()) {
				bpResynthesis->newBlock();
				stretchfft->applyWindow();
				stretchfft->fftshift();
				stretchfft->fft();
				float tval = onset->isTransient(5,1/bpResynthesis->timeScaleFactor,bpResynthesis->outputHopSize);
				bool transient = tval<0.6;
				cout << tval <<":"<<(transient?"transient":"steady")<<endl;
				//bpResynthesis->isTransient=transient;
				stretchfft->getDelta(bpResynthesis->inputHopSize);//targetHopsize
				stretchfft->incrementPsi(1/bpResynthesis->timeScaleFactor);//stretchRatio
				stretchfft->polarToCart(stretchfft->magBuffer,stretchfft->psi);
				stretchfft->ifft();
				stretchfft->fftshift();
				stretchfft->applyWindow();
				bpResynthesis->writeBlock();
			}
			cout << "hopcount:"<<bpResynthesis->hopCounter<<endl;
			//for (int i=0;i<bpResynthesis->getOutputBlockSize();i+=256) {
			//	bpResynthesis->outputBuffer[i+10]=2*i;
			//}
			/*
			for (int i=0;i<bpResynthesis->getOutputBlockSize();i++) {
				//bpResynthesis->outputBuffer[i+10]=2*i;
				if (bpResynthesis->outputBuffer[i]>10000) {
					cout << "over:" << i << ":" <<  (ctr+i) << ":" << bpResynthesis->outputBuffer[i]<<":"<<bpResynthesis->outputBuffer[i-1] << endl;
				}
			}
			ctr+=bpResynthesis->getOutputBlockSize();
			*/
			sf_write_short(osf, bpResynthesis->outputBuffer, bpResynthesis->getOutputBlockSize());
			bpResynthesis->prepareForNextBlock();
		}
		sf_close(isf);
		cout << "close input" << endl;
		if (osf){
			sf_close(osf);
			cout << "close output" << endl;
		}
		if (osf1){
			sf_close(osf1);
			cout << "close output 1" << endl;
		}
}
/*************** end testPVocBlock **********************************************************/
struct intvl_t{
	int start;
	int end;
};

intvl_t sampsFromSecs(SF_INFO isi,intvl_t intvl) {
	intvl_t samps;
	samps.start=intvl.start*isi.samplerate;
	samps.end=intvl.end*isi.samplerate;
	if (intvl.end==0) {	samps.end=isi.frames;}
	if (intvl.start<0) {	samps.start=isi.frames-(-intvl.start)*isi.samplerate;	}
	if (intvl.end<0) {	samps.end=isi.frames-(-intvl.end)*isi.samplerate;	}
	return samps;
}

float checkTempo(SNDFILE* isf,SF_INFO isi,intvl_t samps) {// start,end in secs
	//intvl_t samps = sampsFromSecs( isi, intvl);
	sf_seek(isf,samps.start,SEEK_SET);
	int Fs=isi.samplerate;
	int dataLen=samps.end-samps.start;
	QMFDecimate *decimator1 = new QMFDecimate(8192);
	QMFDecimate *decimator2 = new QMFDecimate(4096);
	QMFDecimate *decimator3 = new QMFDecimate(2048);
	FFT* analysisfft = new FFT(512);
	BlockProcessor* bpAnalysis = new BlockProcessor(analysisfft,1024,256);
	Onset* onset1 = new Onset(analysisfft);
	OnsetArray* sfArr = new OnsetArray(50);
	IOIHistogram* ioih = new IOIHistogram(100);
	int read = 0;
	int tempo1 = 100;
	int readcount = 0;
	short* block = new short[ decimator1->size];
	int hopCount = 0;
	//bpAnalysis->resetCounter();
	while (read < dataLen && read < isi.frames) {
		readcount =  sf_read_short(isf, block, decimator1->size);
		read += readcount;
		decimator1->copyBlock(block);
		decimator1->filterAndDecimate(false);
		decimator2->copyBlock(decimator1->decimated);
		decimator2->filterAndDecimate(false);
		decimator3->copyBlock(decimator2->decimated);
		decimator3->filterAndDecimate(false);
		bpAnalysis->setBlock(decimator3->decimated);
		bpAnalysis->readcount = decimator3->size/2;
		while (bpAnalysis->hasMoreBlocks()) {
			bpAnalysis->newBlock();
			analysisfft->applyWindow();
			analysisfft->fft();
			//cout << ":" << bpAnalysis->getTime(Fs) << " ms : "  << bpAnalysis->getSamps() << ":";
			int sfVal = onset1->spectralFlux();
			//cout << "spectral flux:"  << sfVal <<":"<< endl;;
			sfArr->addElement(sfVal);
			//Util::arrop(sfArr1->values, sfArr1->numValues,"sf");
			if(sfArr->checkForOnset(3) && sfArr->markOnset(hopCount,3) ){
				cout << ":" << bpAnalysis->getTime(Fs/8) << " ms : hopcnt:" << hopCount ;
				cout << ":X (" << sfArr->lastOnsetInterval;
				ioih->addElement(sfArr->lastOnsetInterval);
				int currentTempoHops = ioih->estimateTempo();
				tempo1 = bpAnalysis->getBPM(Fs/8,currentTempoHops);
				cout <<" : thops:"<< currentTempoHops <<" : ttime:"<< bpAnalysis->getTime(Fs/8,currentTempoHops)<<" : tbpm:"<< tempo1 << " : thops:"<<bpAnalysis->bpmToHops(Fs/8,tempo1) << " : total hops:"<<bpAnalysis->hopCounter;
				cout<<"):";
				cout << endl;
			}
			//analysisfft->ifft();
			//analysisfft->applyWindow();
			bpAnalysis->writeBlock();
			hopCount++;
		}
		//cout << "hopcount:"<<hopCount<<endl;
		bpAnalysis->prepareForNextBlock();
	}
	delete decimator1;
	delete decimator2;
	delete decimator3;
	delete analysisfft;
	delete bpAnalysis;
	delete onset1;
	delete sfArr;
	delete ioih ;
	delete [] block;
	return tempo1;

}

float getTempoStd(float tempo) {
	while (tempo>160) {
		tempo/=2;
	}
	while (tempo<80) {
		tempo*=2;
	}
	return tempo;
}

/*************** testMix ********************************************************************************** testMix *****************************************/
void testMix() {
	// 1st input file.
	const char* ipath ="/home/robm/Documents/uni/project/test/stevie1ch.wav";
	SF_INFO isi;
	SNDFILE* isf = sf_open(ipath,SFM_READ,&isi);
	if (!isf) {
		cout<< "couldnt open file"<< ipath << endl;
	} else {
		cout << "opened:"<<ipath<<" (" << isi.samplerate <<" - "<<isi.channels<<")"<< endl;
	}
	const char* ipath1 ="/home/robm/Documents/uni/project/test/shadow_m.wav";
	sf_command (isf, SFC_SET_SCALE_FLOAT_INT_READ, NULL, 1) ;
	// 2nd input file.
	SF_INFO isi1;
	SNDFILE* isf1 = sf_open(ipath1,SFM_READ,&isi1);
	if (!isf1) {
		cout<< "couldnt open file"<< ipath1 << endl;
	} else {
		cout << "opened:"<<ipath1<<" (" << isi1.samplerate <<" - "<<isi1.channels<<")"<< endl;
	}
	sf_command (isf1, SFC_SET_SCALE_FLOAT_INT_READ, NULL, 1) ;
	// output file.
	const char* opath ="/home/robm/Documents/uni/project/test/mix.wav";
	SF_INFO osi;
	osi.channels=1;
	osi.samplerate=isi.samplerate;
	osi.format = SF_FORMAT_WAV |(SF_FORMAT_PCM_16 & SF_FORMAT_SUBMASK);
	SNDFILE* osf = sf_open(opath,SFM_WRITE, &osi);
	if (!osf) {
		cout<< "couldnt open file "<< opath << endl;
	} else {
		cout << "opened:"<< opath <<" (" << osi.samplerate <<")"<< endl;
	}

	// block processor test
	cout << "-----------------------blockproc---------------------------------------------------------------------" << endl;
	//float Fs = isi.samplerate;
	//sf_seek(isf,0,SEEK_SET);
	int read = 0;

	intvl_t song1int; song1int.start=-40;song1int.end=0;
	intvl_t song1samps=sampsFromSecs(isi,song1int);
	int tempo1 = checkTempo(isf,isi,song1samps);
	float tempo1s=getTempoStd(tempo1);
	int dataLen1=song1samps.end-song1samps.start;

	intvl_t song2int; song2int.start=0;song2int.end=40;
	intvl_t song2samps=sampsFromSecs(isi1,song2int);
	int tempo2 = checkTempo(isf1,isi1,song2samps);
	float tempo2s=getTempoStd(tempo2);
	int dataLen2=song2samps.end-song2samps.start;

	float tempoRatio = tempo1s/tempo2s;
	cout << "tempo1:" << tempo1<< ":"<< tempo1s << " tempo2:" << tempo2 << ":"<< tempo2s<< " ratio:" << tempoRatio << endl;

	int opbufsz=max(dataLen1,(int)(dataLen2))+16384;
	cout <<"opbuffsz:"<< opbufsz <<endl;
	short* outputBuffer = new short[opbufsz];

	FFT* stretchfft1 = new FFT(2048);
	FFT* stretchfft2 = new FFT(2048);
	PVBlockProcessor* bpPlayer1 = new PVBlockProcessor(stretchfft1,8192,128,1);
	PVBlockProcessor* bpPlayer2 = new PVBlockProcessor(stretchfft2,8192,256,tempoRatio);
	sf_seek(isf,song1samps.start,SEEK_SET);

	float vol1 = 1;
	float vol2 = 0;

	read=0;
	// just write out the first song segment into output buffer..

	while (read < dataLen1) {
		bpPlayer1->readCount =  sf_read_short(isf, bpPlayer1->inputBuffer, bpPlayer1->inputBlockSize);
		for (int i=0;i<bpPlayer1->inputBlockSize;i++) {
			outputBuffer[read+i]=bpPlayer1->inputBuffer[i]*vol1;
		}
		read+=bpPlayer1->readCount;
		// dont need as song is fading out - may have to do some measurement.
		vol1 = ((float)(dataLen1-read))/ ((float)dataLen1);
	}

	cout<< "mix song 2" << endl;
	int outputPos=0;
	sf_seek(isf1,0,SEEK_SET);
	while (outputPos < dataLen2) {//read<dataLen &&
		bpPlayer2->readCount =  sf_read_short(isf1, bpPlayer2->inputBuffer, bpPlayer2->inputBlockSize);
		read += bpPlayer2->readCount;
		cout <<"read:"<< read << endl;
		while (bpPlayer2->hasMoreBlocks()) {
			bpPlayer2->newBlock();
			stretchfft2->applyWindow();
			stretchfft2->fftshift();
			stretchfft2->fft();
			//float tval = onset->isTransient(5,1/bpResynthesis->timeScaleFactor,bpResynthesis->outputHopSize);
			//bool transient = tval<0.6;
			//cout << tval <<":"<<(transient?"transient":"steady")<<endl;
			//bpResynthesis->isTransient=transient;
			stretchfft2->getDelta(bpPlayer2->inputHopSize);//targetHopsize
			stretchfft2->incrementPsi(1/bpPlayer2->timeScaleFactor);//stretchRatio
			stretchfft2->polarToCart(stretchfft2->magBuffer,stretchfft2->psi);
			stretchfft2->ifft();
			stretchfft2->fftshift();
			stretchfft2->applyWindow();
			bpPlayer2->writeBlock();
		}
		//sf_write_short(osf, bpResynthesis->outputBuffer, bpResynthesis->getOutputBlockSize());
		for (int i=0;i<bpPlayer2->getOutputBlockSize();i++) {
			outputBuffer[outputPos+i]+=bpPlayer2->outputBuffer[i]*5*vol2;
		}
		outputPos+=bpPlayer2->getOutputBlockSize();
		vol2 = ((float)(outputPos))/ ((float)dataLen2);
		cout <<"op:"<< outputPos << endl;
		bpPlayer2->prepareForNextBlock();
		cout <<"op1:"<< read << endl;
	}
	cout<< "dump out: oppos:" << outputPos << " read:" << read << endl;
	sf_write_short(osf, outputBuffer, outputPos);
	float ts = bpPlayer2->timeScaleFactor;
	// now slow back down to 1
	while (ts!=1) {//read<dataLen &&       ///bpPlayer2->timeScaleFactor
		bpPlayer2->readCount =  sf_read_short(isf1, bpPlayer2->inputBuffer, bpPlayer2->inputBlockSize);
		read += bpPlayer2->readCount;
		cout <<"read:"<< read << endl;
		while (bpPlayer2->hasMoreBlocks()) {
			bpPlayer2->newBlock();
			stretchfft2->applyWindow();
			stretchfft2->fftshift();
			stretchfft2->fft();
			//float tval = onset->isTransient(5,bpPlayer2->timeScaleFactor,bpPlayer2->outputHopSize);
			//bool transient = tval<0.6;
			//cout << tval <<":"<<(transient?"transient":"steady")<<endl;
			//bpPlayer2->doTransient(transient);
			stretchfft2->getDelta(bpPlayer2->inputHopSize);//targetHopsize
			stretchfft2->incrementPsi(1/bpPlayer2->timeScaleFactor);//stretchRatio
			stretchfft2->polarToCart(stretchfft2->magBuffer,stretchfft2->psi);
			stretchfft2->ifft();
			stretchfft2->fftshift();
			stretchfft2->applyWindow();
			bpPlayer2->writeBlock();
		}
		for (int i=0;i<bpPlayer2->getOutputBlockSize();i++) {
			bpPlayer2->outputBuffer[i]+=bpPlayer2->outputBuffer[i]*5;
		}
		sf_write_short(osf, bpPlayer2->outputBuffer, bpPlayer2->getOutputBlockSize());

		//float ts = bpPlayer2->timeScaleFactor;
		cout <<"old ts:"<<ts;
		//ts = ts+(1-ts)*0.02;
		if (ts>1) {ts-=0.001;}
		if (ts<1) {ts+=0.001;}
		if (ts<1.001 && ts>0.999) {ts=1;}
		cout <<" new ts:"<<ts<<endl;
		//float newInputHopSize=bpPlayer2->outputHopSize*ts;
		//bpPlayer2->blockPos-=bpPlayer2->inputHopSize;
		//bpPlayer2->blockPos+=newInputHopSize;
		bpPlayer2->prepareForNextBlock();
		//bpPlayer2->setTimeScaleFactor(ts);
		//bpPlayer2->blockPos-=bpPlayer2->inputHopSize;
		cout <<"set ts:"<<ts<<endl;

	}

	// close at end..
	sf_close(isf);
	cout << "close input" << endl;
	sf_close(isf);
	cout << "close input1" << endl;
	if (osf){
		sf_close(osf);
		cout << "close output" << endl;
	}
	delete bpPlayer1 ;
	cout << "close bpPlayer1" << endl;
	delete stretchfft1 ;
	cout << "close stretchfft1" << endl;
	delete bpPlayer2 ;
	cout << "close bpPlayer2" << endl;
	delete stretchfft2 ;
	cout << "close stretchfft2" << endl;

	delete [] outputBuffer;
	cout << "close outputBuffer" << endl;
}
/*************** testMix 2 *********************************************************************************** testMix 2 ****************************************/

SongData* analyseSong(SNDFILE* isf,SF_INFO isi,intvl_t secs) {// start,end in secs
	//intvl_t samps = sampsFromSecs( isi, intvl);
	intvl_t samps=sampsFromSecs(isi,secs);
	sf_seek(isf,samps.start,SEEK_SET);
	int Fs=isi.samplerate;
	//int dataLen=samps.end-samps.start;
	QMFDecimate *decimator1 = new QMFDecimate(8192);
	QMFDecimate *decimator2 = new QMFDecimate(4096);
	QMFDecimate *decimator3 = new QMFDecimate(2048);
	//QMFDecimate *decimator4 = new QMFDecimate(1024);
	QMFDecimate *decimator4 =decimator3;
	FFT* analysisfft = new FFT(256);
	BlockProcessor* bpAnalysis = new BlockProcessor(analysisfft,1024,32);
	SongData* songData = new SongData(bpAnalysis,Fs, 8, 50,200 	);
	songData->start = samps.start;
	songData->end = samps.end;
	songData->length = samps.end-samps.start;

	//songData->onsetFilterThreshold=3;
	const int fsz= 4;
	float *filterThreshs = new float[fsz];
	filterThreshs[0]=2;
	filterThreshs[1]=3;
	filterThreshs[2]=4;
	filterThreshs[3]=5;
	//filterThreshs[4]=1.85;
	//filterThreshs[5]=1.85;
	//filterThreshs[6]=1.5;

	songData->setFilterThresholds(filterThreshs , fsz );
	//songData->setFilterThresholds(filterThreshs , 3 );
	songData->onsetIntervalMinimum=5;

	int read = 0;
	int tempo1 = 100;
	int readcount = 0;
	short* block = new short[ decimator1->size];
	int hopCount = 0;

	//bpAnalysis->resetCounter();
	while (read < songData->length && read < isi.frames) {
		readcount =  sf_read_short(isf, block, decimator1->size);
		read += readcount;
		decimator1->copyBlock(block);
		decimator1->filterAndDecimate(false);
		decimator2->copyBlock(decimator1->decimated);
		decimator2->filterAndDecimate(false);
		decimator3->copyBlock(decimator2->decimated);
		decimator3->filterAndDecimate(false);
		//decimator4->copyBlock(decimator3->decimated);
		//decimator4->filterAndDecimate(false);
		bpAnalysis->setBlock(decimator4->decimated);
		bpAnalysis->readcount = decimator4->size/2;
		while (bpAnalysis->hasMoreBlocks()) {
			bpAnalysis->newBlock();
			songData->analyseBlock();
			//analysisfft->ifft();
			//analysisfft->applyWindow();
			bpAnalysis->writeBlock();

		}
		//cout << "hopcount:"<<hopCount<<endl;
		bpAnalysis->prepareForNextBlock();
	}

	//songData->currentTempo=songData->getTempoStd(songData->currentTempo,160,80);
	//songData->currentTempoHops=bpAnalysis->bpmToHops(Fs/8,songData->currentTempo);

	for (int i=0;i<songData->onsetFilterThresholds->getSize();i++) {
		songData->estimateTempo(i);
		int numblocks = 20;
		int numTempoBlocksSize = songData->onsetssf->getSize()/numblocks;
		//cout << "partial tempos:" << songData->onsetFilterThresholds->get(i) << ":"<<numTempoBlocksSize<< endl;
		float tsum = 0;
		float tempos[numblocks];
		for (int j=0;j<numblocks;j++) {
			int start = j>0?(j-1)*numTempoBlocksSize:(j)*numTempoBlocksSize;
			int end = j<numblocks?(j+1)*numTempoBlocksSize:(j)*numTempoBlocksSize;
			tempos[j] = songData->estimateTempoPartial(i,start,end,30,500);
			tsum+=tempos[j];
			//cout << "intvl:" << j*numTempoBlocksSize<< "-"<< (j+1)*numTempoBlocksSize << " : " << tmpo << endl;
		}
		// find mode
		float margin=1.5;
		int modeIndex = -1;
		int modeCount = -1;
		for (int j=0;j<numblocks;j++) {
			float thisTempo=tempos[j];
			if (thisTempo>1) {
				float testCount=0;
				for (int k=0;k<numblocks;k++) {
					float testTempo=tempos[k];
					if (testTempo<thisTempo+margin && testTempo>thisTempo-margin) {
						testCount++;
					}
				}
				if (testCount>modeCount) {modeCount=testCount;modeIndex=j;}
			}
		}
		//cout << "avg tmpo:" << tsum / numblocks << endl;
		float theTempo = tsum/numblocks;
		if (modeIndex>-1) {
			theTempo=tempos[modeIndex];
		}
		songData->tempo[i]->tempoBPM=theTempo;
		songData->tempo[i]->tempoHops= songData->bpAnalysis->bpmToHops(songData->Fs/songData->decimation,songData->tempo[i]->tempoBPM);
		//cout<<songData->tempo[i]->tempoBPM << ":" ;
		songData->correlateTempo(i);//songData->currentTempoHops
//		cout << "song tempo:"<<songData->onsetFilterThresholds->get(i)<<" av:"<<songData->tempo[i]->avgTempoBPM<< " 1:"<<songData->tempo[i]->tempoBPM<<"("<<songData->tempo[i]->tempoHops<< ") c:"<<songData->tempo[i]->correlationPC<<" ("<<songData->tempo[i]->correlation<<"/"<<songData->beatDataSizes[i]<<") "<<songData->onsetssfHops[i]->getSize() <<endl;
	}
	//cout << endl;

	songData->selOFIdx=-1;
//	float mpccpc = 0;
//	for (int i=0;i<songData->onsetFilterThresholds->getSize();i++) {
//		if (songData->tempo[i]->correlationPC>mpccpc) {
//			songData->selOFIdx=i;
//			mpccpc=songData->tempo[i]->correlationPC;
//		}
//	}
	//float* score = new float[songData->onsetFilterThresholds->getSize()];
	//for (int i=0;i<songData->onsetFilterThresholds->getSize();i++) {score[i]=0;}
	bool alltoolow = true;
	for (int i=0;i<songData->onsetFilterThresholds->getSize();i++) {
		// score matches across filter values
		float thisTempo=songData->tempo[i]->tempoBPM;
		float margin=1.5;

		for (int j=0;j<songData->onsetFilterThresholds->getSize();j++) {
			float testTempo=songData->tempo[j]->tempoBPM;
			if (testTempo<thisTempo+margin && testTempo>thisTempo-margin) {
				songData->tempo[i]->score++;
			}
		}

		// dont count filtervalues with too few onsets
		if ((songData->onsetssfHops[i]->getSize()/(float)songData->beatDataSizes[i])<0.1) {
			songData->tempo[i]->score=0;
		} else {
			alltoolow = false;
		}
		//
		// weight by onsetcorrelation

		//cout << "score fv:"<<songData->onsetFilterThresholds->get(i)<<" sc:"<<songData->tempo[i]->score<<endl;;
	}
	//for (int i=0;i<songData->onsetFilterThresholds->getSize();i++) {songData->tempo[i]->score=1;}
	// if all songs dont have beat correlation then set score to 1 and weight whats there
	if (alltoolow){for (int i=0;i<songData->onsetFilterThresholds->getSize();i++) {songData->tempo[i]->score=1;}}
	// weight by onsetcorrelation
	for (int i=0;i<songData->onsetFilterThresholds->getSize();i++) {
		songData->tempo[i]->score*=(1.0*songData->tempo[i]->correlation/songData->onsetssfHops[i]->getSize()+1.0*songData->tempo[i]->correlationPC);//
	}
	float hiScore = -1;
	for (int i=0;i<songData->onsetFilterThresholds->getSize();i++) {
		if (songData->tempo[i]->score>hiScore) {
			songData->selOFIdx=i;
			hiScore=songData->tempo[i]->score;
		}
	}
//	cout << "song tempo sel:"<<songData->onsetFilterThresholds->get(songData->selOFIdx)<<" av:"<<songData->tempo[songData->selOFIdx]->avgTempoBPM<<" 1:"<<songData->tempo[songData->selOFIdx]->tempoBPM<<endl;
	//delete [] absBuf;
	//cout << "del abs:"<<endl;
	delete decimator1;
	//cout << "del abs:"<<endl;
	delete decimator2;
	//cout << "del d2:"<<endl;
	delete decimator3;
	//cout << "del d3:"<<endl;
	//delete analysisfft;
	//cout << "del fft:"<<endl;
	//delete bpAnalysis;//segfalut why??
	//cout << "del bp:"<<endl;
	delete [] block;
	delete [] filterThreshs;
	//cout << "del blk:"<<endl;
	return songData;
}

void testMix2() {
	// 1st input file.stevie1ch.wav
	//const char* ipath ="/home/robm/Documents/uni/project/test/boards_m.wav";
	const char* ipath ="/home/robm/Documents/uni/project/test/stevie1ch.wav";
	//const char* ipath ="/home/robm/Documents/uni/project/test/digitalisim_what_i_want.wav";
	//const char* ipath ="/home/robm/Documents/uni/project/test/shadow_m.wav";
	//const char* ipath ="/home/robm/Documents/uni/project/test/daft_punk_da_funk.wav";
	//const char* ipath ="/home/robm/Documents/uni/project/test/gza-duel_of_the_iron_mic.wav";
	//const char* ipath ="/home/robm/Documents/uni/project/test/james_brown_Night_Train.wav";
	//const char* ipath ="/home/robm/Documents/uni/project/test/jeff_buckley_So_Real.wav";
	//const char* ipath ="/home/robm/Documents/uni/project/test/johnny_cash_Get_Rhythm.wav";
	SF_INFO isi;
	SNDFILE* isf = sf_open(ipath,SFM_READ,&isi);
	if (!isf) {
		cout<< "couldnt open file"<< ipath << endl;
	} else {
		cout << "opened:"<<ipath<<" (" << isi.samplerate <<" - "<<isi.channels<<")"<< endl;
	}
	//const char* ipath1 = ipath;
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/boards_m.wav";// 212 - 106
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/stevie/stevie1ch.wav";// 126
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/digitalisim_what_i_want.wav"; // 270-280, 135-140
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/shadow_m.wav";//159
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/daft_punk_da_funk.wav";//112
	const char* ipath1 ="/home/robm/Documents/uni/project/test/gza-duel_of_the_iron_mic.wav";//182 - > 91
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/james_brown_Night_Train.wav";//130
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/jeff_buckley_So_Real.wav";//159
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/johnny_cash_Get_Rhythm.wav";//110
	sf_command (isf, SFC_SET_SCALE_FLOAT_INT_READ, NULL, 1) ;
	// 2nd input file.
	SF_INFO isi1;
	SNDFILE* isf1 = sf_open(ipath1,SFM_READ,&isi1);
	if (!isf1) {
		cout<< "couldnt open file"<< ipath1 << endl;
	} else {
		cout << "opened:"<<ipath1<<" (" << isi1.samplerate <<" - "<<isi1.channels<<")"<< endl;
	}
	sf_command (isf1, SFC_SET_SCALE_FLOAT_INT_READ, NULL, 1) ;
	// output file.
	const char* opath ="/home/robm/Documents/uni/project/test/mix.wav";
	SF_INFO osi;
	osi.channels=1;
	osi.samplerate=isi.samplerate;
	osi.format = SF_FORMAT_WAV |(SF_FORMAT_PCM_16 & SF_FORMAT_SUBMASK);
	SNDFILE* osf = sf_open(opath,SFM_WRITE, &osi);
	if (!osf) {
		cout<< "couldnt open file "<< opath << endl;
	} else {
		cout << "opened:"<< opath <<" (" << osi.samplerate <<")"<< endl;
	}

	// block processor test
	cout << "-----------------------blockproc---------------------------------------------------------------------" << endl;
	float Fs = isi.samplerate;
	//sf_seek(isf,0,SEEK_SET);
	int read = 0;
/*
	intvl_t song1int; song1int.start=-40;song1int.end=0;
	intvl_t song1samps=sampsFromSecs(isi,song1int);
	int tempo1 = checkTempo(isf,isi,song1samps);
	float tempo1s=getTempoStd(tempo1);
	int dataLen1=song1samps.end-song1samps.start;



	intvl_t song2int; song2int.start=0;song2int.end=40;
	intvl_t song2samps=sampsFromSecs(isi1,song2int);
	int tempo2 = checkTempo(isf1,isi1,song2samps);
	float tempo2s=getTempoStd(tempo2);
	int dataLen2=song2samps.end-song2samps.start;
*/
	intvl_t song1int; song1int.start=-40;song1int.end=-0;
	SongData* song1 = analyseSong(isf,isi,song1int);

	intvl_t song2int; song2int.start=0;song2int.end=80;
	SongData* song2 = analyseSong(isf1,isi1,song2int);
	intvl_t samps2=sampsFromSecs(isi1,song2int);
	//Plot::plot(song1,"/home/robm/song1");
	//Plot::plot(song2,"/home/robm/song2");
	//return;
	//int dataLen2=Fs*20*2;
	//int dataLen2=8192*5;
	//cout <<"dataLen2:"<< dataLen2 <<":"<< 8192*4 <<endl;

	 //song1->currentTempo=106;
	// song2->currentTempo=158;
	//float tempoRatio = song2->currentTempo/ song1->currentTempo;
	float tempo2=song2->tempo[song2->selOFIdx]->avgTempoBPM;
	float tempo1=song1->tempo[song1->selOFIdx]->avgTempoBPM;
	tempo2 = SongData::getClosestTempo(tempo1,tempo2);
	float tempoRatio = tempo2/ tempo1;
	cout << "tempo1:" <<  tempo1 << " tempo2:" << tempo2 << " ratio:" << tempoRatio << endl;

	float song_pmx1 = song1->peak->max();float song_pmx2 = song2->peak->max();
	float song_pax1 = song1->peak->mean();float song_pax2 = song2->peak->mean();
	float pmxvolRatio  = song_pmx2/song_pmx1;
	float paxvolRatio  = song_pax2/song_pax1;
	cout << "pmax:1:" <<  song_pmx1/(float)32768 << " 2:" <<song_pmx2/(float)32768 << " ratio:" << song_pmx2/song_pmx1 << endl;
	cout << "pavg:1:" <<  song_pax1/(float)32768 << " 2:" <<song_pax2/(float)32768 << " ratio:" << song_pax2/song_pax1 << endl;

	float song_mmx1 = song1->avg->max();float song_mmx2 = song2->avg->max();
	float song_mma1 = song1->avg->mean();float song_mma2 = song2->avg->mean();
	float mmxvolRatio  = song_mmx2/song_mmx1;
	float mmavolRatio  = song_mma2/song_mma1;
	cout << "ratios: peakmax:" <<  song_pmx1/song_pmx2 << " peakavg:" << song_pax1/song_pax2 << " avgmax:"  <<  song_mmx1/song_mmx2 << " avgavg:"  <<  song_mma1/song_mma2 << endl;

	cout << "amax:1:" <<  song_mmx1/(float)32768 << " 2:" <<song_mmx2/(float)32768 << " ratio:" << mmxvolRatio << endl;
	cout << "aavg:1:" <<  song_mma1/(float)32768 << " 2:" <<song_mma2/(float)32768 << " ratio:" << mmavolRatio << endl;
	float useVolRatio=(paxvolRatio+mmavolRatio)/2;
	float vol1=1,vol2=1;
	if (useVolRatio<1) {
		vol1 = useVolRatio;
		vol2 = 1;
	} else {
		vol1 = 1;
		vol2 = useVolRatio;
	}
	//vol1=0.0;
	int opbufsz=max((int)song1->length,(int)song2->length)+16384;
	cout <<"opbuffsz:"<< opbufsz <<endl;
	short* outputBuffer = new short[opbufsz];

	FFT* stretchfft1 = new FFT(2048);
	FFT* stretchfft2 = new FFT(1024);
	PVBlockProcessor2* bpPlayer1 = new PVBlockProcessor2(stretchfft1,8192,8192*2,512,1);
	PVBlockProcessor2* bpPlayer2 = new PVBlockProcessor2(stretchfft2,8192,8192*4,256,1.5);//tempoRatio


	sf_seek(isf,song1->start,SEEK_SET);
	int currBeat1=0;
	BeatData** bd1=song1->beatData[song1->selOFIdx];
	for (int i=0;i<song1->beatDataSizes[song1->selOFIdx];i++) {
		cout << " B1:"<<bd1[i]->pos;
	}
	cout << endl;
	int currBeat2=0;
	BeatData** bd2=song2->beatData[song2->selOFIdx];
	for (int i=0;i<song2->beatDataSizes[song2->selOFIdx];i++) {
		cout << " B2:"<<bd2[i]->pos;
	}
	cout << endl;
	read=0;
	// just write out the first song segment into output buffer..
	short* buffer = new short[(int)(8192*(tempoRatio>1?tempoRatio:1))+16384];
	int readCount = 0;
	int inHopCtr = 0;

	while (read < song1->length) {
		int readCount =  sf_read_short(isf, buffer, bpPlayer1->inputBlockSize);
		for (int i=0;i<bpPlayer1->inputBlockSize;i++) {
			outputBuffer[read+i]=buffer[i]*vol1;
			if (i%512==0){inHopCtr++;}
			//while (currBeat1<song1->beatDataSizes[song1->selOFIdx] && inHopCtr>bd1[currBeat1]->pos ) {
			//	currBeat1++;
			//	cout << "beat:"<<currBeat1<<endl;
			//}
		}

		read+=readCount;
		// dont need as song is fading out - may have to do some measurement.
		//vol1 = ((float)(dataLen1-read))/ ((float)dataLen1);
	}
	OnsetArray* inputVolArray = new OnsetArray(100);//~ 5 sec
	OnsetArray* outputVolArray = new OnsetArray(100);//~ 5 sec
	cout<< "mix song 2" << endl;
	int outputPos=0;
	read=0;
	//sf_seek(isf1,samps2.start,SEEK_SET);
	int baseInputHopSize =bpPlayer2->inputHopSize;
	float ts = bpPlayer2->timeScaleFactor;
	bool finished=false;
	float pvCompensateVol=0;
	float* ramp=new float[10];
	float sumPeakInput=0,sumPeakOutput=0,peakCtr=0;
	//for (float ratio=0.5;ratio<=2;ratio+=0.05) {
			sf_seek(isf1,samps2.start,SEEK_SET);
			 //bpPlayer2->setTimeScaleFactor(ratio);
			 outputPos=0;
			 sumPeakInput=0;sumPeakOutput=0;peakCtr=0;
	//while(!finished) {
		while (outputPos < song2->length) {//read<dataLen &&
			peakCtr++;
			readCount =  sf_read_short(isf1,buffer, bpPlayer2->inputBlockSize);
			float inputPeak=Stats::max(buffer,bpPlayer2->inputBlockSize);
			inputVolArray->addElement(inputPeak);
			inputVolArray->doFilter();
			sumPeakInput+=inputPeak;
			bpPlayer2->setBlock(buffer);
			readCount = bpPlayer2->inputBlockSize;
			read += readCount;
			//cout <<"read:"<< read << endl;
			//cout <<"oa:"<< bpPlayer2->checkOutputAvailable(5) << " sz:"<< bpPlayer2->outputBuffer->size << " fs:"<< bpPlayer2->outputBuffer->freeSpace()<< " d:"<< bpPlayer2->outputBuffer->size - bpPlayer2->outputBuffer->freeSpace()<< " cwp:"<< bpPlayer2->outputBuffer->writePointer<< " crd:"<< bpPlayer2->outputBuffer->readPointer<<endl;
			while (bpPlayer2->hasMoreBlocks()) {
				//cout<<"inputBuffer:"<< bpPlayer2->inputBuffer->readPointer<<":" <<bpPlayer2->inputBuffer->writePointer << endl ;
				//cout<<"outputBuffer:"<< bpPlayer2->outputBuffer->readPointer<<":" <<bpPlayer2->outputBuffer->writePointer << endl ;

				bpPlayer2->newBlock();
				stretchfft2->applyWindow();
				stretchfft2->fftshift();
				stretchfft2->fft();
				//float tval = onset->isTransient(1.2,bpPlayer2->getActualTimeScaleFactor(),bpPlayer2->inputHopSize);
				//bool transient = tval>0.95;
				//cout << tval <<":"<<(transient?"transient":"steady")<<": hopSize:"<<bpPlayer2->actualInputHopSize<<endl;
				//bpPlayer2->isTransient=transient;
				stretchfft2->getDelta(bpPlayer2->actualInputHopSize);//targetHopsize
				stretchfft2->incrementPsi(bpPlayer2->getActualTimeScaleFactor());//stretchRatio
				stretchfft2->polarToCart(stretchfft2->magBuffer,stretchfft2->psi);
				stretchfft2->ifft();
				stretchfft2->fftshift();
				stretchfft2->applyWindow();
				bpPlayer2->writeBlock();
				/*
				if (currBeat1<song2->beatDataSizes[song2->selOFIdx]-1 && currBeat1<song1->beatDataSizes[song1->selOFIdx]-1) {
					if ( bpPlayer2->totalHopCounter>bd1[currBeat1]->pos ) {
						currBeat1++;
						currBeat2=currBeat1;
						//else currBeat2=1;
						cout << "beat 1:"<<currBeat1<<":"<<bd1[currBeat1]->pos<< " beat 2:"<<currBeat2<<":"<<bd2[currBeat2]->pos<<" hop:"<< bpPlayer2->totalHopCounter<<endl;
						int outputHopsRequiredToNextInputBeat = bd1[currBeat1]->pos-bpPlayer2->totalHopCounter;
						int hopsToNextOutputBeat=-1;
						hopsToNextOutputBeat=bd2[currBeat2]->pos*tempoRatio-bpPlayer2->totalHopCounter;
						float ratioHopsToNextInputAndOutputBeat = hopsToNextOutputBeat/(float)outputHopsRequiredToNextInputBeat;
						float newInputHopsSize = baseInputHopSize*ratioHopsToNextInputAndOutputBeat;
						bpPlayer2->setInputHopSize(newInputHopsSize);
						cout << bpPlayer2->inputBlockSize/bpPlayer2->inputHopSize <<" : " << outputHopsRequiredToNextInputBeat  << " : " << hopsToNextOutputBeat ;
						cout << " : " <<bd2[currBeat2]->pos*tempoRatio <<" : " << newInputHopsSize<<" : " << baseInputHopSize<<" : " << tempoRatio<<endl;
					}
					//bpPlayer2->setInputHopSize();
				}
				*/
			}
			//int blockSize = 8192;
			int blockSize = bpPlayer2->getOutputBlockSize()-1;
			float outputPeak = 0;
			while (bpPlayer2->checkOutputAvailable(blockSize)) {
				//cout << "write block:" << bpPlayer2->outputBuffer->freeSpace() << endl;
				bpPlayer2->getOutputBlock(buffer,blockSize,1);
				float outputPeakTmp=Stats::max(buffer,blockSize);
				outputVolArray->addElement(outputPeakTmp);
				outputVolArray->doFilter();

				float newpvCompensateVol = (inputVolArray->filterVal/outputVolArray->filterVal)*vol2;
				if (newpvCompensateVol*outputPeakTmp>32768 ) {

					newpvCompensateVol=32768/outputPeakTmp;
					//cout << "limit hit!!" <<endl;
				}
				float diffNewOld=pvCompensateVol-newpvCompensateVol;
				for (int i=0;i<10;i++) {
					ramp[i]=pvCompensateVol-diffNewOld/10;
				}
				for (int i=0;i<blockSize;i++) {

					outputBuffer[outputPos+i]+=buffer[i]*(i<10?ramp[i]:newpvCompensateVol);//*vol2;//
				}
				pvCompensateVol=newpvCompensateVol;
				outputPos+=blockSize;
				outputPeak=Stats::max(outputPeak,outputPeakTmp);
				//cout << "write block:" << bpPlayer2->outputBuffer->usedSpace() <<":"<<bpPlayer2->checkOutputAvailable(blockSize) << endl;
			}
			//outputVolArray->addElement(outputPeak);
			//outputVolArray->doFilter();
			sumPeakOutput+=outputPeak;
			cout << inputPeak <<":"<<outputPeak<<endl;
			//cout << "gain req:"<<inputVolArray->filterVal/outputVolArray->filterVal<<" : " <<inputVolArray->filterVal<<" : " <<outputVolArray->filterVal<<endl;
			//cout << "finished writing:" << bpPlayer2->outputBuffer->usedSpace()<< endl;
			//cout <<"blockSize:"<< bpPlayer2->getOutputBlockSize() << endl;
			//cout <<"hopount:"<< bpPlayer2->hopCounter << endl;
			//vol2 = ((float)(outputPos))/ ((float)dataLen2);
			bpPlayer2->prepareForNextBlock();
		}
		cout << "pv vol ratio:" <<bpPlayer2->timeScaleFactor<<"="<<sumPeakOutput/sumPeakInput<<endl;

		cout<< "dump out: oppos:" << outputPos << " read:" << read << endl;
		sf_write_short(osf, outputBuffer, outputPos);
		cout<< "dumped out: oppos:" << outputPos << " read:" << read << endl;
		// now slow back down to 1
	//}
		read=0;
		while (read<song2->length) {//
		//while (ts!=1) {//read<dataLen &&       ///bpPlayer2->timeScaleFactor
			readCount =  sf_read_short(isf1,buffer, bpPlayer2->inputBlockSize);
			bpPlayer2->setBlock(buffer);
			read+=readCount;
			//cout <<"read:"<< read << endl;

			while (bpPlayer2->hasMoreBlocks()) {
				bpPlayer2->newBlock();
				stretchfft2->applyWindow();
				stretchfft2->fftshift();
				stretchfft2->fft();
				//float tval = onset->isTransient(1.2,bpPlayer2->getActualTimeScaleFactor(),bpPlayer2->inputHopSize);
				//bool transient = tval>0.95;
				//cout << tval <<":"<<(transient?"transient":"steady")<<": hopSize:"<<bpPlayer2->actualInputHopSize<<endl;
				//bpPlayer2->isTransient=transient;
				stretchfft2->getDelta(bpPlayer2->actualInputHopSize);//targetHopsize
				stretchfft2->incrementPsi(bpPlayer2->getActualTimeScaleFactor());//stretchRatio
				stretchfft2->polarToCart(stretchfft2->magBuffer,stretchfft2->psi);
				stretchfft2->ifft();
				stretchfft2->fftshift();
				stretchfft2->applyWindow();
				bpPlayer2->writeBlock();
			}
			bpPlayer2->getOutputBlock(buffer,bpPlayer2->getOutputBlockSize(),1);
			for (int i=0;i<bpPlayer2->getOutputBlockSize();i++) {
				buffer[i]=buffer[i]*vol2;//*0.35;//*5;
			}
			sf_write_short(osf, buffer, bpPlayer2->getOutputBlockSize());

			bpPlayer2->prepareForNextBlock();
			ts = ts+(1-ts)*0.03;
			if (ts>1) {ts-=0.001;}
			if (ts<1) {ts+=0.001;}
			if (ts<1.001 && ts>0.999) {ts=1;}
			bpPlayer2->setTimeScaleFactor(ts);
		}
	//}
	// close at end..
	sf_close(isf);
	cout << "close input" << endl;
	sf_close(isf);
	cout << "close input1" << endl;
	if (osf){
		sf_close(osf);
		cout << "close output" << endl;
	}
	delete bpPlayer1 ;
	cout << "close bpPlayer1" << endl;
	delete stretchfft1 ;
	cout << "close stretchfft1" << endl;
	delete bpPlayer2 ;
	cout << "close bpPlayer2" << endl;
	delete stretchfft2 ;
	cout << "close stretchfft2" << endl;

	delete [] outputBuffer;
	cout << "close outputBuffer" << endl;
}
/*************** analyse *********************************************************************** analyse ****************************************************/

SongData* analyse(const char* ipath1,bool plot,bool output) {

	// 2nd input file.
	char theStr[250];
	strcpy(theStr,ipath1);
	strcat(theStr,"\0");
	SF_INFO* isi1=(SF_INFO*)malloc(sizeof(SF_INFO));
	SNDFILE* isf1 = sf_open(theStr, SFM_READ, isi1);
	if (!isf1) {
		cout<< "couldnt open file"<< ipath1 << endl;
	} else {
	//	cout << "opened:"<<ipath1<<" (" << isi1.samplerate <<" - "<<isi1.channels<<")"<< endl;
	}
	sf_command (isf1, SFC_SET_SCALE_FLOAT_INT_READ, NULL, 1) ;

	char* fileName;
	intvl_t song1int; song1int.start=0;song1int.end=-0;
	SongData* s = analyseSong(isf1,*isi1,song1int);
	s->fileName = (char*)ipath1;
	if (output) {
		for (int i=0;i<s->onsetFilterThresholds->getSize();i++) {
			cout << "song tempo:"<<s->onsetFilterThresholds->get(i)<<" av:"<<s->tempo[i]->avgTempoBPM<< " 1:"<<s->tempo[i]->tempoBPM<<"("<<s->tempo[i]->tempoHops<< ") c:"<<s->tempo[i]->correlationPC<<" ("<<s->tempo[i]->correlation<<"/"<<s->beatDataSizes[i]<<") "<<s->onsetssfHops[i]->getSize() <<" score:"<<s->tempo[i]->score<<endl;
		}
		cout << "song tempo sel:"<<s->onsetFilterThresholds->get(s->selOFIdx)<<" av:"<<s->tempo[s->selOFIdx]->avgTempoBPM<<" 1:"<<s->tempo[s->selOFIdx]->tempoBPM<<endl;
	}
	sf_close(isf1);
	free(isi1);
	if (plot) {
		Plot::plot(s,"/home/robm/song");
	}
	return s;
}

void testOnset() {
	const char* base="/home/robm/Documents/uni/project/onsetdB/";
	const char* files[][2]= {{"jaxx","mix"},
			{"dido","mix"},
			{"fiona","mix"},
			{"Jaillet15","pp"},
			{"Jaillet17","pp"},
			{"Jaillet21","npp"},
			{"Jaillet27","pp"},
			{"Jaillet29","pp"},
			{"Jaillet34","pp"},
			{"Jaillet64","npp"},
			{"Jaillet65","mix"},
			{"Jaillet66","npp"},
			{"Jaillet67","pp"},
			{"Jaillet70","mix"},
			{"Jaillet73","pp"},
			{"Jaillet74","npp"},
			{"Jaillet75","npp"},
			{"arab60s","pp"},
			{"metheny","mix"},
			{"PianoDebussy","pp"},
			{"tabla","npp"},
			{"violin","pnp"},
			{"wilco","mix"}
	};
	int sz=23;
	char fsong[120];
	char fons[120];
	cout<<"file:"<<"filenum:"<<"filetype:"<<"th:"<<"onsets actual:"<<"onsets detected:"<<"fn:"<<"fp:"<<"c:"<<"P:"<<"R:"<<"F:"<<"selTempo"<<endl;
	for (int i=0;i<sz;i++) {//targetfiles/
		strcpy(fsong,base);
		strcat(fsong,"audio/");
		strcat(fsong,files[i][0]);
		strcat(fsong,".wav");
		//cout <<"audio:" << fsong<<endl;
		SongData* s=analyse(fsong,false,false);
		strcpy(fons,base);
		strcat(fons,"targetfiles/");
		strcat(fons,files[i][0]);
		strcat(fons,".mat.ons");
		//cout <<"onset:" << fons<<endl;

		float* onsetData=new float[1000];;
		FILE* f = fopen(fons,"r");
		float val = 0,lastval=0;
		int ret = fscanf(f," %f\n",&val);
		int onsetDataCount=0;
		while(ret && val!=lastval){
			//cout << val <<":"<<endl;
			onsetData[onsetDataCount++]=val;
			lastval=val;
			ret = fscanf(f," %f\n",&val);
		}
		//cout << "numrec=" <<onsetDataCount<<endl;
		int fp=0,fn =0,match=0;
		int onsetDataPos=0,lastMatched=-1;
		float hopSizeTime=s->hopSize/(s->Fs/s->decimation);
		float marginFwd=hopSizeTime*4;
		float marginBack=hopSizeTime*4;
		float marginOffset=hopSizeTime*4;
		char* fbase=new char[50];
		strcpy(fbase,"/home/robm/song_");
		strcat(fbase,files[i][0]);
		s->fileName=(char*)files[i][0];
		//Plot::plot(s,fbase,onsetData,onsetDataCount);
		for (int j=0;j<s->onsetFilterThresholds->getSize();j++) {
			//cout <<"th:"<<s->onsetFilterThresholds->get(j)<<":"<< s->onsetssfHops[j]->getSize()<<endl;
			fp=0,fn=0,match=0,onsetDataPos=0;
			for (int k=0;k< s->onsetssfHops[j]->getSize();k++) {
				float time = s->bpAnalysis->getTime(s->Fs/s->decimation,s->onsetssfHops[j]->get(k))/(float)1000;
				while (time>onsetData[onsetDataPos] && onsetDataPos<onsetDataCount) {
					if (lastMatched!=onsetDataPos) {fn++;}
					onsetDataPos++;
				}// ff to comparative onset
				if (time>onsetData[onsetDataPos]-marginOffset-marginBack && time<onsetData[onsetDataPos]-marginOffset+marginFwd ) {
					match++;
					lastMatched=onsetDataPos;
					onsetDataPos++;//prevent matching the same onset
				} else {
					fp++;
				}
				//cout <<" t:"<<time<<" > " <<onsetData[onsetDataPos];
			}
			float P = match/(float)(match+fp);
			float R = match/(float)(match+fn);
			float F = 2*P*R/(float)(P+R);
			//cout<<endl;
			cout<<files[i][0]<<":"<<i<<":"<<files[i][1]<<":"<<s->onsetFilterThresholds->get(j)<<":"<<onsetDataCount<<":"<< s->onsetssfHops[j]->getSize()<<":"<<fn<<":"<<fp<<":"<<match<<":"<<P<<":"<<R<<":"<<F<<":"<<(s->selOFIdx==j?"X":"")<<endl;
		}
		//delete s;
	}
	/*
	FILE* f = fopen("/home/robm/Documents/uni/project/onsetdB/targetfiles/arab60s.mat.ons","r");
	float val = 0,lastval=0;
	int ret = fscanf(f," %f\n",&val);
	while(ret && val!=lastval){
		cout << val <<":"<<endl;
		lastval=val;
		ret = fscanf(f," %f\n",&val);
	}
	fclose(f);
	*/
}
/*************** tester *********************************************************************** tester ****************************************************/
// PV amplitude testing - write out time scaled version and compares the peaks relevant sections of scaleds version with original
void tester() {
	//const char* ipath1 = ipath;
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/boards_m.wav";// 212 - 106
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/stevie/stevie1ch.wav";// 126
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/digitalisim_what_i_want.wav"; // 270-280, 135-140
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/shadow_m.wav";//159
	const char* ipath1 ="/home/robm/Documents/uni/project/test/daft_punk_da_funk.wav";//112
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/gza-duel_of_the_iron_mic.wav";//182 - > 91
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/james_brown_Night_Train.wav";//130
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/jeff_buckley_So_Real.wav";//159
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/johnny_cash_Get_Rhythm.wav";//110
	// 2nd input file.
	SF_INFO isi1;
	SNDFILE* isf1 = sf_open(ipath1,SFM_READ,&isi1);
	if (!isf1) {
		cout<< "couldnt open file"<< ipath1 << endl;
	} else {
		cout << "opened:"<<ipath1<<" (" << isi1.samplerate <<" - "<<isi1.channels<<")"<< endl;
	}
	sf_command (isf1, SFC_SET_SCALE_FLOAT_INT_READ, NULL, 1) ;
	const char* opath ="/home/robm/Documents/uni/project/test/mix.wav";

	SF_INFO osi;
	osi.channels=1;
	osi.samplerate=isi1.samplerate;
	osi.format = SF_FORMAT_WAV |(SF_FORMAT_PCM_16 & SF_FORMAT_SUBMASK);
	SNDFILE* osf = sf_open(opath,SFM_WRITE, &osi);
	if (!osf) {
		cout<< "couldnt open file "<< opath << endl;
	} else {
		cout << "opened:"<< opath <<" (" << osi.samplerate <<")"<< endl;
	}
	// block processor test
	cout << "-----------------------blockproc---------------------------------------------------------------------" << endl;
	float Fs = isi1.samplerate;
	//sf_seek(isf,0,SEEK_SET);
	int read = 0;

	intvl_t song2int; song2int.start=0;song2int.end=0;
	SongData* song2 = analyseSong(isf1,isi1,song2int);
	intvl_t samps2=sampsFromSecs(isi1,song2int);
	float tempo2=song2->tempo[song2->selOFIdx]->avgTempoBPM;
	FFT* stretchfft2 = new FFT(1024);
	float tempoRatio=1.5;
	PVBlockProcessor2* bpPlayer2 = new PVBlockProcessor2(stretchfft2,8192,8192*4,256,tempoRatio);//
	float actualts=bpPlayer2->getActualTimeScaleFactor();
	read=0;
	// just write out the first song segment into output buffer..
	short* buffer = new short[(int)(8192*(tempoRatio>1?tempoRatio:1))+16384];
	int readCount = 0;
	OnsetArray* inputVolArray = new OnsetArray(2);//~ 5 sec
	OnsetArray* outputVolArray = new OnsetArray(2);//~ 5 sec
	cout<< "stretch song 2" << endl;
	int outputPos=0;
	read=0;
	//sf_seek(isf1,samps2.start,SEEK_SET);
	float sumPeakInput=0,sumPeakOutput=0,peakCtr=0;
	//float outputPeak = 0;
	//float inputPeak =0;

	sf_seek(isf1,samps2.start,SEEK_SET);
	outputPos=0;
	sumPeakInput=0;sumPeakOutput=0;peakCtr=0;
		while (read < isi1.frames) {//read<dataLen &&
			peakCtr++;
			readCount =  sf_read_short(isf1,buffer, bpPlayer2->inputBlockSize);
			float inputPeak=Stats::max(buffer,bpPlayer2->inputBlockSize);
			inputVolArray->addElement(inputPeak);
			inputVolArray->doFilter();
			sumPeakInput+=inputPeak;
			bpPlayer2->setBlock(buffer);
			readCount = bpPlayer2->inputBlockSize;
			read += readCount;
			while (bpPlayer2->hasMoreBlocks()) {
				bpPlayer2->newBlock();
				stretchfft2->applyWindow();
				stretchfft2->fftshift();
				stretchfft2->fft();
				stretchfft2->getDelta(bpPlayer2->actualInputHopSize);//targetHopsize
				stretchfft2->incrementPsi(bpPlayer2->getActualTimeScaleFactor());//stretchRatio
				stretchfft2->polarToCart(stretchfft2->magBuffer,stretchfft2->psi);
				stretchfft2->ifft();
				stretchfft2->fftshift();
				stretchfft2->applyWindow();
				bpPlayer2->writeBlock();
			}
			int blockSize = 8192;
			//int blockSize = bpPlayer2->getOutputBlockSize()-1;
			float outputPeak = 0;
			while (bpPlayer2->checkOutputAvailable(blockSize)) {
				//cout << "write block:" << bpPlayer2->outputBuffer->freeSpace() << endl;
				bpPlayer2->getOutputBlock(buffer,blockSize,1);
				float outputPeakTmp=Stats::max(buffer,blockSize);
				sf_write_short(osf, buffer, blockSize);
				outputPeak=Stats::max(outputPeak,outputPeakTmp);
			}
			outputVolArray->addElement(outputPeak);
			outputVolArray->doFilter();
			sumPeakOutput+=outputPeak;
			//cout << inputVolArray->filterVal<<":"<<outputVolArray->filterVal <<endl;

			bpPlayer2->prepareForNextBlock();
		}
		sf_close(osf);

		cout<<"start anal:"<<endl;
		SF_INFO osir;
		SNDFILE* osfr = sf_open(opath,SFM_READ,&osir);
		if (!osfr) {
			cout<< "couldnt open file"<< opath << endl;
		} else {
			cout << "opened:"<<opath<<" (" << osir.samplerate <<" - "<<osir.channels<<")"<< endl;
		}
		sf_command (osfr, SFC_SET_SCALE_FLOAT_INT_READ, NULL, 1) ;
		sf_seek(osfr,0,SEEK_SET);
		sf_seek(isf1,0,SEEK_SET);
		//int read=0;
		//float actualts=0.5;
		 read=0;
		int tstBlocksize=500000;
		int oblocsz=(int)(tstBlocksize*actualts+1);
		short* obuf = new short[oblocsz];
		short* ibuf = new short[tstBlocksize];
		while (read<isi1.frames) {
			read +=  sf_read_short(isf1,ibuf, tstBlocksize);
			float imax = Stats::max(ibuf,tstBlocksize);
			sf_read_short(osfr,obuf, oblocsz);
			float omax = Stats::max(obuf,oblocsz);
			cout<<imax<<":"<<omax<<":"<<imax/(float)omax<<endl;
		}
	// close at end..
	sf_close(isf1);


}
/*************** testPVoc *********************************************************************** testPVoc ****************************************************/
void testPVoc() {
	cout << "---------------------------------------testPVoc---------------------------------------------" << endl;
	/*
	 * Matlab commands
	 * x=[1:16]
	 * y=timeStretch(x',1)
	 * third step in loop (until the elements of grain are non-zero)
	 */
	FFT* stretchfft = new FFT(8);
	//BlockProcessor* bpResynthesis = new BlockProcessor(stretchfft,16,4);
	//for (int i=1;i<=bpResynthesis->inputBlockSize;i++) {bpResynthesis->inputBuffer[i]=i;}
	//arrop(bpResynthesis->inputBuffer,bpResynthesis->inputBlockSize,"input");
	//bpResynthesis->readcount=16;
	//while (bpResynthesis->hasMoreBlocks()) {
		//bpResynthesis->newBlock();
		float testInput[] = {0.0625,0.1250, 0.1875, 0.2500, 0.3125, 0.3750, 0.4375, 0.5000};
		for (int i=0;i<8;i++) {stretchfft->processBuffer[i]=testInput[i];}
		Util::arrop(stretchfft->processBuffer,stretchfft->fftSize,"fft input");

		float testPM1[] = {  0,-0.4093,-1.3152,2.4655,     0,-2.4655,1.3152,0.4093};
		for (int i=0;i<8;i++) {stretchfft->phaseBuffer[i]=testPM1[i];}
		Util::arrop(stretchfft->phaseBuffer,stretchfft->fftSize,"fft pm1");
		// NB: window not the same as matlab.
		float testWin[] = {0.1170,  0.4132,    0.7500, 0.9698, 0.9698,   0.7500, 0.4132, 0.1170};
		for (int i=0;i<8;i++) {stretchfft->windowBuffer[i]=testWin[i];}
		Util::arrop(stretchfft->windowBuffer,stretchfft->fftSize,"window func");

		float testPsi[] = {   0,-0.4093, -1.3152, 2.4655,      0,-2.4655, 1.3152, 0.4093};
		for (int i=0;i<8;i++) {stretchfft->psi[i]=testPsi[i];}
		Util::arrop(stretchfft->psi,stretchfft->fftSize,"psi");

		stretchfft->applyWindow();
		Util::arrop(stretchfft->processBuffer,stretchfft->fftSize,"win");
		stretchfft->fftshift();
		Util::arrop(stretchfft->processBuffer,stretchfft->fftSize,"shift");
		stretchfft->fft();
		Util::arrop(stretchfft->realfftBuffer,stretchfft->fftSize,"fft");
		Util::arrop(stretchfft->realfftBuffer,stretchfft->fftSize,"fft-r");
		Util::arrop(stretchfft->imagfftBuffer,stretchfft->fftSize,"fft-i");
		Util::arrop(stretchfft->absFFT(),stretchfft->fftSize,"fft-m");
		Util::arrop(stretchfft->phaseFFT(),stretchfft->fftSize,"fft-p");
		stretchfft->getDelta(2);//targetHopsize

		Util::arrop(stretchfft->deltaBuffer,stretchfft->fftSize,"delta");
		Util::arrop(stretchfft->omegaHop,stretchfft->fftSize,"omegaHop");

		stretchfft->incrementPsi(1);
		stretchfft->polarToCart(stretchfft->magBuffer,stretchfft->psi);
		Util::arrop(stretchfft->realfftBuffer,stretchfft->fftSize,"fft-real");
		Util::arrop(stretchfft->imagfftBuffer,stretchfft->fftSize,"fft-imag");
		stretchfft->ifft();
		Util::arrop(stretchfft->processBuffer,stretchfft->fftSize,"ifft");
		stretchfft->fftshift();
		Util::arrop(stretchfft->processBuffer,stretchfft->fftSize,"shift");
		stretchfft->applyWindow();
		Util::arrop(stretchfft->processBuffer,stretchfft->fftSize,"win");

		//bpResynthesis->writeBlock();

		//reset The Position in the block - doesn't allow for origin (-blockPos)
		//bpResynthesis->blockPos = bpResynthesis->initialBlockPos+bpResynthesis->hopCount*targetHopsize;
	//}
	//arrop(bpResynthesis->outputBuffer,bpResynthesis->outputBlockSize,"output");
}
void testDelete() {
	QMFDecimate *decimator1 = new QMFDecimate(8192);
	short* testArr = new short[8192];
	delete  decimator1;
	delete [] testArr;
}
void testVarArray() {
	VariableArray* v1 = new VariableArray(50);
	for (int i=0;i<105;i++) {
		v1->add(i+1);
	}
	cout <<"v1 sz:"<< v1->getSize() << endl;
	cout <<"v1 20:"<< v1->get(20) << endl;
	cout <<"v1 50:"<< v1->get(50) << endl;
	cout <<"v1 104:"<< v1->get(104) << endl;
	float* f=new float[105];
	v1->get(f,0,105);
	delete v1;
}

void doBulkTesting() {
	const char* base="/media/disk1/result/";

	  // whole list
	const char* files[][2]= {
			{"beatles/01 Taxman.mp3.wav.1.wav","134"},
			{"beatles/02 Eleanor Rigby.mp3.wav.1.wav","138"},
			{"beatles/03 Im Only Sleeping.mp3.wav.1.wav","104"},
			{"beatles/04 Love You To.mp3.wav.1.wav","124"},
			{"beatles/05 Here There and Everywhere.mp3.wav.1.wav","83"},
			{"beatles/06 Yellow Submarine.mp3.wav.1.wav","111"},
			{"beatles/07 She Said She Said.mp3.wav.1.wav","107"},// 106 ??
			{"beatles/08 Good Day Sunshine.mp3.wav.1.wav","117"},
			{"beatles/09 And Your Bird Can Sing.mp3.wav.1.wav","133"},
			{"beatles/10 For No One.mp3.wav.1.wav","82"},
			{"fatboy/01-sunset_strippers-falling_stars__mike_di_scala_vs_paul_smith_jnr_12_inch_edit-usf.mp3.wav.1.wav","130"},
			{"fatboy/02-the_disco_-_chapter_two_(extended_mix)-zzzz.mp3.wav.1.wav","124"},
			{"fatboy/03-the_knife-take_my_breath_away__mylo_remix-usf.mp3.wav.1.wav","126"},
			{"fatboy/04_dj_peter_presta_feat_bonse_-_totall_yhooked_(vocal_mix).mp3.wav.1.wav","126"},
			{"fatboy/05-todd_terje--kul_i_pul-mbs.mp3.wav.1.wav","114"},// causes crash?
			{"fatboy/06-discotronic-blind_vision_(vocal)-drum.mp3.wav.1.wav","129"},
			{"fatboy/07-tom_joy_feat_tony_allen-antigua__bob_sinclar_amour_kefe_remix-usf.mp3.wav.1.wav","126"},
			{"fatboy/08-darko_and_julez-wednesday_am__mix_1-ute.mp3.wav.1.wav","131"},// causes crash?
			{"fatboy/09-jake_childs-goin_down__jakes_dirty_hoe_dub-ute.mp3.wav.1.wav","128"},
			{"fatboy/10-babayaga_and_paolo_bardelli_-_this_is_speedy__club_mix-idc.mp3.wav.1.wav","132"},// causes crash?
			{"guns_roses/01 - Chinese Democracy.mp3.wav.1.wav","134"},
			{"guns_roses/02 - Shacklers Revenge.mp3.wav.1.wav","132"},
			{"guns_roses/03 - Better.mp3.wav.1.wav","98"},
			{"guns_roses/04 - Street Of Dreams.mp3.wav.1.wav","85"},
			{"guns_roses/05 - If The World.mp3.wav.1.wav","148"},
			{"guns_roses/06 - There Was A Time.mp3.wav.1.wav","90"},
			{"james_brown/1-01 PleasePleasePlease.mp3.wav.1.wav","146"},
			{"james_brown/1-02 Why Do You Do Me_.mp3.wav.1.wav","156"},
			{"james_brown/1-03 Try Me.mp3.wav.1.wav","136"},
			{"james_brown/1-04 Tell Me What I Did Wrong.mp3.wav.1.wav","120"},
			{"james_brown/1-05 Bewildered.mp3.wav.1.wav","140"},
			{"james_brown/1-06 Good Good Lovin.mp3.wav.1.wav","158"},
			{"johnny_cash/07 Big River [Mono Version].mp3.wav.1.wav","91"},
			{"johnny_cash/08 Guess Things Happen That Way [Mono Version].mp3.wav.1.wav","149"},
			{"johnny_cash/09 All Over Again.mp3.wav.1.wav","107"},
			{"johnny_cash/10 Dont Take Your Guns to Town.mp3.wav.1.wav","88"},
			{"johnny_cash/11 Five Feet High and Rising.mp3.wav.1.wav","83"},
			{"johnny_cash/12 The Rebel - Johnny Yuma.mp3.wav.1.wav","116"},
			{"junglebros/01 Straight out the Jungle.mp3.wav.1.wav","100"},
			{"junglebros/02 Whats Going On.mp3.wav.1.wav","102"},
			{"junglebros/03 Black Is Black.mp3.wav.1.wav","118"},
			{"junglebros/04 Jimbrowski.mp3.wav.1.wav","93"},
			{"junglebros/05 Im Gonna Do You.mp3.wav.1.wav","90"},
			{"junglebros/06 Ill House You.mp3.wav.1.wav","124"},
			{"junglebros/07 On the Run.mp3.wav.1.wav","112"},
			{"junglebros/08 Behind the Bush.mp3.wav.1.wav","90"},
			{"junglebros/09 Because I Got It Like That.mp3.wav.1.wav","103"},
			{"junglebros/10 Braggin & Boastin.mp3.wav.1.wav","97"},
			{"junglebros/11 Sounds of the Safari.mp3.wav.1.wav","118"},
			{"junglebros/12 Jimmys Bonus Beat.mp3.wav.1.wav","100"},
			{"junglebros/13 The Promo.mp3.wav.1.wav","99"},
			{"kraftwerk/01 - The Robots.mp3.wav.1.wav","116"},
			{"kraftwerk/02 - Spacelab.mp3.wav.1.wav","124"},
			{"kraftwerk/03 - Metropolis.mp3.wav.1.wav","122"},
			{"kraftwerk/04 - The Model.mp3.wav.1.wav","124"},
			{"kraftwerk/05 - Neon Lights.mp3.wav.1.wav","105"},
			{"kraftwerk/06 - The Man-Machine.mp3.wav.1.wav","90"},
			{"leonard_cohen/01 Is This What You Wanted.mp3.wav.1.wav","85"},
			{"leonard_cohen/02 Chelsea Hotel No. 2.mp3.wav.1.wav","88"},
			{"leonard_cohen/03 Lover Lover Lover.mp3.wav.1.wav","112"},
			{"leonard_cohen/04 Field Commander Cohen.mp3.wav.1.wav","109"},
			{"leonard_cohen/05 Why Dont You Try.mp3.wav.1.wav","80"},
			{"madonna/01 Drowned World-Substitute for Love.mp3.wav.1.wav","140"},
			{"madonna/02 Swim.mp3.wav.1.wav","100"},
			{"madonna/03 Ray of Light.mp3.wav.1.wav","128"},
			{"madonna/04 Candy Perfume Girl.mp3.wav.1.wav","97"},
			{"massive_attack/01 - Safe from Harm.mp3.wav.1.wav","82"},
			{"massive_attack/02 - Karmacoma.mp3.wav.1.wav","142"},
			{"massive_attack/03 - Angel.mp3.wav.1.wav","108"},
			{"massive_attack/04 - Teardrop.mp3.wav.1.wav","154"},
			{"massive_attack/05 - Inertia Creeps.mp3.wav.1.wav","84"},
			{"massive_attack/06 - Protection.mp3.wav.1.wav","84"},
			{"techno/01-sunset_strippers-falling_stars__mike_di_scala_vs_paul_smith_jnr_12_inch_edit-usf.mp3.wav.1.wav","131"},
			{"techno/02-the_disco_-_chapter_two_(extended_mix)-zzzz.mp3.wav.1.wav","125"},
			{"techno/03 Sunset (Bird of Prey).mp3.wav.1.wav","83"},
			{"techno/03-the_knife-take_my_breath_away__mylo_remix-usf.mp3.wav.1.wav","125"},
			{"techno/04_dj_peter_presta_feat_bonse_-_totall_yhooked_(vocal_mix).mp3.wav.1.wav","127"},
			{"techno/05-todd_terje--kul_i_pul-mbs.mp3.wav.1.wav","113"},
			{"tst/02-daft_punk-da_funk.mp3.wav.1.wav","112"},
			{"tst/02-gza-duel_of_the_iron_mic-www.bizzydownload.tk.mp3.wav.1.wav","91"},
			{"tst/03 - I Want I Want.mp3.wav.1.wav","135"},
			{"tst/04 Get Rhythm [Mono Version].mp3.wav.1.wav","110"},
			{"tst/05 - So Real.mp3.wav.1.wav","156"},
			{"tst/1-13 Night Train.mp3.wav.1.wav","131"},
			{"tst/Gorillaz_Feel_Good-_Demon_Days.mp3.wav.1.wav","140"},
			{"tst/shadow_Fixed_Income.mp3.wav.1.wav","156"},
			{"white_stripes/01_White_Stripes_-_Elephant_-_Seven_Nation_Army.mp3.wav.1.wav","124"},
			{"white_stripes/06_White_Stripes_-_Elephant_-_I_Want_To_Be_The_Boy....mp3.wav.1.wav","156"},
			{"justice/02 102 - Genesis.mp3.wav.1.wav","117"},
			{"justice/03 103 - Phantom Part 1.mp3.wav.1.wav","121"},
			{"justice/04 104 - Phantom Part 1.5.mp3.wav.1.wav","122"},
			{"justice/05 105 - D.A.N.C.E.mp3.wav.1.wav","110"},
			{"justice/06 106 - D.A.N.C.E Part 2.mp3.wav.1.wav","126"},
			{"justice/07 107 - DVNO.mp3.wav.1.wav","127"},
			{"justice/08 108 - Waters Of Nazareth (Prelude.mp3.wav.1.wav","105"},
			{"justice/09 109 - One Minute To Midnight.mp3.wav.1.wav","124"},
			{"justice/10 110 - Tthhee Ppaarrttyy.mp3.wav.1.wav","127"},
			{"justice/11 111 - Let There Be Light.mp3.wav.1.wav","128"},
			{"justice/12 112 - Stress.mp3.wav.1.wav","130"},
			{"justice/13 113 - We Are Your Friends (Repris.mp3.wav.1.wav","131"},
			{"justice/14 114 - Waters Of Nazareth.mp3.wav.1.wav","132"},
			{"justice/16 116 - Encore We Are Your Friends.mp3.wav.1.wav","106"},
			{"justice/17 117 - NY Excuse (Soulwax).mp3.wav.1.wav","140"},
			{"justice/18 118 - Final (Metallica).mp3.wav.1.wav","107"}
	};
	int sz=104;



	/*
	const char* files[][2]= {
			{"james_brown/1-05 Bewildered.mp3.wav.1.wav","140"},
			{"johnny_cash/07 Big River [Mono Version].mp3.wav.1.wav","91"},
			{"junglebros/02 Whats Going On.mp3.wav.1.wav","102"},
			{"junglebros/04 Jimbrowski.mp3.wav.1.wav","93"},
			{"leonard_cohen/01 Is This What You Wanted.mp3.wav.1.wav","85"},
			{"leonard_cohen/02 Chelsea Hotel No. 2.mp3.wav.1.wav","88"},
			{"leonard_cohen/04 Field Commander Cohen.mp3.wav.1.wav","109"},
			{"leonard_cohen/05 Why Dont You Try.mp3.wav.1.wav","80"}


		};
	int sz=8;
	*/
	int st=0;//89;
	for (int i=st;i<sz;i++) {
			char* fsong=(char*)malloc(200);
			strcpy(fsong,base);
			strcat(fsong,files[i][0]);
			/*
			SF_INFO* isi1=(SF_INFO*)malloc(sizeof(SF_INFO));
			SNDFILE* isf1 = sf_open(fsong, SFM_READ, isi1);
			if (!isf1) {
				cout<< "couldnt open file"<< fsong << endl;
			} else {
			//	cout << "opened:"<<ipath1<<" (" << isi1.samplerate <<" - "<<isi1.channels<<")"<< endl;
			}
			sf_command (isf1, SFC_SET_SCALE_FLOAT_INT_READ, NULL, 1) ;

			char* fileName;
			intvl_t song1int; song1int.start=0;song1int.end=-0;
			SongData* s = analyseSong(isf1,*isi1,song1int);
			free(isi1);

			*/
			SongData *s = analyse(fsong,false,false);
			cout << fsong <<","<< s->onsetFilterThresholds->get(s->selOFIdx)<<",";
			cout <<s->tempo[s->selOFIdx]->tempoBPM<<","<<s->tempo[s->selOFIdx]->tempoBPM;
			cout <<","<<s->tempo[s->selOFIdx]->correlation<<","<<s->beatDataSizes[s->selOFIdx];
			cout <<","<<s->onsetssfHops[s->selOFIdx]->getSize();
			int measured = atoi(files[i][1]);
			cout <<","<< measured;//<<","<< Stats::abs((s->tempo[s->selOFIdx]->tempoBPM-measured)/((float)measured)*100) <<","<< Stats::abs((s->tempo[s->selOFIdx]->avgTempoBPM-measured)/((float)measured)*100)  ;
			cout<<endl;
			free(fsong);
			delete s;
	};

}
int main() {
	cout << "---------------------------------------start---------------------------------------------" << endl;
	//testLookup();
	//testDelete();
	//testPrincarg();
	/////////////// test ifft N=8 //////////////////////////////////
	//testIFFT();
	////////////////////////////////////////////////////////////
	// first fft test function
	//testFFT(testfft,isf,osf);

	//test for continuous buffer
	//testContBuffer(testfft,isf,osf);
	//testQMF();
	//testFFTShift();
	//testPVoc();
	//FFT fft(8);
	//cout << 1.5708+fft.princarg(-0.122526-(-0.4093)-1.5708) <<endl;
	//testProcess();
	//testPVocBlock();
	//testMix();
	//testMix2();
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/boards_m.wav";// 212 - 106
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/stevie/stevie1ch.wav";// 126
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/digitalisim_what_i_want.wav"; // 270-280, 135-140
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/shadow_m.wav";//159
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/daft_punk_da_funk.wav";//112
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/gza-duel_of_the_iron_mic.wav";//182 - > 91
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/james_brown_Night_Train.wav";//130
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/jeff_buckley_So_Real.wav";//159
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/johnny_cash_Get_Rhythm.wav";//110
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/justice_D.A.N.C.E.wav";//110
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/justice_Phantom.wav";//121
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/lc_Chelsea_Hotel.wav";//88
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/lc_Is_This_What_You_Wanted.wav";//85 - interesting example - tempo is detected but doesnt correlate
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/beatles_Eleanor_Rigby.wav";//138
	//const char* ipath1 ="/home/robm/Documents/uni/project/test/beatles_taxman.wav";//137
	//const char* ipath1 ="/media/disk1/result/techno/03 Sunset (Bird of Prey).mp3.wav.1.wav";
	//const char* ipath1 =	"/media/disk1/result/beatles/01 Taxman.mp3.wav.1.wav";//,"137"
	//const char* ipath1 =	"/media/disk1/result/fatboy/01-sunset_strippers-falling_stars__mike_di_scala_vs_paul_smith_jnr_12_inch_edit-usf.mp3.wav.1.wav";
	//const char* ipath1 =	"/media/disk1/result/fatboy/02-the_disco_-_chapter_two_(extended_mix)-zzzz.mp3.wav.1.wav";
	//const char* ipath1 =	"/media/disk1/result/massive_attack/05 - Inertia Creeps.mp3.wav.1.wav";
	//const char* ipath1 =	"/media/disk1/result/johnny_cash/07 Big River [Mono Version].mp3.wav.1.wav";//91 - BAD
	//const char* ipath1 =	"/media/disk1/result/junglebros/04 Jimbrowski.mp3.wav.1.wav";//93 - bad
	//const char* ipath1 =	"/media/disk1/result/james_brown/1-05 Bewildered.mp3.wav.1.wav";//140 - BAD
	//const char* ipath1 =	"/media/disk1/result/beatles/07 She Said She Said.mp3.wav.1.wav";//106-7
	//analyse(ipath1,true,true);

	doBulkTesting();
	//tester();
	//testOnset();

	//cout << -10%3 <<endl;
	/*
	if (1) {
		cout << "1 is true" <<endl;
	} else if (0) {
		cout << "0 is true" <<endl;
	}
	*/
	//testVarArray();
	cout << "---------------------------------------end---------------------------------------------" << endl;

	return 0;
}
