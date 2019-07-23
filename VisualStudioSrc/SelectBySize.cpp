#include "pch.h"
#include "SelectBySize.h"
#include "Utilities.h"
#include <float.h>

SelectBySize::SelectBySize()
{
}

SelectBySize::~SelectBySize()
{
}

// Selects the four main corners based on the size of the corners
// It does it one by one so that not all parts formed by small corners are not lost, but it is slow
// Precondition: pMainCornerIdxs must have at least one corner (it has four)
void SelectBySize::selectMainCorners(OriginalCorners& myCorners, const BoundingBox& box, int pMainCornerIdxs[], const int mainCorners){
	myCorners.setAllIsMain(true);
	for (int i = 0; i < myCorners.NCorners - mainCorners; ++i) // Keep at most mainCorners corners
	{
		// The rest of the corners removed one at each round of this for-i-loop
		double dmin = DBL_MAX;
		int next, elim = -1, distForMe;
		for (int prev = 0; prev < myCorners.NCorners; ++prev)  // Eliminate the one that is 
		// closest to the line between its adjacent uneliminated corners 
		{
			if (myCorners.IsMain[prev] == false) continue; // I have already been eliminated, continue
			// we are here - prev is uneliminated so it is the previous adjacent uneliminated corner
			distForMe = prev;
			do { distForMe = (distForMe + 1) % myCorners.NCorners; }
			while (myCorners.IsMain[distForMe] == false); // Next unelim after prev is the current corner
			next = distForMe; // Current corner for which the distance is being counted
			do { next = (next + 1) % myCorners.NCorners; }
			while (myCorners.IsMain[next] == false); // Next uneliminated is the next adjacent uneliminated
			double dist = myCorners.Corners[distForMe].distanceTo(Utilities::AvgCorner
				(myCorners.Corners[prev], myCorners.Corners[next]));
			if (dist < dmin) { // whether this is the one to be eliminated
				dmin = dist;
				elim = distForMe;
			}
		}
		myCorners.IsMain[elim] = false; // Internal error if elim is still -1, must throw
	} // end of for i
	int cornerIndex = 0;
	for (int i = 0; i < myCorners.NCorners; ++i)
		if (myCorners.IsMain[i]==true)
			pMainCornerIdxs[cornerIndex++] = i;
}