/*
 * Lookup.cpp
 *
 * Created on: 06-Jul-2010
 *      Author: robm
 *
 * NOT USED : Slower than normal sin(), cos(), etc - as they are optimised
 */

#include "Lookup.h"
#include "math.h"

using namespace std;
// static declarations declared in header file
float* Lookup::sinLookup;
float* Lookup::cosLookup;
float* Lookup::atanLookup;
float* Lookup::tanLookup;

Lookup::Lookup() {
	// TODO Auto-generated constructor stub

}

float Lookup::sinl(float angle){// angle in radians
	int multiplier = (int) (angle/2/PI);
	float principal = (angle-TWOPI*(float)multiplier);
	//cout << ":principal: " << principal;
	/*
	int sign = 1;
	if (principal>PI) {principal-=2*PI;}
	if (principal<-PI) {principal+=2*PI;}
	if (principal<0) {
		principal=-principal;
		sign=-1;
	}
	if (principal>PI/2) {principal=PI-principal;}
	//cout << ": principal1: " << principal;
	 */
	//int index = principal/(PI/2)*(float)LOOKUP_SIZE;
	int index = (principal+TWOPI)/(FOURPI)*(float)LOOKUP_SIZE;
	//cout << ": idx:" <<index <<": mult:" <<multiplier <<" : ";

	//return sign*sinLookup[index];
	return sinLookup[index];
}

float Lookup::cosl(float angle){
	int multiplier = (int) (angle/2/PI);
	float principal = (angle-2*PI*(float)multiplier);
	//cout << ":principal: " << principal;
	int sign = 1;
	if (principal>PI) {principal-=2*PI;}
	if (principal<-PI) {principal+=2*PI;}
	if (principal<0 ) {
		principal=-principal;
	}
	if (principal>PI/2) {
		sign=-1;
		principal=PI-principal;
	}
	//cout << ": principal1: " << principal;
	int index = principal/(PI/2)*(float)LOOKUP_SIZE;
	//cout << ": idx:" <<index <<": mult:" <<multiplier <<" : ";
	return sign*cosLookup[index];
}

float Lookup::tanl(float angle){
	int multiplier = (int) (angle/2/PI);
	float principal = (angle-2*PI*(float)multiplier);
	//cout << ":principal: " << principal;
	int sign = 1;
	if (principal>PI) {principal-=2*PI;}
	if (principal<-PI) {principal+=2*PI;}
	if (principal<0 ) {
		principal=principal+PI;
	}
	if (principal>PI/2) {
		sign=-1;
		principal=PI-principal;
	}
	//cout << ": principal1: " << principal;
	int index = principal/(PI/2)*(float)LOOKUP_SIZE;
	//cout << ": idx:" <<index <<": mult:" <<multiplier <<" : ";
	return sign*tanLookup[index];
}

float Lookup::atanl(float grad){
	return 0;
}

void Lookup::init(){
	Lookup::sinLookup = new float[LOOKUP_SIZE];
	//cout << "sinTest:"<< sin(PI/2) << ":" << sin(0)<<endl;
	for (int i=0;i<LOOKUP_SIZE;i++) {
		//float ang = PI/2*((float)i)/(float)LOOKUP_SIZE;
		float ang = -2*PI+4*PI*((float)i)/(float)LOOKUP_SIZE;
		float val=sin(ang);
		//cout << val <<":\t"<< ang <<endl;
		Lookup::sinLookup[i] = val;
	}
	Lookup::cosLookup = new float[LOOKUP_SIZE];
	for (int i=0;i<LOOKUP_SIZE;i++) {
		float ang = PI/2*((float)i)/(float)LOOKUP_SIZE;
		float val=cos(ang);
		//cout << val <<":\t"<< ang <<endl;
		Lookup::cosLookup[i] = val;
	}
	Lookup::tanLookup = new float[LOOKUP_SIZE];
	for (int i=0;i<LOOKUP_SIZE;i++) {
		float ang = PI/2*((float)i)/(float)LOOKUP_SIZE;
		float val=tan(ang);
		//cout << val <<":\t"<< ang <<endl;
		Lookup::tanLookup[i] = val;
	}
	Lookup::atanLookup = new float[LOOKUP_SIZE];
	for (int i=0;i<LOOKUP_SIZE;i++) {
		float ang = PI/2*((float)i)/(float)LOOKUP_SIZE;
		float val=atan(ang);
		//cout << val <<":\t"<< ang <<endl;
		Lookup::atanLookup[i] = val;
	}
}

Lookup::~Lookup() {
	// TODO Auto-generated destructor stub
}
