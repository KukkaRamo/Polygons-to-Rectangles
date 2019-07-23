#include "pch.h"
#include "Point.h"
#include <math.h>

Point::Point()
{
}

Point::Point(const double pX, const double pY) : x(pX), y(pY)
{
}


Point::~Point()
{
}

// Moves the point towards a target point given in parameters
// The distance moved is given as a parameter
void Point::moveTowards(const Point pTowards, const double howMuch) {
	x += howMuch * (pTowards.x - x);
	y += howMuch * (pTowards.y - y);
}

// Calculates the distance between the point and a target point
// The target point is given as the parameter
double Point::distanceTo(const Point target) const {
	return sqrt(pow((target.x - x), 2) + pow((target.y - y), 2));
}
