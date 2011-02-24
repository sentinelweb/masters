/*
 * Building.cpp
 *
 *  Created on: 04-Oct-2009
 *      Author: robm
 */

#include <iostream>
#include "Building.h"

Building::Building() {}
Building::Building(int rooms, int floors, double area) {
	this->rooms = rooms;
	this->floors=floors;
	this->area=area;
}

Building::~Building() {
	// TODO Auto-generated destructor stub
}
void Building::print() {
	cout << "building:" << this->rooms << ":" << this->floors << ":" <<this->area << ":" << '\n';
}
