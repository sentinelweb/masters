/*
 * House.h
 *
 *  Created on: 04-Oct-2009
 *      Author: robm
 */

#ifndef IGLOO_H_
#define HOUSE_H_
using namespace std;
#include "Building.h"
#include <string>

class Igloo: public Building {
public:
	string name;
	int bedrooms;
	int bathrooms;

	string getName() {return name;}
    void setName(string name) {this->name = name; }

    int getBedrooms() const {return bedrooms; }
    void setBedrooms(int bedrooms)  {this->bedrooms = bedrooms;}

    int getBathrooms() const  {return bathrooms;}
    void setBathrooms(int bathrooms) {this->bathrooms = bathrooms;}

    Igloo();
    Igloo(string name,int bedrooms,int bathrooms,Building b);
    virtual void build();
	virtual ~Igloo();

	void print();
};

#endif /* HOUSE_H_ */
