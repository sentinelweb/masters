/*
 * Histogram.cpp
 *
 *  Created on: 30-Nov-2009
 *      Author: robm
 */

#include "Histogram.h"
#include "stdio.h"
#include <vector>
#include <cstdlib>
#include <math.h>
#include <iostream>

using namespace std;

Histogram::Histogram() {
	this->histo = new vector<float>(256);// (int*)malloc(sizeof(int)*255*3)
	this->histor = new vector<float>(256);
	this->histog = new vector<float>(256);
	this->histob = new vector<float>(256);
}

Histogram::~Histogram() {
	delete this->histo;// hello
	delete this->histor;
	delete this->histog;
	delete this->histob;
}

void Histogram::makeHistogram(unsigned char* imgBuf) {
	cerr << "start histogram" <<"\n";
	for (int i=0;i<256;i++) {
		this->histo->at(i)=0;
		this->histor->at(i)=0;
		this->histog->at(i)=0;
		this->histob->at(i)=0;
	}
	cerr << "in histogram" <<"\n";
	for(int y = 0;y < this->imgHeight;y++){
		for(int x = 0;x < this->imgWidth;x++){
			//calculate luminosity for pixel
			//cout << "here2" <<"\n";
			int idx=(y * this->imgWidth + x) * 3;
			unsigned char r = imgBuf[idx];
			unsigned char g = imgBuf[idx+1];
			unsigned char b = imgBuf[idx+2];
			float yl = 0.299*r + 0.587*g + 0.114*b;
			//cerr << (int)r << ":" << (int)g << ":"<< (int)b << ":"<< round(yl) << ":" <<"\n";
			this->histo->at((int)yl)++;
			// colour histogram
			this->histor->at(r)++;
			this->histog->at(g)++;
			this->histob->at(b)++;
			//cout << "here2" <<"\n";
		}
	}
	cerr << "in histogram 1" <<"\n";
	cerr << "made histogram" <<"\n";
}

unsigned char* Histogram::display() {
	cerr << "start display" <<"\n";
	normailse(this->histo,255,-1);

	unsigned char* histImgBuf = (unsigned char*)(malloc(256 * 256 * 3));
	// paint it black
	for (int i=0;i<256;i++) {
		for (int j=0;j<256;j++) {
			histImgBuf[((j) * 255 + i) * 3] = 0;
			histImgBuf[((j) * 255 + i) * 3+1] = 0;
			histImgBuf[((j) * 255 + i) * 3+2] = 0;
		}
	}
	// draw histo
	for (int i =0;i<this->histo->size();i++) {
		float value = this->histo->at(i);
		for (int j=0;j<value;j++) {
			//cout << "255-j:" << 255-j <<"\n";
			histImgBuf[((255-j) * 255 + i) * 3] = 255;
			histImgBuf[((255-j) * 255 + i) * 3+1] = 255;
			histImgBuf[((255-j) * 255 + i) * 3+2] = 255;
		}
	}
	cerr << "made display" <<"\n";

	return histImgBuf;
}
unsigned char* Histogram::displayColour() {
	cerr << "start displayc" <<"\n";
	//get the max value in the seperate colour channels
	int max = getMax(this->histor);
	int tmp = getMax(this->histog);
	if (tmp>max) {max=tmp;}
	tmp = getMax(this->histob);
	if (tmp>max) {max=tmp;}
	normailse(this->histor,255,max);
	normailse(this->histog,255,max);
	normailse(this->histob,255,max);
	unsigned char* histImgBuf = (unsigned char*)(malloc(256 * 256 * 3));
	// paint it black
	for (int i=0;i<255;i++) {
		for (int j=0;j<255;j++) {
			histImgBuf[((j) * 255 + i) * 3] = 0;
			histImgBuf[((j) * 255 + i) * 3+1] = 0;
			histImgBuf[((j) * 255 + i) * 3+2] = 0;
		}
	}
	cerr << "making r" <<"\n";
	// draw histo
	for (int i =0;i<this->histor->size();i++) {
		float value = this->histor->at(i);
		for (int j=0;j<value;j++) {
			histImgBuf[((255-j) * 255 + i) * 3] = 255;
		}
	}
	cerr << "making g" <<"\n";
	for (int i =0;i<this->histog->size();i++) {
		float value = this->histog->at(i);
		for (int j=0;j<value;j++) {
			histImgBuf[((255-j) * 255 + i) * 3+1] = 255;
		}
	}
	cerr << "making b" <<"\n";
	for (int i =0;i<this->histob->size();i++) {
		float value = this->histob->at(i);
		for (int j=0;j<value;j++) {
			histImgBuf[((255-j) * 255 + i) * 3+2] = 255;
		}
	}
	cerr << "made displayc" <<"\n";
	return histImgBuf;
}

