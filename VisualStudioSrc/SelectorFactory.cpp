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
unique_ptr<MainSelector> SelectorFactory::chooseSelector() {
	if (product == BOUNDINGPRODUCT)
	return unique_ptr<MainSelector>(new SelectByBounding());
	else if (product == SIZEPRODUCT)
	return unique_ptr<MainSelector>(new SelectBySize());
	else throw runtime_error("Unknown product for the selection factory");
}
