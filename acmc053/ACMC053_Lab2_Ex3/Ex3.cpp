/*
 * Ex3.cpp
 *
 * Created on: 04-Oct-2009
 * Author: robm
 */
using namespace std;
#include "Ex3.h"
#include <iostream>
//#include "math.h"

Ex3::Ex3() {
	// TODO Auto-generated constructor stub

}

Ex3::~Ex3() {
	// TODO Auto-generated destructor stub
}
int main() {
    float i;
    cout << "Type a number: ";
    cin >> i;
    float j;
    cout << "Type another number: ";
    cin >> j;
    cout << i << " times "<< j << " is " << (i*j) << '\n';
    cout << " max is " << max(i,j) << '\n';
}
