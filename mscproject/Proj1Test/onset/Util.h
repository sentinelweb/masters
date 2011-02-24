/*
 * Util.h
 *
 *  Created on: 20-Jul-2010
 *      Author: robm
 */

#ifndef UTIL_H_
#define UTIL_H_
#include <cstdlib>
#include <stdio.h>
using namespace std;

class Util {
public:
	static void arrop(float* buf,int size,string tag);
	static void arrop(short* buf,int size,string tag);

	Util();
	virtual ~Util();
};

#endif /* UTIL_H_ */
