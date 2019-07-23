#include "pch.h"
#include "OriginalCorners.h"

OriginalCorners::OriginalCorners(const int pCornerCount)
{
	NCorners = 0;
	Corners = new Point[pCornerCount];
	IsMain = new bool[pCornerCount];
}

OriginalCorners::~OriginalCorners()
{
	delete[] Corners;
	delete[] IsMain;
}

// Adds a corner to this data structure
void OriginalCorners::addCorner(const Point p) {
	Corners[NCorners] = p;
	IsMain[NCorners++] = false;
}

// Sets isMain for all corners to the value of the input parameter
void OriginalCorners::setAllIsMain(const bool value) {
	for (int i = 0; i < NCorners; ++i)
		IsMain[i] = value;
}