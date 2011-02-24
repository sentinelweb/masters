
#ifndef AUTOMIX_H_
#define AUTOMIX_H_
#include "mad/mad.h"
#include "mad/stream.h"
#include "android/config.h"
#include <stdio.h>

#define INPUT_BUFFER_SIZE	(5*8192)
#define OUTPUT_BUFFER_SIZE	8192 /* Must be an integer multiple of 4. */

#define     PI          3.14159265358979


//songData field refs
const int SONG_PROGRESS = 1;
const int SONG_TEMPO = 2;
const int SONG_AVGTEMPO = 3;
const int SONG_SR = 4;
const int SONG_SECS = 5;
const int SONG_READ = 6;
const int SONG_PEAKVOL = 7;
const int SONG_AVGVOL = 8;
const int SONG_MATCHEDBEATS = 9;
const int SONG_NUMBEATS = 10;
const int SONG_NUMONSETS = 11;

const int PLAYSTATE_NONE = -1;
const int PLAYSTATE_STOPPED = 0;
const int PLAYSTATE_QUEUED = 1;
const int PLAYSTATE_PLAYING = 2;
const int PLAYSTATE_PLAYING_SCALED = 3;

const int NUM_MP3_HANDLES=100;

typedef struct mad_stream mad_stream;
typedef struct mad_frame mad_frame;
typedef struct mad_synth mad_synth;
/**
 * Struct holding the pointer to a MP3 file.
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

/* static MP3FileHandle array */
static MP3FileHandle* handles[NUM_MP3_HANDLES];

int readMp3Data(float* target, int size, MP3FileHandle* mp3);

#endif /* AUTOMIX_H_ */
