/*
 * Ex2.cpp
 *
 *  Created on: 10-Oct-2009
 *      Author: robm
 */
using namespace std;
#include "Ex2.h"
#include <iostream>
Ex2::Ex2() {
	// TODO Auto-generated constructor stub

}

Ex2::~Ex2() {
	// TODO Auto-generated destructor stub
}

int main() {
	int myInt = 1000;
	int *myIntPointer = &myInt;// declare a pointer to an integer,and make it point to myInt
	*myIntPointer+=5;
	cout << myInt << " " << myIntPointer <<"\n";

	int myInt2 = 1000;
	int *myIntPointer2 = &myInt2;
	// declare another integer whose value is the same as the integer
	// at memory address <myIntPointer>
	int mySecondInt2 = *myIntPointer2;
	cout <<  myInt2 <<"  " <<"\n";
	// dereference the pointer and add 5 to the integer it points to
	*myIntPointer2 += 5;
	cout << myInt2 << " " <<"\n";
	cout << mySecondInt2 <<"\n";

}
