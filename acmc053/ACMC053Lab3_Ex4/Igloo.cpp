/*
 * House.cpp
 *
 *  Created on: 04-Oct-2009
 *      Author: robm
 */
#include <iostream>
#include "Igloo.h"
#include "Building.h"

Igloo::Igloo() {
	this->name="";
	this->bedrooms=0;
	this->bathrooms=0;
}

Igloo::Igloo(string name,int bedrooms,int bathrooms,Building b) {
	this->name=name;
	this->bedrooms=bedrooms;
	this->bathrooms=bathrooms;
	this->floors=b.getFloors();
	this->rooms=b.getRooms();
	this->area=b.getArea();
}

Igloo::~Igloo() {
	// TODO Auto-generated destructor stub
}
void Igloo::build(){
	cout << "igloo:" << "build" << "\n";
}

void Igloo::print() {
	cout << "igloo:" << this->name << ":" << this->bedrooms << ":" <<this->bathrooms << ":" << '\n';
	// this doesn't work how do i refer o the base class method?
	//((Building)this).print();
	Building::print();
}
