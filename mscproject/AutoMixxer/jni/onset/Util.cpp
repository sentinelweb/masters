/*
 * Util.cpp
 *
 *  Created on: 20-Jul-2010
 *      Author: robm
 */
#include <iostream>
#include "Util.h"
using namespace std;
void Util::arrop(float* buf,int size,string tag) {
	cout << tag<< ":::\t";
	for (int k=0; k<size; k++) {
		cout << buf[k] <<":\t";
	}
	cout << endl;
}
void Util::arrop(short* buf,int size,string tag) {
	cout << tag<< ":::\t";
	for (int k=0; k<size; k++) {
		cout << buf[k] <<":\t";
	}
	cout << endl;
}
Util::Util() {
	// TODO Auto-generated constructor stub

}

Util::~Util() {
	// TODO Auto-generated destructor stub
}
