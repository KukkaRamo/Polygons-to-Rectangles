#pragma once
#include "Point.h"
class Utilities
{
public:
	Utilities();
	~Utilities();
	static double ReadDoubleString(const char* inputString);
	static Point AvgCorner(const Point firstPoint, const Point secondPoint) ;
};