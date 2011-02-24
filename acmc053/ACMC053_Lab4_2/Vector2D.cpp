/*
 * Vector2D.cpp
 *
 *  Created on: 04-Oct-2009
 *      Author: robm
 */

#include "Vector2D.h"
#include <cmath>

Vector2D::Vector2D() {
	// TODO Auto-generated constructor stub

}
Vector2D::Vector2D(double x_, double y_) {
	x=x_;
	y=y_;
}

double Vector2D::length() {
	return sqrt(pow(x,2)+pow(y,2)) ;
}

// Overload == for Vector.
bool Vector2D::operator==(Vector2D op2){
  return x==op2.x && y==op2.y;
}

Vector2D::~Vector2D() {
	// TODO Auto-generated destructor stub
}
