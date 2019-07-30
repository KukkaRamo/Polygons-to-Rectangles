#pragma once
#include "MainSelector.h"
#include <memory>
using namespace std;

class SelectorFactory // Chooses and instantiates the method for selecting main corners
{
public:
	enum Product { BOUNDINGPRODUCT, SIZEPRODUCT };
	int product;
	SelectorFactory(Product pProduct);
	~SelectorFactory();
	unique_ptr<MainSelector> chooseSelector();
};

