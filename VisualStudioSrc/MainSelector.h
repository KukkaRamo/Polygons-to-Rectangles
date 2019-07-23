#pragma once
#include "BoundingBox.h"
#include "OriginalCorners.h"
class MainSelector // Abstract class for choosing the selection method for main corners
{
public:
	MainSelector();
	virtual ~MainSelector();
	virtual void selectMainCorners(OriginalCorners& myCorners, const BoundingBox& box, int pMainCornerIdxs[], const int mainCorners) = 0;
};