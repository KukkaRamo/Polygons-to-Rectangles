// How to make a polygon (real level 1) to be printed as a rectangle (real level 0),
// and everything between these according to the selected real level realLev, between 0 and 1

#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	double x;
	double y;
} Corner;

typedef struct {
	Corner boxMin;
	Corner boxMax;
} BoundingBox ;

enum StateType* State = NULL;
Corner* myCorners = NULL;
int* unelimCornerIdxs = NULL;

// Please do not modify these constants, the software is intended for making rectangles
const int MinInputCorners = 3; // You may modify this to 4 if you do not want to input any triangles
const int NumUneliminatedCorners = 4;

Corner minCorner = { DBL_MAX, DBL_MAX };
Corner maxCorner = { DBL_MIN, DBL_MIN };

enum StateType {ELIMINATED, UNELIMINATED};

int myMax (const int a, const int b) {
	return (a < b) ? b : a;
}

void clearPointers() {
	if (State != NULL) free (State);
	if (myCorners != NULL) free (myCorners);
	if (unelimCornerIdxs != NULL) free (unelimCornerIdxs);
}

void errorOccurred() {
	clearPointers();
	exit(EXIT_FAILURE);
}

double ReadDoubleString (char* const inputString) {
	char* endptr;
	double doubleValue = strtod (inputString, &endptr);
	if (endptr != inputString + strlen(inputString)) {
		printf(" %s is not a double \n", inputString);
		errorOccurred(); // Something is in the argument after the double
	}
	return doubleValue;
}

double DistBetweenCorners (const Corner firstPoint, const Corner secondPoint) {
	return sqrt(pow((secondPoint.x-firstPoint.x),2) + pow((secondPoint.y-firstPoint.y),2));
}

double CountDist(const Corner* const endPoint1, const Corner* const endPoint2, const Corner* const distancePoint) {
	double a, b, c;
	if (abs(endPoint1->x - endPoint2->x) < 0.000001 * (endPoint2->y - endPoint1->y)) { // Avoid division by zero
		a=1; b=0; c=-endPoint1->x;
	}  // Same x coordinates vertical line
	else {
		a = (endPoint2->y - endPoint1->y)/(endPoint2->x - endPoint1->x);
		b=-1;
		c = -a * endPoint1->x + endPoint1->y;
	}
	if (a == 0 && b == 0) {
		printf ("a=b=0 in distance calculation\n");
		return (-1);
	}
	else return (abs(a*distancePoint->x+b*distancePoint->y+c))/sqrt(a*a+b*b);
}

Corner AvgCorner (const Corner firstPoint, const Corner secondPoint) {
	Corner middlePoint; // a point in the middle point of the line between the first point and the second point
	middlePoint.x = (firstPoint.x + secondPoint.x) / 2;
	middlePoint.y = (firstPoint.y + secondPoint.y) / 2;
	return middlePoint;
}

void MoveCorner (Corner* cornerMoved, const Corner moveTowards, const double howMuch) {
	cornerMoved->x += howMuch * (moveTowards.x - cornerMoved->x);
	cornerMoved->y += howMuch * (moveTowards.y - cornerMoved->y);
}

// Bounding box corners have fixed indexes 0-3 counterclockwise from lower left (0) 
Corner BoundingCorner (const BoundingBox* const box, const int cornerIndex) {
	if (cornerIndex < 0 || cornerIndex > 3) {
		printf ("\nErroneous corner index %d , must be 0-3");
		errorOccurred();
	}
	Corner boxCorner; // Bounding box corner corresponding the index cornerIndex
	boxCorner.x = (cornerIndex==0 || cornerIndex==3) ? box->boxMin.x : box->boxMax.x;
	boxCorner.y = (cornerIndex<2) ? box->boxMin.y : box->boxMax.y;
	return boxCorner;
}

