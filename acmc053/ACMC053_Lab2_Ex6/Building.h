/*
 * Building.h
 *
 *  Created on: 04-Oct-2009
 *      Author: robm
 */
using namespace std;
#ifndef BUILDING_H_
#define BUILDING_H_

class Building {
public:

    int getRooms() const {   return rooms;  }
    void setRooms(int rooms) {     this->rooms = rooms;  }

    int getFloors() const  {     return floors;   }
    void setFloors(int floors)   {       this->floors = floors;   }

    int getArea() const  {       return area;  }
    void setArea(int area)  {    this->area = area;   }

    Building();
	Building(int rooms, int floors, double area);
	virtual ~Building();

	void print();
protected:
	int rooms ;
	int floors ;
	int area ;

};

#endif /* BUILDING_H_ */
