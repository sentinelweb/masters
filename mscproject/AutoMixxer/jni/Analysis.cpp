/** ********************************************************************************
 * Analysis.cpp
 *
 */
#include "Analysis.h"
#include "AutoMix.h"
#include "onset/QMFDecimate.h"
#include "onset/BlockProcessor.h"
#include <android/log.h>

const char* TAG = "AutoMixx-N-analyse";
// this is not used
intvl_t sampsFromSecs(MP3FileHandle* mp3,intvl_t intvl) {

	__android_log_print(ANDROID_LOG_DEBUG, TAG, "analyse sr: %d len:%d br:%d",mp3->synth.pcm.samplerate,mp3->synth.pcm.length,mp3->frame.header.bitrate);
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "analyse stt: %d endt:%d",intvl.start,intvl.end);
	intvl_t samps;
	samps.start=intvl.start*mp3->synth.pcm.samplerate;
	samps.end=intvl.end*mp3->synth.pcm.samplerate;

	if (intvl.end==0) {	samps.end=mp3->synth.pcm.length;}
	if (intvl.start<0) {	samps.start=mp3->synth.pcm.length-(-intvl.start)*mp3->synth.pcm.samplerate;	}
	if (intvl.end<0) {	samps.end=mp3->synth.pcm.length-(-intvl.end)*mp3->synth.pcm.samplerate;	}
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "analyse sts: %d ends:%d",samps.start,samps.end);
	return samps;
}

int findFreeSongHandle( ){
	int i = 0;
	for( i = 0; i < NUM_SD_HANDLES; i++ )	{
		if( songHandles[i] == 0 )
			return i;
	}
	return -1;
}

void releaseSongData(int songIndex){
	SongData* s = songHandles[songIndex];
	delete s;
	songHandles[songIndex] = 0;
}

SongData* initSong(MP3FileHandle* mp3) {
	int Fs=mp3->synth.pcm.samplerate;
	FFT* analysisfft = new FFT(256);
	BlockProcessor* bpAnalysis = new BlockProcessor(analysisfft,1024,32);
	SongData* songData = new SongData(bpAnalysis,Fs, 8, 100,200);
	return songData;
}

void analyseSong(MP3FileHandle* mp3,SongData* songData) {//,intvl_t secs start,end in secs
	QMFDecimate *decimator1 = new QMFDecimate(8192);
	QMFDecimate *decimator2 = new QMFDecimate(4096);
	QMFDecimate *decimator3 = new QMFDecimate(2048);
	//QMFDecimate *decimator4 = new QMFDecimate(1024);
	QMFDecimate *decimator4 =decimator3;

	songData->start = 0;//samps.start;
	songData->end = -1;//samps.end;
	songData->length = -1;//samps.end-samps.start;

	float *filterThreshs = new float[5];
	filterThreshs[0]=2;
	filterThreshs[1]=3;
	filterThreshs[2]=4;
	filterThreshs[3]=1.5;
	filterThreshs[4]=1.85;
	songData->setFilterThresholds(filterThreshs , 5 );
	songData->onsetIntervalMinimum=5;

	int tempo1 = 100;
	int readcount = 0;
	float* block = new float[ decimator1->size];
	int hopCount = 0;
	readcount =  readMp3Data(block, decimator1->size, mp3);
	while (readcount>0) {
		songData->read += readcount;
		decimator1->copyBlock(block);
		decimator1->filterAndDecimate(false);
		decimator2->copyBlock(decimator1->decimated);
		decimator2->filterAndDecimate(false);
		decimator3->copyBlock(decimator2->decimated);
		decimator3->filterAndDecimate(false);
		//decimator4->copyBlock(decimator3->decimated);
		//decimator4->filterAndDecimate(false);
		songData->bpAnalysis->setBlock(decimator4->decimated);
		songData->bpAnalysis->readcount = decimator4->size/2;

		while (songData->bpAnalysis->hasMoreBlocks()) {
			songData->bpAnalysis->newBlock();
			songData->analyseBlock();
			songData->bpAnalysis->writeBlock();

		}
		songData->bpAnalysis->prepareForNextBlock();
		readcount =  readMp3Data(block, decimator1->size, mp3);
	}
	songData->length=songData->read;
	for (int i=0;i<songData->onsetFilterThresholds->getSize();i++) {
		songData->estimateTempo(i);
		songData->correlateTempo(i);
	}

	songData->selOFIdx=-1;
	float mpccpc = 0;
	for (int i=0;i<songData->onsetFilterThresholds->getSize();i++) {
		if (songData->tempo[i]->correlationPC>mpccpc) {
			songData->selOFIdx=i;
			mpccpc=songData->tempo[i]->correlationPC;
		}
	}
	delete decimator1;
	delete decimator2;
	delete decimator3;
	//delete decimator4;
}