unsigned char* Histogram::equalise(unsigned char* imgBuf){
	makeHistogram(imgBuf);
	// divide by number of pixels
	int pixNum = this->imgHeight*this->imgWidth;
	for (int i=0;i<this->histo->size();i++) {
		this->histo->at(i)/=(float)pixNum;
	}
	// produce cumulative distribution
	vector<float> *cumulativeDist = new vector<float>(256);
	float cumulativeSum = 0;
	for (int i=0;i<this->histo->size();i++) {
		cumulativeSum+=this->histo->at(i);
		cumulativeDist->at(i)=cumulativeSum;
	}
	// multiply by grey level (255)
	for (int i=0;i<cumulativeDist->size();i++) {
		cumulativeDist->at(i)*=255;
		//cerr<<i<<":"<<(int)cumulativeDist->at(i)<<",";
	}
	// use this lookup table for the image.
	unsigned char* equalisedImageBuf =  (unsigned char*)(malloc(this->imgWidth * this->imgHeight * 3));
	for(int y = 0;y < this->imgHeight;y++){
		for(int x = 0;x < this->imgWidth;x++){
			//calculate luminosity for pixel
			int idx = (y * this->imgWidth + x) * 3;
			unsigned char r = imgBuf[ idx];
			unsigned char g = imgBuf[idx+1];
			unsigned char b = imgBuf[idx+2];
			float yl = 0.299 * r + 0.587*g + 0.114*b;
			int pixVal = (int) cumulativeDist->at( (int)yl );
			equalisedImageBuf[idx]=pixVal;
			equalisedImageBuf[idx+1]=pixVal;
			equalisedImageBuf[idx+2]=pixVal;
		}
	}
	return equalisedImageBuf;
}
unsigned char* Histogram::equaliseColour(unsigned char* imgBuf){
	unsigned char* yuvBuf = this->toYUV(imgBuf);
	makeHistogram(yuvBuf);
	// divide by number of pixels
	int pixNum = this->imgHeight*this->imgWidth;
	for (int i=0;i<this->histor->size();i++) {
		this->histor->at(i)/=(float)pixNum;
	}
	// produce cumulative distribution
	vector<float> *cumulativeDist = new vector<float>(256);
	float cumulativeSum = 0;
	for (int i=0;i<this->histor->size();i++) {
		cumulativeSum+=this->histor->at(i);
		cumulativeDist->at(i)=cumulativeSum;
	}
	// multiply by grey level (255)
	for (int i=0;i<cumulativeDist->size();i++) {
		cumulativeDist->at(i)*=255;
		//cerr<<i<<":"<<(int)cumulativeDist->at(i)<<",";
	}
	// use this lookup table for the image.
	unsigned char* equalisedImageBufYUV =  (unsigned char*)(malloc(this->imgWidth * this->imgHeight * 3));
	for(int y = 0;y < this->imgHeight;y++){
		for(int x = 0;x < this->imgWidth;x++){
			//calculate luminosity for pixel
			int idx = (y * this->imgWidth + x) * 3;
			unsigned char yl = yuvBuf[ idx];
			char u = (char)yuvBuf[idx+1];
			char v = (char)yuvBuf[idx+2];
			int pixVal = (int) cumulativeDist->at( (int)yl );
			equalisedImageBufYUV[idx]=pixVal;
			equalisedImageBufYUV[idx+1]=u;
			equalisedImageBufYUV[idx+2]=v;
		}
	}
	unsigned char* equalisedImageBuf = this->toRGB(equalisedImageBufYUV);
	return equalisedImageBuf;
}

int Histogram::getMax(vector<float> *histBuf){
	float maxValue=0;
	for (int i=0;i<histBuf->size();i++) {
		if (maxValue<histBuf->at(i)) {
			maxValue = histBuf->at(i);
		}
	}
	return maxValue;
}

void Histogram::normailse(vector<float> *histBuf,int normalisationValue,float maxValue){
	// normailse the results to normalisationValue
	// find max
	if (maxValue==-1) {
		maxValue=getMax(histBuf);
	}
	//normailise the rest to normalisationValue.
	for (int i=0;i<histBuf->size();i++) {
		histBuf->at(i)*=normalisationValue/maxValue;
	}
}
unsigned char* Histogram::toYUV(unsigned char* imgBuf){
	unsigned char* yuvImageBuf =  (unsigned char*)(malloc(this->imgWidth * this->imgHeight * 3));
	for(int y = 0;y < this->imgHeight;y++){
		for(int x = 0;x < this->imgWidth;x++){
			int idx =(y * this->imgWidth + x) * 3;
			unsigned char r = imgBuf[idx ];
			unsigned char g = imgBuf[idx +1];
			unsigned char b = imgBuf[idx +2];

			float Y = 0.299 * r + 0.587*g + 0.114*b;
			float U = (b-Y)*0.492;
			float V = (r-Y)*0.877;
			//float U = -0.147*r-0.289*g+0.436*b;
			//float V = 0.615*r-0.515*g-0.100*b;

			yuvImageBuf[idx ]= (unsigned char)Y;
			yuvImageBuf[idx+1 ]=(unsigned char)U;
			yuvImageBuf[idx+2 ]=(unsigned char)V;

			if (x==40 && y==40) {
				cout <<"Y:"<< (int)Y <<" U:" << (int)U <<" :V" <<(int)V <<" r:"<< (int)r <<" g:" <<(int)g <<" b:"<< (int)b <<"\n";
			}
		};
	}
	return  yuvImageBuf;
}
unsigned char* Histogram::toRGB(unsigned char* imgBuf){
	unsigned char* rgbImageBuf =  (unsigned char*)(malloc(this->imgWidth * this->imgHeight * 3));
	for(int y = 0;y < this->imgHeight;y++){
		for(int x = 0;x < this->imgWidth;x++){
			int idx =(y * this->imgWidth + x) * 3;
			unsigned char Y = imgBuf[idx ];
			char U = (char)imgBuf[idx +1];
			char V = (char)imgBuf[idx +2];

			float r = Y + 1.140*V;
			float g = Y - 0.395*U -0.581*V;
			float b = Y + 2.032*U ;

			rgbImageBuf[idx ]= (unsigned char)r;
			rgbImageBuf[idx+1 ]=(unsigned char)g;
			rgbImageBuf[idx+2 ]=(unsigned char)b;
			if (x==40 && y==40) {
				cout <<"Y:"<< (int)Y <<" U:" << (int)U <<" :V" <<(int)V <<" r:"<< (int)r <<" g:" <<(int)g <<" b:"<< (int)b <<"\n";
			}
		};
	}
	return  rgbImageBuf;
}
