/*
 * Author : robert munro
 * Testing implementation for onset detection.
 *
 */
#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <math.h>
#include <android/log.h>
#include "pj1.h"
#include "onset/FFT.h"
#include "onset/BlockProcessor.h"
#include "onset/Onset.h"
#include "onset/OnsetArray.h"
#include "onset/PVBlockProcessor.h"
#include "onset/Stats.h"
#include "onset/QMFDecimate.h"
#include "onset/IOIHistogram.h"

//#include <cstdlib>
//#include <algorithm>
#include "stdlib.h"
#include "time.h"

int test=MYTEST;

typedef struct mad_stream mad_stream;
typedef struct mad_frame mad_frame;
typedef struct mad_synth mad_synth;
//typedef struct mad_timer_t mad_timer_t;

/**
 * Struct holding the pointer to a wave file.
 */
typedef struct MP3FileHandle {
	int size;
	FILE* file;
	mad_stream stream;
	mad_frame frame;
	mad_synth synth;
	mad_timer_t timer;
	int leftSamples;
	int offset;
	unsigned char inputBuffer[INPUT_BUFFER_SIZE];
} MP3FileHandle;

/** static WaveFileHandle array **/
static MP3FileHandle* handles[100];

//FFT* fftObj;
//BlockProcessor* bp ;
//Onset* onset;

//float sfThresh;
//float cdThresh;
//float wpdThresh;
//OnsetArray* sfArr;
//OnsetArray* cdArr;
//OnsetArray* wpdArr;
FFT* stretchfft2;
PVBlockProcessor* bpPlayer2;
float inputScale=10;


QMFDecimate* decimate1;
QMFDecimate* decimate2;
QMFDecimate* decimate3;

float* tempo;
int* hopCount;

FFT** analysisfft ;
BlockProcessor** bpAnalysis;

Onset** onset ;
OnsetArray** sfArr ;
IOIHistogram** ioih;


void createObjects() {
	__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "createObjects");
	//fftObj = new FFT(1024);
	//bp = new BlockProcessor(fftObj,8192,256);
	//onset = new Onset(fftObj);
    //sfArr = new OnsetArray(10);
	//cdArr = new OnsetArray(10);
	//wpdArr = new OnsetArray(10);

	// phase vocoder
	stretchfft2 = new FFT(1024);
	bpPlayer2 = new PVBlockProcessor(stretchfft2,8192,256,1);

	// decimators
	decimate1=new QMFDecimate(16384);
	decimate2=new QMFDecimate(8192);
	decimate3=new QMFDecimate(4096);

	// analysis
	analysisfft = new FFT*[2];
	analysisfft[0] = new FFT(256);
	analysisfft[1] = new FFT(256);

	bpAnalysis = new BlockProcessor*[2];
	bpAnalysis[0] = new BlockProcessor(analysisfft[0],2048,128);
	bpAnalysis[1] = new BlockProcessor(analysisfft[1],2048,128);

	tempo = new float[2];
	hopCount = new int[2];

	onset = new Onset*[2];
	onset[0]= new Onset(analysisfft[0]);
	onset[1]= new Onset(analysisfft[1]);

	sfArr = new OnsetArray*[2];
	sfArr[0] = new OnsetArray(20);
	sfArr[1] = new OnsetArray(20);

	ioih = new IOIHistogram*[2];
	ioih[0] = new IOIHistogram(100);
	ioih[1] = new IOIHistogram(100);

}

void removeObjects() {
	__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "removeObjects");
	//delete fftObj ;
	//delete bp ;
	//delete onset ;
	//delete sfArr;
	//delete cdArr;
	//delete wpdArr;
	delete stretchfft2;
	delete bpPlayer2;
	delete decimate1;
	delete decimate2;
	delete decimate3;

	delete bpAnalysis[0] ;
	delete bpAnalysis[1] ;
	delete [] bpAnalysis ;

	delete analysisfft[0] ;
	delete analysisfft[1] ;
	delete [] analysisfft ;



	delete tempo ;

	delete onset[0] ;
	delete onset[1] ;
	delete [] onset ;

	delete sfArr[0] ;
	delete sfArr[1] ;
	delete [] sfArr ;

	delete ioih[0] ;
	delete ioih[1] ;
	delete [] ioih ;
}
/**
 * Seeks a free handle in the handles array and returns its index or -1 if no handle could be found
 */
