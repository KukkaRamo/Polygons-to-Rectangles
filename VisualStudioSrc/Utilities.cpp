#include "pch.h"
#include "Point.h"
#include "Utilities.h"
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>
using namespace std;


Utilities::Utilities()
{
}


Utilities::~Utilities()
{
}

// Reads a string into a double and throws on format error
double Utilities::ReadDoubleString(const char* inputString) {
	char* endptr;
	double doubleValue = strtod(inputString, &endptr);
	if (endptr != inputString + strlen(inputString)) {
		cerr << inputString << " is not a double"; // Errno
		throw runtime_error("Something follows the double in " + *inputString);
		 // Something is in the argument after the double
	}
	return doubleValue;
}

// Calculates an average point between two 2D-points
Point Utilities::AvgCorner(const Point firstPoint, const Point secondPoint) {
	Point middlePoint; // a point in the middle point of the line between the first point and the second point
	middlePoint.x = (firstPoint.x + secondPoint.x) / 2;
	middlePoint.y = (firstPoint.y + secondPoint.y) / 2;
	return middlePoint;
}


