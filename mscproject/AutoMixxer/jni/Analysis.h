#ifndef ANALYSIS_H_
#define ANALYSIS_H_

#include "AutoMix.h"
#include "onset/SongData.h"


const int NUM_SD_HANDLES=100;
static SongData* songHandles[NUM_SD_HANDLES];

// simple interval structure
struct intvl_t{
	int start;
	int end;
};

// method declarations for external use
SongData* initSong(MP3FileHandle* mp3) ;
void analyseSong(MP3FileHandle* mp3,SongData* songData);//,intvl_t secs
int findFreeSongHandle( );
void releaseSongData(int songIndex);

#endif /* ANALYSIS_H_ */