static int findFreeHandle( ){
	int i = 0;
	for( i = 0; i < 100; i++ )	{
		if( handles[i] == 0 )
			return i;
	}
	return -1;
}


static inline void closeHandle( MP3FileHandle* handle ){
	mad_synth_finish(&handle->synth);
	mad_frame_finish(&handle->frame);
	mad_stream_finish(&handle->stream);
	fclose( handle->file );
	delete handle;
}

static inline signed short fixedToShort(mad_fixed_t Fixed){
	if(Fixed>=MAD_F_ONE) return(SHRT_MAX);
	if(Fixed<=-MAD_F_ONE) return(-SHRT_MAX);

	Fixed=Fixed>>(MAD_F_FRACBITS-15);
	return((signed short)Fixed);
}

/*
 * Should this ust read all the time?
 */
static inline int readNextFrame( MP3FileHandle* mp3 ){
	do{
		if( mp3->stream.buffer == 0 || mp3->stream.error == MAD_ERROR_BUFLEN )
		{
			int inputBufferSize = 0;
			if( mp3->stream.next_frame != 0 )
			{
				int leftOver = mp3->stream.bufend - mp3->stream.next_frame;
				for( int i = 0; i < leftOver; i++ )
					mp3->inputBuffer[i] = mp3->stream.next_frame[i];
				int readBytes = fread( mp3->inputBuffer + leftOver, 1, INPUT_BUFFER_SIZE - leftOver, mp3->file );
				if( readBytes == 0 )
					return 0;
				inputBufferSize = leftOver + readBytes;
			}
			else {
				int readBytes = fread( mp3->inputBuffer, 1, INPUT_BUFFER_SIZE, mp3->file );
				if( readBytes == 0 )
					return 0;
				inputBufferSize = readBytes;
			}
			mad_stream_buffer( &mp3->stream, mp3->inputBuffer, inputBufferSize );
			mp3->stream.error = MAD_ERROR_NONE;
		}

		if( mad_frame_decode( &mp3->frame, &mp3->stream ) )	{
			if( mp3->stream.error == MAD_ERROR_BUFLEN ||(MAD_RECOVERABLE(mp3->stream.error)))
				continue;
			else
				return 0;
		} else break;
	} while( true );

	mad_timer_add( &mp3->timer, mp3->frame.header.duration );
	mad_synth_frame( &mp3->synth, &mp3->frame );
	mp3->leftSamples = mp3->synth.pcm.length;
	mp3->offset = 0;

	return -1;
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

int readMp3Data(float* target, int size, int handle) {
	MP3FileHandle* mp3 = handles[handle];
	int idx = 0;
	int readSize = size;

	while( idx != readSize ){
		if( mp3->leftSamples > 0 )	{
			for( ; idx < readSize && mp3->offset < mp3->synth.pcm.length; mp3->leftSamples--, mp3->offset++ ){
				int value = fixedToShort(mp3->synth.pcm.samples[0][mp3->offset]);
				if( MAD_NCHANNELS(&mp3->frame.header) == 2 ){
					value += fixedToShort(mp3->synth.pcm.samples[1][mp3->offset]);
					value /= 2;
				}
				target[idx++] = value;
			}
		} else{
			int result = readNextFrame( mp3 );
			if( result == 0 )return 0;
		}
	}
	__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "open sr: %d len:%d",mp3->synth.pcm.samplerate, mp3->synth.pcm.length);

	return idx;
}
float analyseTempo(float* block, int idx,int size) {
	int Fs=44100;
	int readcount = 0;
	decimate2->copyBlock(block);
	decimate2->filterAndDecimate(false);
	decimate3->copyBlock(decimate2->decimated);
	decimate3->filterAndDecimate(false);
	bpAnalysis[idx]->setBlock(decimate3->decimated);
	bpAnalysis[idx]->readcount = decimate3->size/2;

	while (bpAnalysis[idx]->hasMoreBlocks()) {
		bpAnalysis[idx]->newBlock();
		analysisfft[idx]->applyWindow();
		analysisfft[idx]->fft();
		//cout << ":" << bpAnalysis->getTime(Fs) << " ms : "  << bpAnalysis->getSamps() << ":";
		int sfVal = onset[idx]->spectralFlux();

		sfArr[idx]->addElement(sfVal);
		//__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "onset: %d sf:%d (%d) %d", idx, sfVal, sfArr[idx]->numValues, bpAnalysis[idx]->hopCounter );
		if(sfArr[idx]->checkForOnset(5) && sfArr[idx]->markOnset(hopCount[idx],3) ){

			//cout << ":" << bpAnalysis->getTime(Fs/8) << " ms : hopcnt:" << hopCount ;
			//cout << ":X (" << sfArr->lastOnsetInterval;
			ioih[idx]->addElement(sfArr[idx]->lastOnsetInterval);
			int currentTempoHops = ioih[idx]->estimateTempo();
			tempo[idx] = bpAnalysis[idx]->getBPM(Fs/8,currentTempoHops);
			tempo[idx]=getTempoStd(tempo[idx]);
			__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "%d %d",bpAnalysis[idx]->hopCounter,bpAnalysis[idx]->getTime(Fs/8) );
			__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "onset: (%d) tempo:%d ",idx, tempo[idx] );

			//cout <<" : thops:"<< currentTempoHops <<" : ttime:"<< bpAnalysis->getTime(Fs/8,currentTempoHops)<<" : tbpm:"<< tempo1 << " : thops:"<<bpAnalysis->bpmToHops(Fs/8,tempo1);
			//cout<<"):";
			//cout << endl;
		}
		bpAnalysis[idx]->writeBlock();
		hopCount[idx]++;
	}
	//cout << "hopcount:"<<hopCount<<endl;
	bpAnalysis[idx]->prepareForNextBlock();
}

