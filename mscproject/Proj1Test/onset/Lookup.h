/*
 * Lookup.h
 *
 * Created on: 06-Jul-2010
 *      Author: robm
 *
 * NOT USED : Slower than normal sin(), cos(), etc - as they are optimised
 */

#ifndef LOOKUP_H_
#define LOOKUP_H_
#define 	LOOKUP_SIZE	16384
#define     PI          3.14159265358979
#define     TWOPI         6.283185307
#define     FOURPI        12.566370614
class Lookup {

public:
	static float* sinLookup;
	static float* cosLookup;
	static float* atanLookup;
	static float* tanLookup;
	static float sinl(float angle);
	static float cosl(float angle);
	static float tanl(float angle);
	static float atanl(float angle);
	static void init();
	Lookup();
	virtual ~Lookup();
};

#endif /* LOOKUP_H_ */
