/*
 * SongData.h
 *
 *  Created on: 23-Jul-2010
 *      Author: robm
 */

#ifndef SONGDATA_H_
#define SONGDATA_H_
#include "Onset.h"
#include "OnsetArray.h"
#include "IOIHistogram.h"
#include "VariableArray.h"
#include "BlockProcessor.h"

class TempoData{
	public:
		int tempoHops;
		float tempoBPM;
		float avgTempoHops;
		float avgTempoBPM;
		float correlation;
		float correlationPC;
		float score;
		int corrScore;
		TempoData();
		virtual ~TempoData();

} ;
class BeatData{
	public:
		int pos;
		int type;
		int interval;
		BeatData();
		virtual ~BeatData();

} ;

class SongData {
public:
	float Fs;
	int decimation;
	float hopSize;
	char* fileName;
	void *filePointer;
	long start;//=_samples
	long end;//=_samples
	long length;//=_samples
	int hopCount;
	int read;
	int onsetArrSize;
	int ioiHistoSize;
	bool onsetInitialised;
	int selOFIdx;// Selected Onsdets Filter index
	// onset variables
	VariableArray*  onsetFilterThresholds;// // how many time over the filtered value do we need to be for onset.
	float onsetIntervalMinimum;//in hops
	int beatSnapMargin;

	float* absBuffer;

	VariableArray* peak ;
	VariableArray* avg ;
	VariableArray* onsetssf ;

	VariableArray** onsetssfHops ;
	VariableArray** onsetssfHopsCorr;
	//VariableArray** beatMarks;
	//VariableArray** beatTypes;
	OnsetArray** sfArr ;
	IOIHistogram** ioih ;

	TempoData** tempo;
	BeatData*** beatData;
	int* beatDataSizes;
	BlockProcessor* bpAnalysis;

	Onset* onset ;

	int* maxBeatTimes;
	int* maxBeatTypes;
	int* maxBeatIntervals;


	int* tmp_beatTimes;
	int* tmp_beatTypes;
	int* tmp_beatIntervals;

	SongData( BlockProcessor* _fft,float _Fs, int _decimation, int _onsetArrSize,int _ioiArrSize);
	virtual ~SongData();
	float getTempoStd(float tempo,int max,int min);
	static float getClosestTempo(float _tempo1,float _tempo2);
	void setFilterThresholds(float* _threshs,int _size);
	void tearDownOnsets();
	void analyseBlock();
	void estimateTempo(int index) ;
	float estimateTempoPartial(int _index,int _start,int _end,int _sumPrevious,int _histSize);
	void correlateTempo(int index);
};

/*
class BeatData {
public:
	int _time;
	int _type;
	BeatData( int _time,int _type);
	virtual ~BeatData();
}
*/
#endif /* SONGDATA_H_ */