/*
void doOnsetCalculations() {

	sfArr->addElement(onset->spectralFlux());
	cdArr->addElement(onset->complexDomain());
	wpdArr->addElement(onset->weightedPhaseDeviation(false));
}
*/
extern "C" {
	JNIEXPORT jint JNICALL Java_net_robmunro_test_at1_AudioTrack1_openFile(JNIEnv *env, jobject obj, jstring file);
	JNIEXPORT jint JNICALL Java_net_robmunro_test_at1_AudioTrack1_readSamples__IILjava_nio_ShortBuffer_2I(JNIEnv *env, jobject obj, jint handle1,jint handle2, jobject buffer, jint size);
	JNIEXPORT void  JNICALL Java_net_robmunro_test_at1_AudioTrack1_closeFile(JNIEnv *env, jobject obj, jint handle);
	JNIEXPORT void  JNICALL Java_net_robmunro_project1_ProjectTest1_setFFTData(JNIEnv *env, jobject obj, jobject buffer,int size);
	JNIEXPORT jint JNICALL Java_net_robmunro_project1_ProjectTest1_getFFTData(	JNIEnv *env, jobject thiz, jshortArray data, jint size);

	JNIEXPORT void JNICALL Java_net_robmunro_project1_ProjectTest1_processBuffer2(JNIEnv *env,jobject thiz,	jobject in_buffer,jobject out_buffer,jint size) {
		//__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "processBuffer2");
		jbyte *cinput = (jbyte*)env->GetDirectBufferAddress( in_buffer );
		//__android_log_print(ANDROID_LOG_DEBUG, "at1", "buff addr(%d)\n",cinput);
		jlong size1 = env->GetDirectBufferCapacity( in_buffer);
		//__android_log_print(ANDROID_LOG_DEBUG, "at1", "buff size(%d)\n",size1);
		jbyte *coutput = (jbyte*)env->GetDirectBufferAddress( out_buffer );
		int i=0;
		//__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "read(%d)", size);
		for (i=0;i<size;i++) {
			coutput[i]=cinput[i];
		}
		//__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "fread(%d)", size);
		/*
		for(i=0;i<size;i++) {
			bp->inputBuffer[i]=coutput[i]/inputScale;
		}

		if (size<1024){
			for (int i=size;i<1024;i++) {
				bp->inputBuffer[i]=0;
			}
		}
		bp->readcount=1024;
		while (bp->hasMoreBlocks()) {
			bp->newBlock();
			fftObj->applyWindow();
			fftObj->fft();
			doOnsetCalculations();
			bp->writeBlock();
		}
		bp->prepareForNextBlock();
		*/
	}


	JNIEXPORT jint JNICALL Java_net_robmunro_project1_ProjectTest1_openFile(JNIEnv *env, jobject obj, jstring file){
		int index = findFreeHandle( );

		if( index == -1 )return -1;

		const char* fileString = env->GetStringUTFChars(file, NULL);
		FILE* fileHandle = fopen( fileString, "rb" );
		env->ReleaseStringUTFChars(file, fileString);
		if( fileHandle == 0 ) return -1;

		MP3FileHandle* mp3Handle = new MP3FileHandle( );
		mp3Handle->file = fileHandle;
		fseek( fileHandle, 0, SEEK_END);
		mp3Handle->size = ftell( fileHandle );
		rewind( fileHandle );

		mad_stream_init(&mp3Handle->stream);
		mad_frame_init(&mp3Handle->frame);
		mad_synth_init(&mp3Handle->synth);
		mad_timer_reset(&mp3Handle->timer);
		handles[index] = mp3Handle;

		return index;
	}

	JNIEXPORT jint JNICALL Java_net_robmunro_project1_ProjectTest1_readSamples__IILjava_nio_ShortBuffer_2I(JNIEnv *env, jobject obj, jint handle1, jint handle2, jobject buffer, jint size) 	{
		clock_t start = clock();

		short* outputtarget = (short*)env->GetDirectBufferAddress(buffer);
		//short* target = bpPlayer2->inputBuffer;
		//float* target = decimate->block;
		//short* target = bp->inputBuffer;
		int idx = readMp3Data(decimate1->block, size*2, handle1);
		decimate1->filterAndDecimate(false);
		analyseTempo(decimate1->decimated,0,8192);
		for (int i=0;i<size;i++) {
			outputtarget[i]=decimate1->decimated[i]/2;
		}
		idx = readMp3Data(decimate1->block, size*2, handle2);
		decimate1->filterAndDecimate(false);
		analyseTempo(decimate1->decimated,1,8192);

		//__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "readSamples: %d",idx);
		// Onset testing test code
		/*
		if ( idx >fftObj->fftSize ) {
			int i=0;
			bp->readcount=idx;
			//for(i=0;i<bp->inputBlockSize && i<idx;i++) {
				//bp->inputBuffer[i]=target[i]/inputScale;
				//bp->readcount++;
			//}
			while (bp->hasMoreBlocks()) {
				bp->newBlock();
				fftObj->applyWindow();
				fftObj->fft();
				//doOnsetCalculations();
				bp->writeBlock();
			}
			for (int i=0;i<size;i++) {
				 outputtarget[i]=bp->outputBuffer[i];
			}
			bp->prepareForNextBlock();

		}
		*/
		clock_t now = clock();
		int mseconds_elapsed = (now - start) / (CLOCKS_PER_SEC/1000.0);
		//__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "decode: %d",mseconds_elapsed);
		start = clock();

		// do stuff here
		if (tempo[0]>0 && tempo[1]>0) {
			float tempo1s=getTempoStd(tempo[0]);
			float tempo2s=getTempoStd(tempo[1]);

			float tempoRatio = tempo1s/tempo2s;
			__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "ratio: %2f",tempoRatio);
			//bpPlayer2->setTimeScaleFactor(tempoRatio);
		}

		clock_t times[8];
		clock_t tmpTimes[8];
		for (int i=0;i<9;i++) {times[i]= 0;tmpTimes[i]= 0;}
		bpPlayer2->readCount = idx;

		//__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "readSamples: %d",idx);
		if ( idx >stretchfft2->fftSize ) {

			decimate1->filterAndDecimate(false);
			bpPlayer2->setBlock(decimate1->decimated);
			bpPlayer2->readCount = size;
			times[0]+=clock()-start;
			//bpPlayer2->readCount =  sf_read_short(isf1, bpPlayer2->inputBuffer, bpPlayer2->inputBlockSize);
			while (bpPlayer2->hasMoreBlocks()) {
				tmpTimes[0]=clock();
				bpPlayer2->newBlock();
				stretchfft2->applyWindow();
				stretchfft2->fftshift();
				stretchfft2->fft();
				tmpTimes[1]=clock();times[1]+=clock()-tmpTimes[0];
				//float tval = onset->isTransient(5,1/bpResynthesis->timeScaleFactor,bpResynthesis->outputHopSize);
				//bool transient = tval<0.6;
				//cout << tval <<":"<<(transient?"transient":"steady")<<endl;
				//bpResynthesis->isTransient=transient;

				stretchfft2->getDelta(bpPlayer2->inputHopSize);//targetHopsize
				tmpTimes[2]=clock();times[2]+=clock()-tmpTimes[1];
				stretchfft2->incrementPsi(1/bpPlayer2->timeScaleFactor);//stretchRatio
				tmpTimes[3]=clock();times[3]+=clock()-tmpTimes[2];
				stretchfft2->polarToCart(stretchfft2->magBuffer,stretchfft2->psi);
				tmpTimes[4]=clock();times[4]+=clock()-tmpTimes[3];
				stretchfft2->ifft();
				stretchfft2->fftshift();
				stretchfft2->applyWindow();
				tmpTimes[5]=clock();times[5]+=clock()-tmpTimes[4];
				bpPlayer2->writeBlock();
				tmpTimes[6]=clock();times[6]+=clock()-tmpTimes[5];
			}
			//sf_write_short(osf, bpResynthesis->outputBuffer, bpResynthesis->getOutputBlockSize());
			//for (int i=0;i<bpPlayer2->getOutputBlockSize();i++) {
			//	outputBuffer[outputPos+i]+=bpPlayer2->outputBuffer[i]*5*vol2;
			//}
			//outputPos+=bpPlayer2->getOutputBlockSize();
			//vol2 = ((float)(outputPos))/ ((float)dataLen2);
			for (int i=0;i<size && i<bpPlayer2->getOutputBlockSize();i++) {//
				 outputtarget[i]+=bpPlayer2->outputBuffer[i]*2;
			}
			bpPlayer2->prepareForNextBlock();
		}
		//__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "after: ");
		now = clock();
		//__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "after1: ");
		mseconds_elapsed = (now - start) / (CLOCKS_PER_SEC/1000.0);
		//__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "after2: ");

		for (int i=0;i<7;i++) {times[i]/= (CLOCKS_PER_SEC/1000.0);}
		__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "time:: %d,dec: %d, fft:%d, delta:%d, incpsi: %d, ptoc:%d, ifft:%d, wb:%d sum:%d",mseconds_elapsed,times[0],times[1],times[2],times[3],times[4],times[5],times[6],times[0]+times[1]+times[2]+times[3]+times[4]+times[5]+times[6]);
		__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "tempo1:%d tempo2:%d", (int)tempo[0], (int)tempo[1]);
		if( idx > size*2 ) return 0;
		return size;
	}

	JNIEXPORT void  JNICALL Java_net_robmunro_project1_ProjectTest1_closeFile(JNIEnv *env, jobject obj, jint handle){
		if( handles[handle] != 0 ){
			closeHandle( handles[handle] );
			handles[handle] = 0;

		}
	}

	JNIEXPORT jint JNICALL Java_net_robmunro_project1_ProjectTest1_getFFTData(	JNIEnv *env, jobject thiz, jshortArray data, jint size){
		//__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "fft");
		short* pData = (short*) env->GetPrimitiveArrayCritical(data, NULL);
		float *absBuf = stretchfft2->absFFT();
		for (int i = 0; i < size; i++) {
			pData[i] = absBuf[i];//log(absBuf[i])*30;
		}
		env->ReleasePrimitiveArrayCritical(data, pData, 0);
		//__android_log_print(ANDROID_LOG_DEBUG, "pj1n", "fft end");
		return 0;
	}
	JNIEXPORT void JNICALL Java_net_robmunro_project1_ProjectTest1_createObjects(JNIEnv *env, jobject obj){
		createObjects();
	}

	JNIEXPORT void JNICALL Java_net_robmunro_project1_ProjectTest1_removeObjects(JNIEnv *env, jobject obj){
		removeObjects();
	}

	JNIEXPORT void JNICALL Java_net_robmunro_project1_ProjectTest1_setInputScale(JNIEnv *env, jobject obj,float val){
		inputScale=val;
	}

	JNIEXPORT jfloat JNICALL Java_net_robmunro_project1_ProjectTest1_getTempo(JNIEnv *env, jobject obj,jint idx){
		return tempo[idx];
	}

}//extern c
