/*
 * SongData.cpp
 *
 *  Created on: 23-Jul-2010
 *      Author: robm
 */

#include "SongData.h"
#include "Stats.h"
#include "stdlib.h"

//#include <iostream>

using namespace std;

SongData::SongData( BlockProcessor* _bp,float _Fs, int _decimation, int _onsetArrSize,int _ioiHistoSize) {
	// TODO Auto-generated constructor stub

	this->Fs=_Fs;
	this->decimation=_decimation;
	this->filePointer=0;
	this->start=0;
	this->end=0;
	this->length=0;
	this->read=0;
	this->onsetArrSize= _onsetArrSize;
	this->ioiHistoSize= _ioiHistoSize;

	//this->currentTempoHops=0;;
	//this->tempo=0;
	this->onsetInitialised=false;

	this->selOFIdx=-1;
	this->avg = new VariableArray(100);
	this->peak = new VariableArray(100);
	//this->tempo = new VariableArray(100);
	this->onsetssf = new VariableArray(100);
	this->beatSnapMargin=6;
	//this->onsetssfHops = new VariableArray(100);
	//this->beatMarks = new VariableArray(100);
	//this->beatTypes = new VariableArray(100);
	//this->onsetssfHopsCorr = new VariableArray(100);

	//this->onsetscd = new VariableArray(100);
	//this->onsetswdp = new VariableArray(100);

	//this->fft=_fft;
	this->bpAnalysis=_bp;
	this->hopSize=bpAnalysis->outputHopSize;
	absBuffer=new float[bpAnalysis->inputHopSize];

	this->onset = new Onset(this->bpAnalysis->fft);


	this->onsetIntervalMinimum=0;
	//this->currentTempo=0;
	this->hopCount=0;
}
void SongData::setFilterThresholds(float *_threshs, int _size){

	tearDownOnsets();
	this->onsetFilterThresholds=new VariableArray(10);
	this->onsetssfHops = new VariableArray*[_size];
	//this->beatMarks = new VariableArray*[_size];
	//this->beatTypes = new VariableArray*[_size];
	this->beatData = new BeatData**[_size];
	this->beatDataSizes=new int[_size];
	this->onsetssfHopsCorr = new VariableArray*[_size];
	this->sfArr = new OnsetArray*[_size] ;
	this->ioih = new IOIHistogram*[_size] ;
	this->tempo = new TempoData*[_size];
	for (int i=0;i<_size;i++) {
		this->onsetFilterThresholds->add(_threshs[i]);
		this->onsetssfHops[i]=new VariableArray(100);
		//this->beatMarks[i]=new VariableArray(100);
		//this->beatTypes[i]=new VariableArray(100);
		this->beatData[i] =  (BeatData**)-1;
		this->onsetssfHopsCorr[i]=new VariableArray(100);
		this->sfArr[i] = new OnsetArray(onsetArrSize) ;
		this->ioih[i] = (IOIHistogram*)-1;//new IOIHistogram(ioiArrSize);//
		this->tempo [i] = new TempoData();
		//this->ioih[i] = new IOIHistogram(_ioiArrSize);
	}
	this->onsetInitialised=true;
}
void SongData::tearDownOnsets(){
	if (this->onsetInitialised) {
		for (int i=0;i<this->onsetFilterThresholds->getSize();i++) {
			delete onsetssfHops[i];
			//delete beatMarks[i];
			//delete beatTypes[i];
			delete onsetssfHopsCorr[i];
			if ((int)this->ioih[i]!=-1) {
				delete ioih[i];// maybe not exist#
			}
			if ((int)this->beatData[i]!=-1) {
				for (int j=0;j<beatDataSizes[i];j++) {
					delete beatData[i][j];
				}
				delete [] beatData[i];
			}
		}
		delete [] onsetssfHops;
		//delete [] beatMarks;
		//delete [] beatTypes;
		delete [] beatData;
		delete [] beatDataSizes;
		delete [] onsetssfHopsCorr;
		delete [] sfArr;
		delete [] ioih;
		delete [] tempo;

		this->onsetInitialised=false;
	}
}
float SongData::getTempoStd(float tempo,int max,int min) {
	if (tempo<=0) {return 0;}
	while (tempo>max) {
		tempo/=2;
	}
	while (tempo<min) {
		tempo*=2;
	}
	return tempo;
}
void SongData::analyseBlock() {
	//just grab the last hops worth as they are the new samples.
	Stats::abs(bpAnalysis->fft->processBuffer,absBuffer, bpAnalysis->inputHopSize,bpAnalysis->fft->fftSize- bpAnalysis->inputHopSize);//
	short max = Stats::max(absBuffer, bpAnalysis->inputHopSize);
	peak->add(max);
	float mean = Stats::mean(absBuffer, bpAnalysis->inputHopSize);
	//cout<< "mean:"<< mean << endl;
	avg->add(mean);

	bpAnalysis->fft->applyWindow();
	bpAnalysis->fft->fft();
	//cout << ":" << bpAnalysis->getTime(Fs) << " ms : "  << bpAnalysis->getSamps() << ":";
	int sfVal = onset->spectralFlux();
	//cout << "spectral flux:"  << sfVal <<":"<< endl;;
	if (this->hopCount>1) {// allow this and last frame init.
		onsetssf->add(sfVal);
		//Util::arrop(sfArr1->values, sfArr1->numValues,"sf");
		for (int i=0;i<onsetFilterThresholds->getSize();i++) {
			//sfArr[i]->doFilter();
			sfArr[i]->addElement(sfVal);
			if(sfArr[i]->checkForOnset(onsetFilterThresholds->get(i)) && sfArr[i]->markOnset(this->hopCount,onsetIntervalMinimum) ){
				onsetssfHops[i]->add(hopCount-1);// this is where the peak starts to decrease, so add the last point
				//cout << ":" << bpAnalysis->getTime(Fs/decimation) << " ms : hopcnt:" << this->hopCount ;
				//cout << ":X (" <<" i:" << i <<":"<< sfArr[i]->lastOnsetInterval<<" )" ;
				//cout << endl;
			}
		}
	} else {
		onsetssf->add(0);
	}
	this->hopCount++;
}
void SongData::estimateTempo(int index) {
	IOIHistogram* ii=new IOIHistogram(onsetssfHops[index]->getSize());
	this->ioih[index]=ii;;
	int sz = onsetssfHops[index]->getSize();
	for (int i=1;i<sz;i++) {
		ioih[index]->addElement(onsetssfHops[index]->get(i)-onsetssfHops[index]->get(i-1));
	}
	ioih[index]->histoSize=this->ioiHistoSize;
	tempo[index]->tempoHops=ioih[index]->estimateTempo();

	tempo[index]->tempoBPM = this->getTempoStd(bpAnalysis->getBPM( Fs/decimation, tempo[index]->tempoHops ),160,80);
	tempo[index]->tempoHops= bpAnalysis->bpmToHops(Fs/decimation,tempo[index]->tempoBPM);
}

