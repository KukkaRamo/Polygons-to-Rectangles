#include "pch.h"
#include "BoundingBox.h"
#include "SelectByBounding.h"
#include <stdexcept>
#include <string>
using namespace std;

SelectByBounding::SelectByBounding()
{
}

SelectByBounding::~SelectByBounding()
{
}

int index1(int p) {
	return p % 4;
}

// Updates the internal structure here that contains four closest original corners for any bounding box corner
void SelectByBounding::updateBoundingArrays(const BoundingBox& box, const int index, const Point corner) {
	for (int i = 0; i < 4; ++i) {
		double dist = corner.distanceTo(box.BoundingCorner(i));
		int j = NumClosestCorners[i];
		while (j > 0 && dist < ClosestDist[i][j - 1]) {
			if (j < 4) // The last one just drops out
			{
				ClosestCornerIndeces[i][j] = ClosestCornerIndeces[i][j - 1];
				ClosestDist[i][j] = ClosestDist[i][j - 1];
			}
			--j;
		}
		if (j < 4) {
			ClosestCornerIndeces[i][j] = index;
			ClosestDist[i][j] = dist;
			if (NumClosestCorners[i] < 4)
				(NumClosestCorners[i])++;
		}
	}
}
// Finds an original corner closest to the bounding box corner that has the index given as a parameter
// The Not-argumets (first, second, and third) are corners that are passed, so none of them can be returned
// The default values for Not-parameters are values that cannot be corners
int SelectByBounding::closestCorner(const int index, const int firstNot, const int secondNot, const int thirdNot) const {
	int closestIndex = BoundingBox::NOTCORNER;

	const int num = NumClosestCorners[index];
	if (num == 0)
		throw runtime_error("No corners, update bounding box first");
	for (int j = 0; j < num; ++j) {
		int thisCorner = ClosestCornerIndeces[index][j];
		if ((firstNot == NOTUSED || thisCorner != firstNot)
			&& (secondNot == NOTUSED || thisCorner != secondNot)
			&& (thirdNot == NOTUSED || thisCorner != thirdNot))
		{
			closestIndex = thisCorner;
			break;
		}
	}
	return closestIndex;
}

// This is one trial when looking for appropriate corners according to how close they are to
// the bounding ox corners
void SelectByBounding::sortByControl(OriginalCorners& myCorners, const BoundingBox& box, const char* commandString, const int startCorner) {
	// Command string [[lLrRwW][0123]] {0-4} [cC][0123] {0-4} \0 and at most four commands (a command has a letter and a digit)
	// Returns dist and corner's complement number of integers
	//	Caller keeps track of smallest distances
	if (strlen(commandString) > 8) throw runtime_error("Too many arguments in command");
	int commandLength = strlen(commandString); // size_t and int are certainly at least 8
	if (commandLength % 2 == 1) throw runtime_error("Wrong format for commands");
	if (commandLength / 2 + nBoxCorners != 4) throw runtime_error("Wrong number of commands with respect to ready corners");
	int boundingArgs[4];
	double dist = 0;
	int commandCorners = 0;
	int charIndex = 0;
	char cmd;
	int indexFor0, indexFor1;
	int readyBoundingArgs = 0;
	for (int i = 0; i < nBoxCorners; ++i)
		boundingArgs[readyBoundingArgs++] = wCorners[i];
	// bounding args wcorner 4 - command length/2 corners 
	while ((cmd = commandString[charIndex++]) != '\0') {
		int number = commandString[charIndex++] - '0'; // This method throws if number is an index out of bounds
		switch(cmd) {
		case 'L': {
			if (commandCorners > 0) throw runtime_error("Other commands after corners");
			dist += left[index1(startCorner + number)];
			boundingArgs[readyBoundingArgs++] = leftCorner[index1(startCorner + number)];
			break;
		}
		case 'R': {
			if (commandCorners > 0) throw runtime_error("Other commands after corners");
			dist += right[index1(startCorner + number)];
			boundingArgs[readyBoundingArgs++] = rightCorner[index1(startCorner + number)];
			break;
		}
		case 'C': {
			if (commandCorners == 0) {
				indexFor0 = closestCorner(index1(startCorner + number),
					(readyBoundingArgs >= 1) ? boundingArgs[0] : NOTUSED,
					(readyBoundingArgs >= 2) ? boundingArgs[1] : NOTUSED,
					(readyBoundingArgs == 3) ? boundingArgs[2] : NOTUSED
				);
				dist += myCorners.Corners[indexFor0].distanceTo(box.BoundingCorner(index1(startCorner + number)));
				boundingArgs[readyBoundingArgs++] = indexFor0;
				++commandCorners;
			}
			else if (commandCorners == 1) {
				indexFor1 = closestCorner(index1(startCorner + number), indexFor0,
					(readyBoundingArgs >= 2) ? boundingArgs[0] : NOTUSED,
					(readyBoundingArgs == 3) ? boundingArgs[1] : NOTUSED
				);
				dist += myCorners.Corners[indexFor1].distanceTo(box.BoundingCorner(index1(startCorner + number)));
				boundingArgs[readyBoundingArgs++] = indexFor1;
				++commandCorners;
			}
			else throw runtime_error("Too many corners for me");
			break;
		}
		default: {
			throw runtime_error("Unknown command in command string");
		}
		}

	}
	if (controlDist == -1 || dist < controlDist) {
		for (int i = nBoxCorners; i < 4; ++i)
			wCorners[i] = boundingArgs[i];
		controlDist = dist;
	}
	
}

