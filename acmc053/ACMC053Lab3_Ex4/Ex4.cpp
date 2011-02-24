/*
 * Ex4.cpp
 *
 *  Created on: 11-Oct-2009
 *      Author: robm
 */

#include "Ex4.h"
#include "Building.h"
#include "House.h"
//#include "Igloo.h"
#include "IceCastle.h"
Ex4::Ex4() {
	// TODO Auto-generated constructor stub

}

Ex4::~Ex4() {
	// TODO Auto-generated destructor stub
}
int main() {
	Building *b = new Building(2,2,23);
	b->build();
	Building *hb = new Building (2,2,34);
	House *h=new House("loombah",2,2,*hb);
	b->build();
	h->build();
	Igloo *i = new Igloo("icey",3,3,*hb);
	i->build();
	IceCastle *ic = new IceCastle();
	ic->build();


}
