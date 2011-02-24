/*
 * Ex6.cpp
 *
 *  Created on: 04-Oct-2009
 *      Author: robm
 */

#include "Ex6.h"
#include "House.h";
#include "Building.h";

Ex6::Ex6() {
	// TODO Auto-generated constructor stub

}

Ex6::~Ex6() {
	// TODO Auto-generated destructor stub
}
int main() {
	Building b1 = Building(5,2,60);
	House h1 = House("loombah",4,2,b1);
	House h2;
	h2.setArea(30);
	h2.setBathrooms(2);
	h2.setBedrooms(7);
	h2.setFloors(3);
	h2.setName("Mansion");
	h2.setRooms(20);

	b1.print();
	h1.print();
	h2.print();
}
