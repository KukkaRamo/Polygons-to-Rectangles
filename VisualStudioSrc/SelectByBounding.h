#pragma once
#include "BoundingBox.h"
#include "MainSelector.h"
#include "OriginalCorners.h"
#include "Point.h"
#include <bitset>
#include <string.h>
using namespace std;

class SelectByBounding: public MainSelector // Main corners are selected based on bounding box
{
public:
	static const int NOTUSED = -1; // This must not be an existing corner index
	int nBoxCorners = 0;
	bitset<4> cMap = bitset<4>(string("0000"));
	int wCorners[4];
	int nSide[4] = {0, 0, 0, 0};
	double left[4];
	double right[4];
	int leftCorner[4];
	int rightCorner[4];
	double controlDist = -1;
	int NumClosestCorners[4] = { 0,0,0,0 };
	int ClosestCornerIndeces[4][4];
	double ClosestDist[4][4];

	SelectByBounding();
	virtual ~SelectByBounding();
	void updateBoundingArrays(const BoundingBox& box, const int index, const Point corner);
	int closestCorner(const int index, const int firstNot = NOTUSED, const int secondNot = NOTUSED, const int thirdNot = NOTUSED) const;
	void selectMainCorners(OriginalCorners& myCorners, const BoundingBox& box, int pMainCornerIdxs[], const int mainCorners);
	void sortByControl(OriginalCorners& myCorners, const BoundingBox& box, const char* commandString, const int startCorner);
};

