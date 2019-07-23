#pragma once
#include "MainSelector.h"
class SelectorFactory // Chooses and instantiates the method for selecting main corners
{
public:
	enum Product { BOUNDINGPRODUCT, SIZEPRODUCT };
	int product;
	SelectorFactory(Product pProduct);
	~SelectorFactory();
	MainSelector* chooseSelector();
};

