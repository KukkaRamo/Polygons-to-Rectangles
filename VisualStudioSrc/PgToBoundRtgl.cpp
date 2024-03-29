#include "pch.h"
#include "Polygon.h"
#include "SelectByBounding.h"
#include "Utilities.h"
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <string>
using namespace std;

int main(int argc, char** argv)
{
	try {
		Polygon myPolygon = Polygon((argc - 2) / 2, SelectorFactory::BOUNDINGPRODUCT);
		// Check arguments and read in real level and corners
		int minCorners = myPolygon.getMinInputCorners();
		if (argc < 2 * minCorners + 2) {
			throw runtime_error("Please give the real level and then at least " + to_string(minCorners) + " corners");
			// Must be at least realLevel and three corners (and program name)
		}
		if (argc % 2 == 1) {
			throw runtime_error("Please give the real level and then both coordinates for each corner");
			// Every corner does not have both coordinates
		}
		myPolygon.setRealLevel(Utilities::ReadDoubleString(argv[1]));
		for (int i = 2; i < argc; i += 2)
			myPolygon.addCorner(Utilities::ReadDoubleString(argv[i]), 
				Utilities::ReadDoubleString(argv[i + 1]));
		myPolygon.completeCorners();
		myPolygon.selectMainCorners();
		myPolygon.moveCorners();
		for (int i = 0; i < myPolygon.myCorners.NCorners; ++i)
			cout << myPolygon.myCorners.Corners[i].x << " " << myPolygon.myCorners.Corners[i].y << " ";
	}
	catch (exception e) {
		cerr << "An exception occurred\n";
		cerr << e.what();
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

