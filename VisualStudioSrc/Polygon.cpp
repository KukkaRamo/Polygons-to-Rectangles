#include "pch.h"
#include "Point.h"
#include "Polygon.h"
#include "SelectorFactory.h"
#include "Utilities.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include "OriginalCorners.h"
using namespace std;

Polygon::Polygon(int pInputCorners, SelectorFactory::Product pProduct) : 
	myCorners (OriginalCorners(max(pInputCorners, NumMainCorners))),
	mySelectorFactory(pProduct)
{
}

Polygon::~Polygon()
{
}

int index(int p) {
	return p % 4;
}

// Getter for the constant for the minimum required number of input corners
int Polygon::getMinInputCorners() const {
	return MinInputCorners;
}

// Getting an incremented or decremented index circularly
// Precondition: p is at least -NumMainCorners
int Polygon::mainIndexModulo(const int p) const {
	return (p + NumMainCorners) % NumMainCorners;
}

// Setting the level of reality, throws if the level is erroneous
void Polygon::setRealLevel(const double pLevel) {
	if (pLevel >= 0 && pLevel <= 1)
		realLevel = pLevel;
	else throw runtime_error("Real level must be [0..1], it is " + to_string(pLevel));
}

// Add a corner to the structure of original corners and updates bounding box
void Polygon::addCorner(const double pX, const double pY) {
	Point p = Point(pX, pY);
	myCorners.addCorner(p);
	myBoundingBox.updateOnAdd(p);
}

// If there are too few corners on input, more of them are built automatically
void Polygon::completeCorners() {
	while (myCorners.NCorners < NumMainCorners) {
		// If the input polygon is a triangle,
		// we add a 180-degree corner to make the number of corners to be at least NumMainCorners
		// Duplicate corners allowed but there must be 4 corners, so let us make the fourth one
		myCorners.Corners[myCorners.NCorners] = Utilities::AvgCorner(myCorners.Corners[myCorners.NCorners - 1], myCorners.Corners[0]); 
		// If MinInputCorners is decreased, note that the previous line must work (Ncorners must be at least one)
		++myCorners.NCorners;
	}
}

// If there are more than four corners, selects four as main corners
// The corners are selected in some rotation order
// The selection policy is chosen by a factory, following the factory design pattern
void Polygon::selectMainCorners() {
	if (myCorners.NCorners > NumMainCorners) {
		unique_ptr <MainSelector> myMainSelector = nullptr;
		try {
			myMainSelector = mySelectorFactory.chooseSelector();
			myMainSelector->selectMainCorners(myCorners, myBoundingBox, mainCornerIdxs, NumMainCorners);
		}
		catch (exception e) {
			cerr << e.what(); // If e is lost here on delete, the message has to be written here
			throw e;
		}
	}
	else {
		for (int i = 0; i < myCorners.NCorners; ++i) {
			myCorners.IsMain[i] = true;
			mainCornerIdxs[i] = i;
		}
	}
}

// Moves the main corners towards bounding box corners in some rotation direction
// and turns other corners into straight 180-degree corners
void Polygon::moveCorners() {
	// Main corners more perpendicular
		// First stage:  Choose the start corner
		// in order to make the total moving distance as small as possible
	double curDist = DBL_MAX;
	int loopRotDir = -1;
	int firstMoved = 0; // Starts from the first corner if all distance sums are INT_MAX
	int rotDirection = loopRotDir;
	do{
		for (int loopStartCorner = 0; loopStartCorner < NumMainCorners; ++loopStartCorner) {
			double rotdist = 0;
			for (int currentCorner = 0; currentCorner < NumMainCorners; ++currentCorner) {
				int rotIdx = mainIndexModulo(loopStartCorner + loopRotDir * currentCorner);
				rotdist += myCorners.Corners[mainCornerIdxs[rotIdx]].distanceTo (myBoundingBox.BoundingCorner(currentCorner));
			}
			if (rotdist < curDist) {
				firstMoved = loopStartCorner;
				curDist = rotdist;
				rotDirection = loopRotDir;
			}
		}
		loopRotDir += 2;
	} while (loopRotDir <= 1);
	// Second stage: Move each main corner towards a corner of the bounding box,
	// one per each corner,
	// by performing the rotation chosen in the first stage
	for (int i = 0; i < NumMainCorners; ++i) {
		int moveCornerIndex = mainCornerIdxs[mainIndexModulo(firstMoved + i * rotDirection)];
		myCorners.Corners[moveCornerIndex].moveTowards(myBoundingBox.BoundingCorner(i), 1 - realLevel);
	}

	// Move eliminated (non-main) corners
	int nextMain = 0; // mainCornerIdxs[0] is the index in Corners-array for the first main corner
	for (int loopCornerIdx = 0; loopCornerIdx < myCorners.NCorners; ++loopCornerIdx)
	{
		if (myCorners.IsMain[loopCornerIdx]) {
			nextMain = mainIndexModulo(nextMain + 1);
			continue;
		}
		// Move towards the middle point of the line through previous and next main corner
		myCorners.Corners[loopCornerIdx].moveTowards (Utilities::AvgCorner(myCorners.Corners[mainCornerIdxs[nextMain]],
			myCorners.Corners[mainCornerIdxs[mainIndexModulo(nextMain - 1)]]), 1 - realLevel);
		// NumMainCorners is certainly at least 4, so the indexing works
	}
}