int beatCmp( const void* a, const void* b ) {  // compare function
	BeatData *ba= *((BeatData**)a);
	BeatData *bb= *((BeatData**)b);
	if( ba->pos < bb->pos ) return -1;
	else if( ba->pos == bb->pos ) return 0;
	else return 1;

	//float diff=bb->pos-ba->pos;
	//return (int) diff+0.5;
}


float SongData::getClosestTempo(float _tempo1, float _tempo2){
	float diff = Stats::abs(_tempo1-_tempo2);
	if (Stats::abs(_tempo1-_tempo2*2)<diff) {
		return _tempo2*2;
	} else if (Stats::abs(_tempo1-_tempo2/2)<diff) {
		return _tempo2/2;
	}
	return _tempo2;
}

void SongData::correlateTempo(int index) {
	// loose corellation snap beats to decided tempo
	VariableArray*  _onsetssfHops=onsetssfHops[index];
	int maxCorr = 0;
	int beatsSize = (int)(onsetssf->getSize()/this->tempo[index]->tempoHops+1);

	this-> maxBeatTimes= new int[beatsSize];
	this->maxBeatTypes= new int[beatsSize];
	this->maxBeatIntervals= new int[beatsSize];
	int maxBeatSize=0;
	this->tmp_beatTimes = new int[beatsSize];
	this->tmp_beatTypes = new int[beatsSize];
	this->tmp_beatIntervals = new int[beatsSize];

	int sz = _onsetssfHops->getSize();
	//cout << "sz:" << sz <<endl;
	for (int onsetIndex=0;onsetIndex<sz;onsetIndex++) {
		//cout << "onsetIndex:" << onsetIndex << endl;
		int searchPos=_onsetssfHops->get(onsetIndex);
		for (int i=0;i<beatsSize;i++) {
			this->tmp_beatTimes[i]=0;
		}
		//cout << "1:" << onsetIndex << endl;
		int beatCtr=0;
		tmp_beatTimes[beatCtr] = searchPos;
		//cout << "2:" << onsetIndex << endl;
		tmp_beatTypes[beatCtr] = 2;
		tmp_beatIntervals[beatCtr] = tempo[index]->tempoHops;
		//cout << "3:" << onsetIndex << endl;
		beatCtr++;
		//cout << "onsetIndex:"<<onsetIndex;

		int tmpSearchPos = searchPos;//search position in hops
		int lastTmpSearchPos = searchPos;
		int corrValue = 0;// the total
		int searchPointer=onsetIndex;
		tmpSearchPos-=tempo[index]->tempoHops;
		//float margin=6;
		//cout << " searchPos:"<<searchPos ;
		int found = 0;
		while (tmpSearchPos>0) {
			int beatType = 0;
			while( searchPointer>1 && tmpSearchPos<_onsetssfHops->get(searchPointer-1)  ) {
				searchPointer--;
			}
			float diff=tmpSearchPos - _onsetssfHops->get(searchPointer-1);
			float diff1=tmpSearchPos - _onsetssfHops->get(searchPointer);
			//cout << " diff:"<<diff<<":"<<diff1 ;
			if (Stats::abs(diff)<beatSnapMargin) {
				corrValue++;
				tmpSearchPos=_onsetssfHops->get(searchPointer-1);
				beatType = 1;
				found++;
			} else if (Stats::abs(diff1)<beatSnapMargin) {
				corrValue++;
				tmpSearchPos=_onsetssfHops->get(searchPointer);
				beatType = 1;
				found++;
			}
			//cout << "("<<beatType<<":"<< searchPointer <<")";
			tmp_beatTimes[beatCtr]=tmpSearchPos;
			tmp_beatTypes[beatCtr]=beatType;

			if (beatCtr>1) {
				tmp_beatIntervals[beatCtr] = Stats::abs(tmpSearchPos-lastTmpSearchPos);
			} else {
				tmp_beatIntervals[beatCtr] = tempo[index]->tempoHops;
			}
			beatCtr++;
			lastTmpSearchPos=tmpSearchPos;
			tmpSearchPos-=tempo[index]->tempoHops;


			//cout << " corrValue:"<<corrValue << " tmpSearchPos:"<<tmpSearchPos;
		}
		//cout <<" fn:"<<found << " ::finnNeg::: -----------------------"<<endl;
		tmpSearchPos=searchPos;
		lastTmpSearchPos=searchPos;
		searchPointer=onsetIndex+1;
		found=0;
		tmpSearchPos+=tempo[index]->tempoHops;
		int maxVal = _onsetssfHops->get(_onsetssfHops->getSize()-1);
		while (tmpSearchPos < maxVal ) {
			int beatType = 0;
			while(tmpSearchPos>_onsetssfHops->get(searchPointer) && searchPointer<_onsetssfHops->getSize()-1) {
				searchPointer++;
			}
			float diff=Stats::abs(tmpSearchPos - _onsetssfHops->get(searchPointer+1));
			float diff1=tmpSearchPos - _onsetssfHops->get(searchPointer);
			//cout << " diff:"<<diff<<":"<<diff1 ;
			if (Stats::abs(diff)<beatSnapMargin ) {
				corrValue++;
				tmpSearchPos=_onsetssfHops->get(searchPointer+1);
				beatType = 1;
				found++;
			} else if (Stats::abs(diff1)<beatSnapMargin ) {
				corrValue++;
				tmpSearchPos=_onsetssfHops->get(searchPointer);
				beatType = 1;
				found++;
			}
			//cout << "("<<beatType<<":"<< searchPointer <<")";
			tmp_beatTimes[beatCtr]=tmpSearchPos;
			tmp_beatTypes[beatCtr]=beatType;
			if (beatCtr>1) {
				tmp_beatIntervals[beatCtr] = Stats::abs(tmpSearchPos-lastTmpSearchPos);
			} else {
				tmp_beatIntervals[beatCtr] = tempo[index]->tempoHops;
			}
			beatCtr++;
			lastTmpSearchPos=tmpSearchPos;
			tmpSearchPos+=tempo[index]->tempoHops;
			//cout << " corrValue:"<<corrValue << " tmpSearchPos:"<<tmpSearchPos;
		}
		//cout <<" ff:"<<found<< " corr:"<<corrValue<<"::finnPos::: "<< endl;
		onsetssfHopsCorr[index]->add(corrValue);
		//cout << "1.loop end: "<<corrValue<<endl;
		if (corrValue>maxCorr) {
			maxCorr=corrValue;
			maxBeatSize=beatCtr;
			for (int i=0;i<beatsSize;i++) {
				maxBeatTimes[i]=tmp_beatTimes[i];
				maxBeatTypes[i]=tmp_beatTypes[i];
				maxBeatIntervals[i]=tmp_beatIntervals[i];
			}
		}
		//cout << "2.loop end: "<<maxCorr<<endl;
	}

	tempo[index]->correlation=maxCorr;
	tempo[index]->correlationPC = tempo[index]->correlation/maxBeatSize;
	this->beatDataSizes[index]=maxBeatSize;
	this->beatData[index] = new BeatData*[maxBeatSize];

	for (int i=0;i< maxBeatSize;i++) {
		BeatData *b = new BeatData();
		b->pos=maxBeatTimes[i];
		b->type=maxBeatTypes[i];
		b->interval=maxBeatIntervals[i];
		this->beatData[index][i]=b;
		//cout << " B:"<< maxBeatTimes[i];
		//this->beatMarks[index]->add(maxBeatTimes[i]);
		//this->beatTypes[index]->add(maxBeatTypes[i]);
	}
	//cout << endl;
	qsort(this->beatData[index],maxBeatSize, sizeof(BeatData*), beatCmp);
	//cout << "sorted:" <<endl;
	float mean = 0;
	int ctr=0;
	//cout << "1.fin: "<<maxCorr<<endl;
	for (int i=0;i<maxBeatSize;i++) {
		if (this->beatData[index][i]->interval>0 && this->beatData[index][i]->interval<1000) {
			mean+=this->beatData[index][i]->interval;
			//cout<<"beat:"<<this->beatData[index][i]->pos <<":"<<this->beatData[index][i]->type<<endl;
			ctr++;
		}
	}
	//cout << "2.fin: "<<mean<<endl;
	mean=mean/(float)ctr;
	tempo[index]->avgTempoHops=mean;
	tempo[index]->avgTempoBPM=bpAnalysis->getBPM(Fs/decimation,tempo[index]->avgTempoHops);

	//cout << "bt phase: maxCorr:"<< maxCorr << " sz:"<< maxBeatSize << " searchPos: " << maxBeatTimes[0] << " btmpoHops:"<<mean <<endl;

	delete [] this->maxBeatTimes;
	delete [] this->maxBeatTypes;
	delete [] this->maxBeatIntervals;

	//cout << "0.fin: "<<endl;
	delete [] this->tmp_beatTimes;
	delete [] this->tmp_beatTypes;
	delete [] this->tmp_beatIntervals;
	//cout << "01.fin: "<<endl;
}


SongData::~SongData() {
	// TODO Auto-generated destructor stub
	delete this->avg;
	delete this->peak;
	delete this->onsetssf ;
	tearDownOnsets();
	delete this->onset;
	delete this->onsetssfHops;
	delete this->absBuffer;
}
TempoData::TempoData(){tempoHops=0;tempoBPM=0;avgTempoHops=0;avgTempoBPM=0;correlation=0;correlationPC=0;}
TempoData::~TempoData(){};
BeatData::BeatData(){pos=0;type=0;interval=0;}
BeatData::~BeatData(){};
