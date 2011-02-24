/*
 * Ex5.cpp
 *
 *  Created on: 04-Oct-2009
 *      Author: robm
 */
using namespace std;
#include "Ex5.h"
#include "Vector2D.h"
#include <iostream>

Ex5::Ex5() {
	// TODO Auto-generated constructor stub

}

Ex5::~Ex5() {
	// TODO Auto-generated destructor stub
}
int main() {
	Vector2D v1 =  Vector2D(3,4);
	Vector2D v2;
	v2.x =4;
	v2.y=5;
	cout << "v1 len;" << v1.length() <<'\n';
	cout << "v2 len;" << v2.length() <<'\n';
}
