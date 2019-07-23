#pragma once
#include "MainSelector.h"
class SelectBySize :
	public MainSelector // Main corners are selected based primarily on size
	// but bounding is considered by choosing only one corner per one main loop round

{
public:
	SelectBySize();
	virtual ~SelectBySize();
	void selectMainCorners(OriginalCorners& myCorners, const BoundingBox& box, int pMainCornerIdxs[], const int mainCorners);
};

