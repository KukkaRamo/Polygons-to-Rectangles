#pragma once
#include "BoundingBox.h"
#include "OriginalCorners.h"
#include "Point.h"
#include "SelectByBounding.h"
#include "SelectorFactory.h"
class Polygon
{
public:
	// Please do not modify these constants, the software is intended for making rectangles
	const int MinInputCorners = 3; // You may modify this to 4 if you do not want to input any triangles
	const int NumMainCorners = 4; // Must be at least MinInputCorners

	double realLevel;
	OriginalCorners myCorners;
	BoundingBox myBoundingBox;
	SelectorFactory mySelectorFactory;
	int mainCornerIdxs[4];

	Polygon(int pInputCorners, SelectorFactory::Product pProduct);
	~Polygon();
	int getMinInputCorners() const;
	int mainIndexModulo(const int p) const;
	void setRealLevel(const double pLevel);
	void addCorner(const double pX, const double pY);
	void completeCorners();
	void selectMainCorners();
	void moveCorners();
};