int main (int argc, char** argv) {
	
	BoundingBox boundingBox = {
		minCorner,
		maxCorner
	};

	// Check arguments
	if (argc < 2 * MinInputCorners + 2) {
		printf ("\nPlease give the real level and then at least %d corners\n", MinInputCorners);
		errorOccurred(); // Must be at least realLevel and three corners (and program name)
	}
	if (argc % 2 == 1) {
		printf ("\nPlease give the real level and then both coordinates for each corner \n");
		errorOccurred(); // Every corner does not have both coordinates
	}
	int NInputCorners = (argc-2) / 2;
	double realLevel = ReadDoubleString(argv[1]);
	if (realLevel < 0 || realLevel > 1) {
		printf ("Real level must be between 0 and 1, endpoints included\n");
		errorOccurred(); // Real level must be between 0 and 1, endpoints included
	}
	const int firstCornerIdx = 2;
	int NCorners = myMax(NInputCorners, NumUneliminatedCorners);
	
	if ((State = malloc(NCorners * sizeof(enum StateType))) == NULL) {
		perror("\nFailed to allocate state array\n");
		errorOccurred();
	}
	if ((myCorners = malloc(NCorners * sizeof(Corner))) == NULL) {
		perror("\nFailed to allocate corner array\n");
		errorOccurred();
	}

	// Read in and check corners and figure out the bounding rectangle
	int argIdx = firstCornerIdx;
	for (int i=0; i<NInputCorners; ++i) {
		State[i]=UNELIMINATED;
		myCorners[i].x = ReadDoubleString(argv[argIdx++]);
		myCorners[i].y = ReadDoubleString(argv[argIdx++]);

		if (myCorners[i].x < boundingBox.boxMin.x) boundingBox.boxMin.x = myCorners[i].x;
		if (myCorners[i].x > boundingBox.boxMax.x) boundingBox.boxMax.x = myCorners[i].x;
		if (myCorners[i].y < boundingBox.boxMin.y) boundingBox.boxMin.y = myCorners[i].y;
		if (myCorners[i].y > boundingBox.boxMax.y) boundingBox.boxMax.y = myCorners[i].y;
	}

	for (int i = NInputCorners; i < NumUneliminatedCorners; ++i) {
 // If the input polygon is a triangle,
 // we add a 180-degree corner to make the number of corners to be at least NumUneliminatedCorners
		State[i]=UNELIMINATED; // i is the next index for a corner
		myCorners[i] = AvgCorner(myCorners[i-1], myCorners[0]); // MinInputCorners should not be modified, this must work
	// Duplicate corners allowed but there must be 4 corners, so let us make the fourth one
	}

	// This is unfeasible if there are very many corners
	for  (int i=0; i<NCorners-NumUneliminatedCorners; ++i) // Keep at most NumUneliminatedCorners corners
	{
	// The rest of the corners removed one at each round of this for-i-loop
		double dmin  = DBL_MAX;
		int next, elim=-1, distForMe;
		for  (int prev=0; prev<NCorners; ++prev)  // Eliminate the one that is 
		// closest to the line between its adjacent uneliminated corners 
		{
			if (State[prev] == ELIMINATED) continue; // I have already been eliminated, continue
			// we are here - prev is uneliminated so it is the previous adjacent uneliminated corner
			distForMe = prev;
			do {distForMe = (distForMe + 1) % NCorners; } while ( State[distForMe] == ELIMINATED); // Next unelim after prev is the current corner
			next = distForMe; // Current corner for which the distance is being counted
			do {next = (next + 1) % NCorners; } while ( State[next] == ELIMINATED); // Next uneliminated is the next adjacent uneliminated
			double dist = CountDist (&myCorners[prev], &myCorners[next], &myCorners[distForMe]);
			if (dist < dmin) { // whether this is the one to be eliminated
				dmin = dist;
				elim = distForMe;
			}
		}
		State [elim] = ELIMINATED; // Internal error if elim is still -1, must throw
	} // end of for i
	
	int NunelimCornerIdxs = 0; // indexes of uneliminated corners into unelimCornerIdxs
	if ((unelimCornerIdxs = malloc(NumUneliminatedCorners * sizeof(int))) == NULL) {
		perror("\nFailed to allocate uneliminated corner index array\n");
		errorOccurred();
	}
	for (int i=0; i<NCorners; ++i)
		if (State[i] != ELIMINATED ) {unelimCornerIdxs[NunelimCornerIdxs++]=i;}

	// Uneliminated corners more perpendicular
	// First stage:  Choose the start corner and rotation direction
	// in order to make the total moving distance as small as possible
	double curDist = DBL_MAX;
	int rotDir = -1;
	int firstMoved = 0; // Starts from the first corner if all distance sums are INT_MAX
	int rotDirection = rotDir;
	do {
		for (int loopStartCorner=0; loopStartCorner<NumUneliminatedCorners; ++loopStartCorner) {
			double rotdist = 0;
			for (int currentCorner=0; currentCorner<NumUneliminatedCorners; ++currentCorner) {
				int rotIdx = (loopStartCorner + NumUneliminatedCorners + rotDir * currentCorner) % NumUneliminatedCorners;
				rotdist += DistBetweenCorners(myCorners[unelimCornerIdxs[rotIdx]], BoundingCorner (&boundingBox, currentCorner));
			}
			if (rotdist < curDist) {
				firstMoved = loopStartCorner;
				curDist = rotdist;
				rotDirection = rotDir;
			}
		}
		rotDir += 2;
	} while (rotDir <= 1);
	// Second stage: Move each uneliminated corner towards a corner of the bounding box,
	// one per each corner,
	// by performing the rotation chosen in the first stage
	for (int i = 0; i < NumUneliminatedCorners; ++i) {
		int moveCornerIndex = unelimCornerIdxs[(firstMoved + i*rotDirection + NumUneliminatedCorners) % NumUneliminatedCorners];
		MoveCorner ( &myCorners[moveCornerIndex], BoundingCorner (&boundingBox, i), 1-realLevel );
	}

	// shrink eliminated distances
	int nextUnElim = 0; // unelimCornerIdxs[0] is the index in Corners-array for the first uneliminated corner
	for  (int loopCornerIdx=0; loopCornerIdx<NCorners; ++loopCornerIdx)  
	{
		if (State[loopCornerIdx] != ELIMINATED) {
			nextUnElim = (nextUnElim + 1) % NumUneliminatedCorners;
			continue;
		}
		// Shrink towards the middle point of the line through previous and next uneliminated corner
		MoveCorner (&myCorners[loopCornerIdx], AvgCorner(myCorners[unelimCornerIdxs[nextUnElim]],
			myCorners[unelimCornerIdxs[(nextUnElim + NumUneliminatedCorners - 1) % NumUneliminatedCorners]]), 1-realLevel);
			// NumUneliminatedCorners is certainly at least 4, so the indexing works
	}
	
	// Print results and clean up
	for  (int j=0; j<NCorners; ++j) 
		printf ("Corner %d %lf %lf \n", j, myCorners[j].x, myCorners[j].y);
	clearPointers();
	printf("Paina jotakin näppäintä\n");
	char c;
	scanf (&c);
	exit (EXIT_SUCCESS);

}
