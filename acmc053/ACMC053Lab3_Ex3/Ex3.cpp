/*
 * Ex3.cpp
 *
 *  Created on: 11-Oct-2009
 *      Author: robm
 */
#include <iostream>
using namespace std;

#include "Ex3.h"

class base {
public:
     virtual void vfunc() {
           cout << "This is base’s vfunc().\n";
     }
};
class derived1 : public base {
public:
     void vfunc() {
           cout << "This is derived1’s vfunc().\n";
     }
};

int main(){
     base *p, b;
     derived1 d1;
     p = &b;
     p->vfunc();
     p = &d1;
     p->vfunc();
     (*p).vfunc();// alternative for ->
     return 0;
}
Ex3::Ex3() {
	// TODO Auto-generated constructor stub

}

Ex3::~Ex3() {
	// TODO Auto-generated destructor stub
}
