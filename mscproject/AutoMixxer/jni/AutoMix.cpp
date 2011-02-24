/** ********************************************************************************
 * AutoMix.cpp
 *
 */
#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <math.h>
#include <android/log.h>
#include "AutoMix.h"
#include "Analysis.h"

#include "onset/FFT.h"
#include "onset/BlockProcessor.h"
#include "onset/Onset.h"
#include "onset/OnsetArray.h"
#include "onset/PVBlockProcessor2.h"
#include "onset/Stats.h"
#include "onset/QMFDecimate.h"
#include "onset/IOIHistogram.h"
#include "onset/SongData.h"

#include "stdlib.h"
#include "time.h"

const char* TAG1 = "AutoMixx-N";
struct sdb_t{
	jfieldID position;
	jfieldID scaleFactor;
	jfieldID nativeMP3Handle;
	jfieldID sampleRate;
	jfieldID bitRate;
	jfieldID path;
	jfieldID playState;
	jfieldID vol;
};
/* ============================== MP3 Handling ============================================================ */
/*
 * MP3 (libmad based) MP3 read function taken from : http://apistudios.com/hosted/marzec/badlogic/wordpress/?p=231
 */

/**
 * Seeks a free handle in the handles array and returns its index or -1 if no handle could be found
 */
static int findFreeHandle( ){
	int i = 0;
	for( i = 0; i < NUM_MP3_HANDLES; i++ )	{
		if( handles[i] == 0 )
			return i;
	}
	return -1;
}

static inline signed short fixedToShort(mad_fixed_t Fixed){
	if(Fixed>=MAD_F_ONE) return(SHRT_MAX);
	if(Fixed<=-MAD_F_ONE) return(-SHRT_MAX);

	Fixed=Fixed>>(MAD_F_FRACBITS-15);
	return((signed short)Fixed);
}

/*
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

int readMp3Data(float* target, int size, MP3FileHandle* mp3) {
	//MP3FileHandle* mp3 = handles[handle];
	int idx = 0;
	int readSize = size;

	while( idx < readSize ){
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
	return idx;
}

int openFile (const char* fileString) {
	int index = findFreeHandle( );

	if( index == -1 )return -1;

	//const char* fileString = env->GetStringUTFChars(file, NULL);
	FILE* fileHandle = fopen( fileString, "rb" );
	//env->ReleaseStringUTFChars(file, fileString);
	if( fileHandle == 0 ) return -1;

	MP3FileHandle* mp3Handle = new MP3FileHandle( );
	mp3Handle->file = fileHandle;
	fseek( fileHandle, 0, SEEK_END);
	mp3Handle->size = ftell( fileHandle );
	__android_log_print(ANDROID_LOG_DEBUG, TAG1, "fileSize: %d",mp3Handle->size);
	rewind( fileHandle );

	mad_stream_init(&mp3Handle->stream);
	mad_frame_init(&mp3Handle->frame);
	mad_synth_init(&mp3Handle->synth);
	mad_timer_reset(&mp3Handle->timer);
	readNextFrame(mp3Handle);
	__android_log_print(ANDROID_LOG_DEBUG, TAG1, "open sr: %d len:%d",mp3Handle->synth.pcm.samplerate,mp3Handle->synth.pcm.length);
	handles[index] = mp3Handle;

	return index;
}

static inline void closeHandle( int handleIdx ){
	if( handles[handleIdx] != 0 ){
		MP3FileHandle* handle=handles[handleIdx];
		mad_synth_finish(&handle->synth);
		mad_frame_finish(&handle->frame);
		mad_stream_finish(&handle->stream);
		fclose( handle->file );
		delete handle;
		handles[handleIdx] = 0;
	}
}

/* ============================== end MP3 Handling ============================================================ */
/* ============================== Analysis data Handling ============================================================ */


/* ============================== end Analysis data Handling ============================================================ */

