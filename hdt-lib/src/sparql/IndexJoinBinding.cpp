/*
 * IndexJoinBinding.cpp
 *
 *  Created on: 24/09/2011
 *      Author: mck
 */

#include "IndexJoinBinding.hpp"

namespace hdt {

IndexJoinBinding::IndexJoinBinding(char *var, VarBindingInterface *left, VarBindingInterface *right) :
	BaseJoinBinding(var, left, right),
	leftCount(0),
	rightCount(0)
{
    	cerr << "Index join of " << left->estimatedNumResults() << "x" << right->estimatedNumResults() << endl;
	goToStart();

}
IndexJoinBinding::~IndexJoinBinding() {
    // Left & Right Already deleted by parent.
}

unsigned int IndexJoinBinding::isOrdered(unsigned int numvar) {
	// FIXME: TEST
	return false;
}

unsigned int IndexJoinBinding::estimatedNumResults() {
            return left->estimatedNumResults()*right->estimatedNumResults();
}
ResultEstimationType IndexJoinBinding::estimationAccuracy() {
	return UNKNOWN;
}

bool IndexJoinBinding::findNext(const char *varName, unsigned int value) {
	throw "Unsupported";
}

bool IndexJoinBinding::findNext() {
	// Iterate over right using same left
	if(right->findNext(bindingVarPosRight, leftVarValue)) {
		return true;
	}

	// Right exhausted, go forward on left until there is some result on right.
	while(left->findNext()) {
		leftVarValue = left->getVarValue(bindingVarPosLeft);

		// Set the new search pattern on right
		right->searchVar(bindingVarPosRight, leftVarValue);

		// And fetch next result.
		if(right->findNext(bindingVarPosRight, leftVarValue)) {
			return true;
		}
	}
	return false;
}

//virtual void findNext(unsigned int numvar, unsigned int value=0);
void IndexJoinBinding::goToStart() {
    left->goToStart();
    right->goToStart();

    if(left->findNext()) {
	leftVarValue = left->getVarValue(bindingVarPosLeft);

	right->searchVar(bindingVarPosRight, leftVarValue);
    } else {
	// FIXME: No results??
    }
}

void IndexJoinBinding::searchVar(unsigned int numvar, unsigned int value) {
	throw "Unsupported";
}


}
