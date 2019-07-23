#pragma once
#include "Point.h"
class OriginalCorners
{
public:
	int NCorners;
	Point* Corners;
	bool* IsMain;

	OriginalCorners(const int pCornerCount);
	~OriginalCorners();
	void addCorner(const Point p);
	void setAllIsMain(const bool value);
};

