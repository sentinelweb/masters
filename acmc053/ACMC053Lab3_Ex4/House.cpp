/*
 * House.cpp
 *
 *  Created on: 04-Oct-2009
 *      Author: robm
 */
#include <iostream>
#include "House.h"
#include "Building.h"

House::House() {
	this->name="";
	this->bedrooms=0;
	this->bathrooms=0;
}

House::House(string name,int bedrooms,int bathrooms,Building b) {
	this->name=name;
	this->bedrooms=bedrooms;
	this->bathrooms=bathrooms;
	this->floors=b.getFloors();
	this->rooms=b.getRooms();
	this->area=b.getArea();
}

House::~House() {
	// TODO Auto-generated destructor stub
}
void House::build(){
	cout << "house:" << "build" << "\n";
}

void House::print() {
	cout << "house:" << this->name << ":" << this->bedrooms << ":" <<this->bathrooms << ":" << '\n';
	// this doesn't work how do i refer o the base class method?
	//((Building)this).print();
	Building::print();
}
