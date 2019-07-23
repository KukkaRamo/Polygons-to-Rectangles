#pragma once
#include "Point.h"
#include <float.h>
class BoundingBox
{
public:
	static const int NOTCORNER = - 1; // This must not be an existing corner index
	static const int NOTSIDE = -1; // This must not be an existing side index
	double minX;
	double maxX;
	double minY;
	double maxY;
	// Bounding box corners have fixed indexes 0-3 counterclockwise from lower left (0) 
	double* rotationMap[4][2] = { {&minX, &minY}, {&maxX, &minY}, {&maxX, &maxY}, {&minX, &maxY} };

	BoundingBox();
	~BoundingBox();
	Point BoundingCorner(const int cornerIndex) const;
	int BoundingIndex(const Point pCorner) const;
	int onSide(const Point pCorner) const;
	double distOnSide(const int side, const Point pCorner) const;
	void updateOnAdd(const Point pAdded);
};