// Chooses four corners as main corners according to theis contact with or close location to the bounding box 
void SelectByBounding::selectMainCorners(OriginalCorners& myCorners, const BoundingBox& box, int pMainCornerIdxs[], const int mainCorners) {
for (int i = 0; i < myCorners.NCorners; ++i) {
	int wind;
	int mySide;
	if ((wind = box.BoundingIndex(myCorners.Corners[i])) != box.NOTCORNER) {
		if (!cMap.test(wind)) {
			wCorners[nBoxCorners++] = i;
			cMap.set(wind);
		}
	}
	else if ((mySide = box.onSide(myCorners.Corners[i])) != box.NOTSIDE) {
		double dist = box.distOnSide(mySide, myCorners.Corners[i]);
		if (nSide[mySide] == 0) {
			left[mySide] = right[mySide] = dist;
			leftCorner[mySide] = rightCorner[mySide] = i;
		}
		else {
			if (dist < left[mySide]) {
				left[mySide] = dist;
				leftCorner[mySide] = i;
			}
			else if (dist > right[mySide]) {
				right[mySide] = dist;
				rightCorner[mySide] = i;
			}
		}
		++(nSide[mySide]);
	}
	updateBoundingArrays(box, i, myCorners.Corners[i]);
}

switch (cMap.count()) { // (Length of command string /2) + cMap.Count() must always be 4
case 0: { // Fill corners with rotations
	sortByControl(myCorners, box, "L0L1L2L3", 0);
	sortByControl(myCorners, box, "R0R1R2R3", 0);
	break;
}
case 1: {
	int startCorner = -1;
	for (int j = 0; j < 4; ++j)
		if (cMap.test(j))
			startCorner = j; // Study all other corners and points 
				//far from them on the edges opposite to the start corner
	sortByControl(myCorners, box, "L1L2C3", startCorner);
	sortByControl(myCorners, box, "R1R2C1", startCorner);
	sortByControl(myCorners, box, "L1R2C2", startCorner);
	break;
}
case 2: {
	if (cMap[0] == cMap[2]) { // Diagonal,
		// look at the other corners in both orders
		int startCorner = (cMap[0]) ? 0 : 1;
		sortByControl(myCorners, box, "C1C3", startCorner);
		sortByControl(myCorners, box, "C3C1", startCorner);
	}
	else { // Adjacent, look at a point and a corner from the opposite edge
		int startCorner = ((cMap[3]) ? 2 : 0) + ((cMap[0] != cMap[1]) ? 1 : 0);
		sortByControl(myCorners, box, "R2C2", startCorner);
		sortByControl(myCorners, box, "L2C3", startCorner);
	}
	break;
}
case 3: {
	int missingCorner = -1;
	for (int i = 0; i < 4; ++i)
		if (!cMap.test(i))
			missingCorner = i; // Look at the missing corner
	wCorners[nBoxCorners++] = closestCorner(missingCorner, wCorners[0], wCorners[1], wCorners[2]);
	break;
}
case 4: { // Do  nothing, you already have all corners
	break;
}
}
for (int i = 0; i < 4; ++i) { // Sort according to index
	pMainCornerIdxs[i] = wCorners[i];
	myCorners.IsMain[wCorners[i]] = true;
	for (int j = i - 1; j >= 0; --j) {
		int k = j + 1;
		if (pMainCornerIdxs[j] > pMainCornerIdxs[k]) {
			pMainCornerIdxs[j] ^= pMainCornerIdxs[k];
			pMainCornerIdxs[k] ^= pMainCornerIdxs[j];
			pMainCornerIdxs[j] ^= pMainCornerIdxs[k];
		}
	}
}
}

