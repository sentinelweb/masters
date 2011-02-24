/*
 * HelloWorld.cpp
 *
 *  Created on: 03-Oct-2009
 *      Author: robm
 */
using namespace std;

#include "Ex2.h"
#include <iostream>
#include <iomanip>

Ex2::Ex2() {
	cout<<"construct"<<endl;
}

Ex2::~Ex2() {
	cout<<"destruct"<<endl;
}

int main() {
	char c = 64;
	char *a=&c;char& p=*a;
	int d = p;
	cout<<c<<" "<<d<<endl;
	Ex2 h;
    cout<<"Hello world !"<<endl;
    cout<<"modtst:"<<-5%2<<endl;
    // test int fcasting
   cout << 0.55 << ":" << (int)0.55 <<endl;
   cout << 0.95 << ":" << (int)0.95 <<endl;
   cout << -0.55 << ":" << (int)-0.55 <<endl;
   cout << -0.95 << ":" << (int)-0.95 <<endl;
   cout << 1.55 << ":" << (int)1.55 <<endl;
   cout << -1.55 << ":" << ((signed int)-1.55) <<endl;
   cout << -1 << endl;

    float i;
    cout << "Type a number: ";
    cin >> i;
    cout.setf(ios_base::hex, ios_base::basefield);// outputs numbers in hex
    cout.setf(ios_base::fixed, ios_base::floatfield);
    cout << i << " times 3 is " << (i*3) << '\n';


}
