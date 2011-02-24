/*
 * Histogram.h
 *
 *  Created on: 30-Nov-2009
 *      Author: robm
 */

#ifndef HISTOGRAM_H_
#define HISTOGRAM_H_
using namespace std;
#include <vector>
class Histogram {
public:
	vector<float> *histo;
	vector<float> *histor;
	vector<float> *histog;
	vector<float> *histob;
	int imgWidth;
	int imgHeight;


    Histogram();
	virtual ~Histogram();

	void makeHistogram(unsigned char* imgBuf);
	unsigned char* equalise(unsigned char* imgBuf);
	unsigned char* equaliseColour(unsigned char* imgBuf);
	int getMax(vector<float> *histBuf);
	void normailse(vector<float> *histo,int normalisationValue,float maxValue);
	unsigned char* display();
	unsigned char* displayColour();
	unsigned char* toYUV(unsigned char* imgBuf);
	unsigned char* toRGB(unsigned char* imgBuf);

	int getImgHeight() const   {return imgHeight;}
	int getImgWidth() const    {return imgWidth;}
	void setImgHeight(int imgHeight)   {this->imgHeight = imgHeight;}
	void setImgWidth(int imgWidth)   {this->imgWidth = imgWidth;}
};

#endif /* HISTOGRAM_H_ */
