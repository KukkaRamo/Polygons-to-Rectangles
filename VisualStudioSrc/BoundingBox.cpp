#include "pch.h"
#include "BoundingBox.h"
#include "Point.h"
#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;

BoundingBox::BoundingBox()
{
	minX = DBL_MAX;
	maxX = DBL_MIN;
	minY = DBL_MAX;
	maxY = DBL_MIN;
}

BoundingBox::~BoundingBox()
{
}

// Returns the bounding box corner corresponding the index cornerIndex
// Bounding box corners have fixed indexes 0-3 counterclockwise from lower left (0) 
Point BoundingBox::BoundingCorner(const int cornerIndex) const {
	if (cornerIndex < 0 || cornerIndex > 3) {
		throw runtime_error("\nErroneous corner index " + to_string(cornerIndex) + " , must be 0-3");
	}
	return Point (*rotationMap[cornerIndex][0], *rotationMap[cornerIndex][1]);
}

// If the input parameter corner is close enough to a bounding box corner,
// returns the corresponding corner index, otherwise returns an integer corresponging
// non-existence
// In narrow bounding boxes, precision issues may occur
int BoundingBox::BoundingIndex(const Point pCorner) const {
	int myIdx = NOTCORNER;
	const double eps = 0.0001;
	for (int i = 0; i < 4; ++i) {
		if (abs(pCorner.x - *rotationMap[i][0]) < eps
			&& abs(pCorner.y - *rotationMap[i][1]) < eps)
				myIdx = i; // Found corresponding corner in bounding box
	}
	return myIdx;
}

// Precondition: must not be on the same line as the side outside the side
// The sides are counterclockwise, and the first side is ymin (lower left edge)
// So the side is always counterclockwise next to the corner in rotation map
// With very narrow bounding boxes, precision issues may occur
int BoundingBox::onSide(const Point pCorner) const {
	int mySide = NOTSIDE;
	const double eps = 0.0001;
	int isY = 1; // First side is in y-direction
	for (int i = 0; i < 4; ++i) {
		if (abs(((isY) ? pCorner.y : pCorner.x) - *rotationMap[i][isY]) < eps)
			mySide = i; // Found corresponding side in bounding box
		isY = 1 - isY; // complement since every second side is in y-direction
	}
	return mySide;
}

// Precondition: input parameter corner must be on the input parameter side, no check here!
// Precodition: input parameter corner must be inside or very close to the bounding box
// Absolute values can be used since the point is never way out of the bounding box
// Distance is from the corner in the previous side
double BoundingBox::distOnSide(const int side, const Point pCorner) const {
	double myDist = -1;
	if (side >= 0 && side <= 3)
		myDist = pCorner.distanceTo(Point(*rotationMap[side][0], *rotationMap[side][1]));
	return myDist;
}

// When a corner pAdded has been read or created for the polygon, the bounding box is updated
void BoundingBox::updateOnAdd(const Point pAdded) {
	if (pAdded.x < minX) minX = pAdded.x;
	if (pAdded.x > maxX) maxX = pAdded.x;
	if (pAdded.y < minY) minY = pAdded.y;
	if (pAdded.y > maxY) maxY = pAdded.y;
}