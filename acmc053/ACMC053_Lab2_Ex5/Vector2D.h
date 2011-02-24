/*
 * Vector2D.h
 *
 *  Created on: 04-Oct-2009
 *      Author: robm
 */

#ifndef VECTOR2D_H_
#define VECTOR2D_H_

class Vector2D {
public:
	double x;
	double y;
	Vector2D();
	Vector2D(double x, double y);
	double length();
	virtual ~Vector2D();
};

#endif /* VECTOR2D_H_ */
