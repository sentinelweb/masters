/*
 * Plot.cpp
 *
 *  Created on: 07-Aug-2010
 *      Author: robm
 */

#include "Plot.h"
#include "gd.h"
#include "gdfontt.h"
#include "gdfonts.h"
#include "gdfontmb.h"
#include "gdfontl.h"
#include "gdfontg.h"
#include "onset/Stats.h"
#include <iostream>
#include <cstring>

using namespace std;
Plot::Plot() {
	// TODO Auto-generated constructor stub

}
void Plot::plot(SongData *_s, char *_fileName){
	plot( _s,  _fileName,(float*)-1,0);
}
void Plot::plot(SongData *_s, char *_fileName,float* onsetData, int onsetDataSize){

	FILE *pngout;
	int w=30024,h=768;
	int black,white,red,blue,green,orange,cyan,mag,brown;
	for (int f=0;f<_s->onsetFilterThresholds->getSize();f++) {
		w=120000;h=300;
		//w=10000;h=300;
		char num[5];
		sprintf(num , "%d",f);
		char imgFilename[50];
		strcpy(imgFilename,_fileName);
		strcat(imgFilename,"_");
		strcat(imgFilename,num);
		strcat(imgFilename,".png");
		cout<<"file:"<<imgFilename<<endl;
		gdImagePtr im = gdImageCreate(w, h);
		black = gdImageColorAllocate(im, 0, 0, 0);
		white = gdImageColorAllocate(im, 255, 255, 255);
		red = gdImageColorAllocate(im, 180, 0, 0);
		orange = gdImageColorAllocate(im, 120,120, 0);
		blue = gdImageColorAllocate(im, 0, 0, 180);
		green = gdImageColorAllocate(im, 0, 180, 0);
		cyan = gdImageColorAllocate(im, 0, 180, 180);
		mag = gdImageColorAllocate(im, 180, 0, 180);
		brown = gdImageColorAllocate(im, 139, 69, 19);

		int top = 10;
		int bottom = h-40;
		int left = 20;
		int right=w;
		int gw=right-left;//graph width
		int gh=bottom-top;//graph height


		gdImageFilledRectangle(im,0,0,w,h,white);
		// plot axes
		char* ptitle = new char[300];
		sprintf(ptitle,"Time Onset Analysis  %s",_s->fileName);
		//cout<<ptitle<<endl;
		gdImageString(im, gdFontGetLarge(), 20,top , (unsigned char*)ptitle, black);
		int offset=2000;
		//legend
		int pos=offset+500;
		gdImageRectangle(im,pos+10,top,pos+15,top+30,black);
		gdImageString(im, gdFontGetSmall(), pos+25,top , (unsigned char*)"Actual Onsets", black);

		pos=offset+390;
		gdImageLine(im,pos,top+10,pos+30,top+10,red);
		gdImageString(im, gdFontGetSmall(), pos+35,top , (unsigned char*)"Spectral Flux", black);

		pos=offset+620;
		gdImageRectangle(im,pos+10,top,pos+11,top+30,blue);
		gdImageString(im, gdFontGetSmall(), pos+35,top , (unsigned char*)"Detected Onset (SF)", black);

		pos=offset+800;
		gdImageRectangle(im,pos+10,top,pos+15,top+30,green);
		gdImageString(im, gdFontGetSmall(), pos+35,top , (unsigned char*)"Unmatched estimated beat", black);

		pos=offset+1000;
		gdImageRectangle(im,pos+10,top,pos+15,top+30,orange);
		gdImageString(im, gdFontGetSmall(), pos+35,top , (unsigned char*)"Matched estimated beat", black);
		// end legend
		gdImageLine(im, left, top, left, bottom,black);
		gdImageLine(im, left, bottom, right, bottom,black);
		for (int i=0;i<_s->onsetssf->getSize();i+=50) {
			char* num = new char[5];
			sprintf(num,"%d",i);
			int x=i/(float)_s->onsetssf->getSize()*gw;
			gdImageLine(im, left+x, bottom, left+x, bottom+3,black);
			gdImageString(im, gdFontGetTiny(), left+x-3, bottom+4, (unsigned char*)num, black);
		}

		int lastx=-1;int lasty=-1;

		float mxsf = 0;
		//plot sf
		for (int i=3;i<_s->onsetssf->getSize();i++) {
			mxsf=Stats::max(mxsf,_s->onsetssf->get(i));
		}
		//cout << "plot:mx:"<<mxsf<<endl;
		int crossSize = 3;
		for (int i=0;i<_s->onsetssf->getSize();i++) {
			int x=i/(float)_s->onsetssf->getSize()*gw;
			int y = gh-(_s->onsetssf->get(i)/mxsf)*gh;
			if (lastx!=-1) {
				gdImageLine(im, left+lastx, top+lasty, left+x, top+y, red);
				gdImageLine(im, left+x-crossSize, top+y-crossSize, left+x+crossSize, top+y+crossSize,black);
				gdImageLine(im, left+x+crossSize, top+y-crossSize, left+x-crossSize, top+y+crossSize,black);
			}
			lastx=x;
			lasty=y;
		}

		float mxc = 0;
		for (int i=3;i<_s->onsetssfHopsCorr[f]->getSize();i++) {
			mxc=Stats::max(mxc,_s->onsetssfHopsCorr[f]->get(i));
		}

		// mark onsets
		int ctr=0,ctrDir=1;
		for (int i=0;i<_s->onsetssfHops[f]->getSize();i++) {
			int x=_s->onsetssfHops[f]->get(i)/(float)_s->hopCount*gw;
			gdImageRectangle(im, left+x, top+gh, left+x+1, top+gh-gh/2,blue);
			int yc= (_s->onsetssfHopsCorr[f]->get(i)/mxc)*gh/4;//_s->onsetssfHopsCorr->get(i)/(float)_s->hopCount*w;
			gdImageLine(im, left+x, top+gh, left+x, top+gh-yc,brown);
			char* num = new char[5];
			sprintf(num,"%d",i);
			gdImageString(im, gdFontGetTiny(), left+x-3, top+gh-gh/2-10, (unsigned char*)num, black);
			// plot predicted onset from tempo
		}

		// project beat marks
		for (int i=0;i<_s->beatDataSizes[f];i++) {
			int x=_s->beatData[f][i]->pos/(float)_s->hopCount*gw;
			int t=_s->beatData[f][i]->type;
			gdImageRectangle(im,left+x-2,top+gh,left+x+2,top+gh-(t+1)*50,t==0?green:orange);

		}
		if (onsetDataSize>0) {
			// project onsetData
			float dfs=_s->Fs/_s->decimation;
			float length =_s->onsetssf->getSize()*(_s->hopSize/dfs);
			for (int i=0;i<onsetDataSize;i++) {
				int x=(onsetData[i])/(length)*gw;
				gdImageRectangle(im,left+x-2,top+gh,left+x+2,top+gh-gh/4,black);

			}
		}
		/*
		// plot pk
		float mxp = 0;
		for (int i=3;i<_s->peak->getSize();i++) {
			mxp=Stats::max(mxp,_s->peak->get(i));
		}
		lastx=-1,lasty=-1;
		for (int i=0;i<_s->peak->getSize();i++) {
			int x=i/(float)_s->peak->getSize()*gw;
			int y = gh-(_s->peak->get(i)/mxp)*gh;
			if (lastx!=-1) {
				gdImageLine(im, left+lastx, top+lasty, left+x, top+y, green);
			}
			lastx=x;
			lasty=y;
		}

		// plot avg
		//float mxa = 0;
		//for (int i=3;i<_s->avg->getSize();i++) {
		//	mxa=Stats::max(mxa,_s->avg->get(i));
		//}
		float mxa = mxp;
		lastx=-1,lasty=-1;
		for (int i=0;i<_s->avg->getSize();i++) {
			int x=i/(float)_s->avg->getSize()*gw;
			int y = gh-(_s->avg->get(i)/mxa)*gh;
			if (lastx!=-1) {
				gdImageLine(im, left+lastx, top+lasty, left+x, top+y, cyan);
			}
			lastx=x;
			lasty=y;
		}
		*/
		//gdImageLine(im, 0, 0, w, h, white);
		pngout = fopen(imgFilename, "wb");
		gdImagePng(im, pngout);
		gdImageDestroy(im);
		fclose(pngout);

		// plot ioihisto//////////////////////////////////////////////////////////////////////////////////////////////////////////
		w=600;
		right=w;
		gw=right-left;//graph width

		gdImagePtr im1 = gdImageCreate(w, h);
		black = gdImageColorAllocate(im1, 0, 0, 0);
		white = gdImageColorAllocate(im1, 255, 255, 255);
		red = gdImageColorAllocate(im1, 180, 0, 0);
		orange = gdImageColorAllocate(im1, 180,180, 0);
		blue = gdImageColorAllocate(im1, 0, 0, 180);
		green = gdImageColorAllocate(im1, 0, 180, 0);
		cyan = gdImageColorAllocate(im1, 0, 180, 180);
		mag = gdImageColorAllocate(im1, 180, 0, 180);
		gdImageFilledRectangle(im1,0,0,w,h,white);
		// make histogram & weighted..
		int histoSize=_s->ioiHistoSize;
		IOIHistogram* ii=new IOIHistogram(_s->onsetssfHops[f]->getSize());
		int sz = _s->onsetssfHops[f]->getSize();
		for (int i=1;i<sz;i++) {
			ii->addElement(_s->onsetssfHops[f]->get(i)-_s->onsetssfHops[f]->get(i-1));
		}
		int* histo = ii->genHistogram(histoSize);
		float* whisto = ii->genWeightedHisto(histoSize,histo);

		float mxt = 0;
		for (int i=5;i<histoSize;i++) {
			mxt=Stats::max(mxt,histo[i]);
		}
		float mxwt = 0;
		for (int i=5;i<histoSize;i++) {
			mxwt=Stats::max(mxwt,whisto[i]);
		}
		char* htitle = new char[300];
		sprintf(htitle,"IOI Histogram, Onset Threshold = %f, Sum Previous = %d",_s->onsetFilterThresholds->get(f),ii->sumPrevious);
		gdImageString(im1, gdFontGetLarge(), right/ 2 - (strlen(htitle) * gdFontGetLarge()->w / 2),top , (unsigned char*)htitle, black);
		gdImageString(im1, gdFontGetSmall(), right-150,top+gh/2+10 , (unsigned char*)"Time Interval (hops)", black);
		gdImageString(im1, gdFontGetSmall(), 2,2 , (unsigned char*)"Frequency", black);
		gdImageLine(im1, left, top+gh/2, right, top+gh/2,black);
		gdImageLine(im1, left, top+10, left, top+gh/2,black);

		sprintf(htitle,"Weighted IOI Histogram, Onset Threshold = %f, Sum Previous = %d",_s->onsetFilterThresholds->get(f),ii->sumPrevious);
		gdImageString(im1, gdFontGetLarge(), right/ 2 - (strlen(htitle) * gdFontGetLarge()->w / 2),bottom+20 , (unsigned char*)htitle, black);
		gdImageString(im1, gdFontGetSmall(), right-150,bottom+10 , (unsigned char*)"Time Interval (hops)", black);
		gdImageString(im1, gdFontGetSmall(), 2,gh/2+18 , (unsigned char*)"Weighted value", black);
		gdImageLine(im1, left, bottom, right, bottom,black);//hor
		gdImageLine(im1, left, top+gh/2+20, left, bottom,black);//vert

		for (int i=0;i<histoSize;i++) {
			int x=i/(float)histoSize*gw;
			int y = gh/2-(histo[i]/mxt)*gh/2;
			gdImageLine(im1, left+x, top+gh/2, left+x, top+y,blue);
			int y1 = (gh/2-(whisto[i]/mxwt)*gh/2)+gh/2;
			gdImageLine(im1, left+x, top+gh, left+x, top+y1,green);

			//plot axis marks
			if (i%10==0) {
				char* num = new char[5];
				sprintf(num,"%d",i);
				gdImageLine(im1, left+x, bottom, left+x, bottom+3,black);
				gdImageString(im1, gdFontGetTiny(), left+x-3, bottom+4, (unsigned char*)num, black);
				gdImageLine(im1, left+x, top+gh/2, left+x, top+gh/2+3,black);
				gdImageString(im1, gdFontGetTiny(), left+x-3, top+gh/2+4, (unsigned char*)num, black);
			}
		}
		char ioiFilename[50];
		strcpy(ioiFilename,_fileName);
		strcat(ioiFilename,"_");
		strcat(ioiFilename,num);
		strcat(ioiFilename,".ioi.png");
		pngout = fopen(ioiFilename, "wb");
		gdImagePng(im1, pngout);
		gdImageDestroy(im1);
		fclose(pngout);
	}
}

Plot::~Plot() {
	// TODO Auto-generated destructor stub
}
