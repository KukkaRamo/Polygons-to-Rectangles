#include "pch.h"
#include "SelectorFactory.h"
#include "SelectByBounding.h"
#include "SelectBySize.h"
#include <stdexcept>
#include <stdlib.h>
using namespace std;

SelectorFactory::SelectorFactory(Product pProduct): product(pProduct)
{
}

SelectorFactory::~SelectorFactory()
{
}

// Instantiate a bounding selector
MainSelector* SelectorFactory::chooseSelector() {
	if (product == BOUNDINGPRODUCT)
	return new SelectByBounding();
	else if (product == SIZEPRODUCT)
	return new SelectBySize();
	else throw runtime_error("Unknown product for the selection factory");
}