/* ============================== JNI methods ============================================================ */
extern "C" {
	// called from AnalysisThread
	JNIEXPORT jint JNICALL Java_net_robmunro_automix_AnalysisThread_allocateSong(JNIEnv *env, jobject obj){
		int songIndex = findFreeSongHandle( );
		__android_log_print(ANDROID_LOG_DEBUG, TAG1, "song index: %d",songIndex);
		if (songIndex == -1 ) return -1;
		__android_log_print(ANDROID_LOG_DEBUG, TAG1, "song index1: %d",songIndex);
		return songIndex;
	}

	// called from AnalysisThread
	JNIEXPORT jint JNICALL Java_net_robmunro_automix_AnalysisThread_analyseSong(JNIEnv *env, jobject obj, jint songHandle,jstring file){
		const char* fileString = env->GetStringUTFChars(file, NULL);
		int mp3index = openFile( fileString );
		__android_log_print(ANDROID_LOG_DEBUG, TAG1, "analyse mp3idx: %d",mp3index);
		__android_log_print(ANDROID_LOG_DEBUG, TAG1, "analyse file: %s",fileString);
		env->ReleaseStringUTFChars(file, fileString);
		if (mp3index == -1 ) {
			//releaseSongData(songIndex);
			//songHandles[songIndex]=0;
			return -1;//fail
		}
		//intvl_t region;
		//region.start=0;
		//region.end=-1;
		__android_log_print(ANDROID_LOG_DEBUG, TAG1, "analyse: %d",songHandle);
		//songHandles[songHandle]=analyseSong(handles[mp3index], region);
		songHandles[songHandle]=initSong(handles[mp3index]);
		analyseSong(handles[mp3index],songHandles[songHandle]);
		closeHandle(mp3index);
		return 1;// success
	}

	JNIEXPORT jfloat JNICALL Java_net_robmunro_automix_AutoMixActivity_getField(JNIEnv *env, jobject obj, jint songHandle, jint field){
		if (songHandles[songHandle]==0) {return -1;}
		SongData* song = songHandles[songHandle];
		switch (field)  {
			case SONG_PROGRESS:return song->read/(float)song->length;
			case SONG_TEMPO:return song->tempo[song->selOFIdx]->tempoBPM;
			case SONG_AVGTEMPO:return song->tempo[song->selOFIdx]->avgTempoBPM;
			case SONG_SR:return song->Fs;
			case SONG_SECS:return song->length/song->Fs;
			case SONG_READ:return song->read;
			case SONG_PEAKVOL:return song->peak->max();
			case SONG_AVGVOL:return song->avg->max();
			case SONG_MATCHEDBEATS:return song->tempo[song->selOFIdx]->correlation;
			case SONG_NUMBEATS:return song->beatDataSizes[song->selOFIdx];
			case SONG_NUMONSETS:return song->onsetssfHops[song->selOFIdx]->getSize();
		}
	}
	sdb_t sdb_fields;
	JNIEXPORT jint JNICALL Java_net_robmunro_automix_SynthesisThread_initFields(JNIEnv *env, jobject obj, jclass _sdbclass){
		sdb_fields.position = env->GetFieldID(_sdbclass,(const char*)"playPosition",(const char*)"J");// 'J' for long ... hmmmphh
		sdb_fields.scaleFactor = env->GetFieldID(_sdbclass,(const char*)"scaleFactor",(const char*)"F");
		sdb_fields.nativeMP3Handle = env->GetFieldID(_sdbclass,(const char*)"nativeMP3Handle",(const char*)"I");
		sdb_fields.sampleRate = env->GetFieldID(_sdbclass,(const char*)"sampleRate",(const char*)"F");
		sdb_fields.bitRate = env->GetFieldID(_sdbclass,(const char*)"bitRate",(const char*)"F");
		sdb_fields.playState = env->GetFieldID(_sdbclass,(const char*)"playState",(const char*)"I");
		sdb_fields.vol = env->GetFieldID(_sdbclass,(const char*)"vol",(const char*)"F");
		//sdb_fields.path = env->GetFieldID(_sdbclass,(const char*)"path",(const char*)"Ljava/lang/String");
	}
	JNIEXPORT jint JNICALL Java_net_robmunro_automix_SynthesisThread_openMP3File(JNIEnv *env, jobject obj, jobject sdb,jstring path){
		//jstring* path = (jstring*)env->GetObjectField(sdb,sdb_fields.path);
		//const char* fileString = env->GetStringUTFChars(*path, NULL);
		const char* fileString = env->GetStringUTFChars(path, NULL);
		__android_log_print(ANDROID_LOG_DEBUG, TAG1, "openMP3File path: %s",fileString);
		int mp3Handle = openFile( fileString );
		//env->ReleaseStringUTFChars(*path, fileString);
		env->ReleaseStringUTFChars(path, fileString);
		env->SetIntField(sdb,sdb_fields.nativeMP3Handle,mp3Handle);
		env->SetFloatField(sdb,sdb_fields.sampleRate,handles[mp3Handle]->synth.pcm.samplerate);
		env->SetFloatField(sdb,sdb_fields.bitRate,handles[mp3Handle]->frame.header.bitrate);
		env->SetLongField(sdb,sdb_fields.position,0);
		return mp3Handle;
	}
	JNIEXPORT jint JNICALL Java_net_robmunro_automix_SynthesisThread_closeMP3File(JNIEnv *env, jobject obj, jobject sdb){
		jint mp3Handle = env->GetIntField(sdb,sdb_fields.nativeMP3Handle);
		closeHandle(mp3Handle);
		env->SetIntField(sdb,sdb_fields.nativeMP3Handle,-1);
		return 0;
	}
	PVBlockProcessor2* bpPlayer2;
	QMFDecimate *decimator;
	//QMFDecimate *decimator1;
	FFT* stretchfft2;

	int blockSize=8192;
	JNIEXPORT jint JNICALL Java_net_robmunro_automix_SynthesisThread_setupPlay(JNIEnv *env, jobject obj, jint _blockSize){
		blockSize = _blockSize;
		stretchfft2 = new FFT(1024);
		bpPlayer2 = new PVBlockProcessor2(stretchfft2,blockSize,blockSize*4,256,1);
		decimator = new QMFDecimate(blockSize*2);
		//decimator1 = new QMFDecimate(blockSize*2);
		__android_log_print(ANDROID_LOG_DEBUG, TAG1, "setupPlay blockSize: %d",blockSize);
		return 0;
	}
	JNIEXPORT jint JNICALL Java_net_robmunro_automix_SynthesisThread_tearDownPlay(JNIEnv *env, jobject obj){
		delete bpPlayer2;
		delete decimator;
		//delete decimator1;
		return 0;
	}
	//__Lnet_robmunro_automix_SongDataBeanLnet_robmunro_automix_SongDataBeanLjava_nio_ShortBuffer_2I
	// should be a method sig here but couldnt figure it out, seems to work without it.
	JNIEXPORT jint JNICALL Java_net_robmunro_automix_SynthesisThread_readSamples(JNIEnv *env, jobject obj, jobject thisSongBean, jobject nextSongBean, jobject buffer) {
		clock_t start = clock();
		clock_t inc = start;
		clock_t otimes[8];
		short* outputtarget = (short*)env->GetDirectBufferAddress(buffer);
		jint thisPlayState = env->GetIntField(thisSongBean,sdb_fields.playState);
		int read = 0;
		if (thisPlayState==PLAYSTATE_PLAYING) {
			jint mp3Handle1 = env->GetIntField(thisSongBean,sdb_fields.nativeMP3Handle);
			otimes[0] = clock()-inc;inc=clock();
			read = readMp3Data(decimator->block, decimator->size, handles[mp3Handle1]);
			//__android_log_print(ANDROID_LOG_DEBUG, TAG1, "readSamples blockSize: %d sz:%d",decimator->size, blockSize);
			// decimate to 22050
			decimator->filterAndDecimate(false);

			jfloat vol1 = env->GetFloatField(thisSongBean,sdb_fields.vol);
			for (int i=0;i<blockSize;i++) {
				outputtarget[i]=decimator->decimated[i]*vol1;
			}
			jlong pos = env->GetLongField(thisSongBean,sdb_fields.position);
			pos+=read;
			env->SetLongField(thisSongBean,sdb_fields.position,pos);
		} else {
			for (int i=0;i<blockSize;i++) {outputtarget[i]=0;}
		}
		otimes[1] = clock()-inc;inc=clock();

		otimes[2] = clock()-inc;inc=clock();
		jint mp3Handle2 = env->GetIntField(nextSongBean,sdb_fields.nativeMP3Handle);
		jint nextPlayState = env->GetIntField(nextSongBean,sdb_fields.playState);
		clock_t now = clock();
		//__android_log_print(ANDROID_LOG_DEBUG, TAG1, "readSamples2 nextPlayState: %d",nextPlayState);
		start = clock();
		int mseconds_elapsed = (now - start) / (CLOCKS_PER_SEC/1000.0);
		clock_t times[8];
		clock_t tmpTimes[8];
		otimes[3] = clock()-inc;inc=clock();
		jfloat vol2 = env->GetFloatField(nextSongBean,sdb_fields.vol);
		for (int i=0;i<9;i++) {times[i]= 0;tmpTimes[i]= 0;}
		//__android_log_print(ANDROID_LOG_DEBUG, TAG1, "readSamples2 nextPlayState: %d",nextPlayState);
		if (nextPlayState>=PLAYSTATE_PLAYING) {
			//__android_log_print(ANDROID_LOG_DEBUG, TAG1, "readSamples2 outputAvailable: %d",bpPlayer2->checkOutputAvailable(blockSize));

			while (!bpPlayer2->checkOutputAvailable(blockSize)) {
				int readCount = readMp3Data(decimator->block, decimator->size, handles[mp3Handle2]);
				// decimate to 22050s
				decimator->filterAndDecimate(false);
				bpPlayer2->setBlock(decimator->decimated);
				//read += readCount;
				jlong pos2 = env->GetLongField(nextSongBean,sdb_fields.position);
				pos2+=readCount;
				env->SetLongField(nextSongBean,sdb_fields.position,pos2);
				//__android_log_print(ANDROID_LOG_DEBUG, TAG1, "readSamples2 pos: %d",pos2);
				jfloat ts2 = env->GetFloatField(nextSongBean,sdb_fields.scaleFactor);
				bpPlayer2->setTimeScaleFactor(ts2);
				//cout <<"read:"<< read << endl;

				while (bpPlayer2->hasMoreBlocks()) {
					//__android_log_print(ANDROID_LOG_DEBUG,TAG1, "inputBuffer: %d : %d ",bpPlayer2->inputBuffer->readPointer,bpPlayer2->inputBuffer->writePointer);
					//__android_log_print(ANDROID_LOG_DEBUG,TAG1, "outputBuffer: %d : %d ",bpPlayer2->outputBuffer->readPointer,bpPlayer2->outputBuffer->writePointer);
					tmpTimes[0]=clock();
					bpPlayer2->newBlock();
					stretchfft2->applyWindow();
					stretchfft2->fftshift();
					stretchfft2->fft();
					tmpTimes[1]=clock();times[1]+=clock()-tmpTimes[0];
					stretchfft2->getDelta(bpPlayer2->actualInputHopSize);//targetHopsize
					tmpTimes[2]=clock();times[2]+=clock()-tmpTimes[1];
					stretchfft2->incrementPsi(bpPlayer2->getActualTimeScaleFactor());//stretchRatio
					tmpTimes[3]=clock();times[3]+=clock()-tmpTimes[2];
					stretchfft2->polarToCart(stretchfft2->magBuffer,stretchfft2->psi);
					tmpTimes[4]=clock();times[4]+=clock()-tmpTimes[3];
					stretchfft2->ifft();
					tmpTimes[5]=clock();times[5]+=clock()-tmpTimes[4];
					stretchfft2->fftshift();
					stretchfft2->applyWindow();
					bpPlayer2->writeBlock();
					tmpTimes[6]=clock();times[6]+=clock()-tmpTimes[5];

				}
				//__android_log_print(ANDROID_LOG_DEBUG,TAG1, "hops: %d : %d",bpPlayer2->hopCounter, blockSize);
				bpPlayer2->prepareForNextBlock();
			}
			otimes[4] = clock()-inc;inc=clock();
			if (bpPlayer2->checkOutputAvailable(blockSize)) {
				bpPlayer2->getOutputBlockAdd(outputtarget, blockSize, vol2);
			}

			/*
			int readCount = readMp3Data(decimator->block, decimator->size, handles[mp3Handle2]);
			decimator->filterAndDecimate(false);
			bpPlayer2->setBlock(decimator->decimated);
			while (bpPlayer2->hasMoreBlocks()) {
				bpPlayer2->newBlock();
				stretchfft2->applyWindow();
				bpPlayer2->writeBlock();
			}
			bpPlayer2->prepareForNextBlock();

			if (bpPlayer2->checkOutputAvailable(blockSize)) {
				bpPlayer2->getOutputBlockAdd(outputtarget, blockSize, vol2);//
			}
			jlong pos2 = env->GetLongField(sdb2,sdb_fields.position);
			pos2+=readCount;
			env->SetLongField(sdb2,sdb_fields.position,pos2);
			*/
		}
		otimes[5] = clock()-inc;inc=clock();
		now = clock();
		mseconds_elapsed = (now - start) / (CLOCKS_PER_SEC/1000.0);
		for (int i=0;i<6;i++) {otimes[i]/= (CLOCKS_PER_SEC/1000.0);}
		__android_log_print(ANDROID_LOG_DEBUG, TAG1, "otime:: 1: %d 2:%d 3:%d 4:%d 5:%d 6:%d sum:%d",otimes[0],otimes[1],otimes[2],otimes[3],otimes[4],otimes[5],otimes[0]+otimes[1]+otimes[2]+otimes[3]+otimes[4]+otimes[5]);
		for (int i=0;i<7;i++) {times[i]/= (CLOCKS_PER_SEC/1000.0);}
		__android_log_print(ANDROID_LOG_DEBUG, TAG1, "time:: %d,dec: %d, fft:%d, delta:%d, incpsi: %d, ptoc:%d, ifft:%d, wb:%d sum:%d",mseconds_elapsed,times[0],times[1],times[2],times[3],times[4],times[5],times[6],times[0]+times[1]+times[2]+times[3]+times[4]+times[5]+times[6]);

		// do stuff
		return blockSize;
	}

	JNIEXPORT jint JNICALL Java_net_robmunro_automix_AutoMixActivity_test(JNIEnv *env, jobject obj, jobject buffer,jobject buffer1) {
		short* input = new short[8192];
		short* outputtarget = (short*)env->GetDirectBufferAddress(buffer);
		short* inputtarget = (short*)env->GetDirectBufferAddress(buffer);
		for (int i=0;i<8192;i++) {input[i]=i;}
		bpPlayer2->setBlock(input);
		bpPlayer2->inputBuffer->read(inputtarget,8192,false,1);
		while (bpPlayer2->hasMoreBlocks()) {
			bpPlayer2->newBlock();
			//stretchfft2->applyWindow();
			bpPlayer2->writeBlock();
		}
		bpPlayer2->prepareForNextBlock();
		bpPlayer2->getOutputBlockAdd(outputtarget, bpPlayer2->outputBuffer->usedSpace(), 1);
		return bpPlayer2->outputBuffer->usedSpace();
	}
}
