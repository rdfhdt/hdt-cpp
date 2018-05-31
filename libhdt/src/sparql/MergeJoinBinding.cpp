/*
 * MergeJoinBinding.cpp
 *
 *  Created on: 24/09/2011
 *      Author: mck
 */
#include <stdexcept>
#include "MergeJoinBinding.hpp"


namespace hdt {

MergeJoinBinding::MergeJoinBinding(char *var, VarBindingInterface *left, VarBindingInterface *right) :
    BaseJoinBinding(var, left, right),
    leftCount(0),
    rightCount(0)
{
    goToStart();

    if(!left->isOrdered(left->getVarIndex(var))||!right->isOrdered(right->getVarIndex(var))) {
	throw std::runtime_error("Cannot merge join if the variables are not sorted!");
    }

    cerr << "Merge join of " << left->estimatedNumResults() << " against " << right->estimatedNumResults() << endl;
}
MergeJoinBinding::~MergeJoinBinding() {

}

size_t MergeJoinBinding::isOrdered(size_t numvar) {
    if(numvar==0) {
	// Join var is always sorted
	return true;
    }
    // The rest are not necessarily sorted. Check?
    return false;
}

size_t MergeJoinBinding::estimatedNumResults() {
    return left->estimatedNumResults()*right->estimatedNumResults();
}
ResultEstimationType MergeJoinBinding::estimationAccuracy() {
    return UNKNOWN;
}

bool MergeJoinBinding::findNext(const char *varName, size_t value) {
    throw std::logic_error("Unsupported");
}

bool MergeJoinBinding::findNext() {
    // When we have a list of operands, just move forward.
    if(leftCount<leftOperands.size() || rightCount<rightOperands.size() ) {
	rightCount++;

	if(rightCount==rightOperands.size()) {
	    rightCount = 0;
	    leftCount++;
	}
	if(leftCount<leftOperands.size()) {
	    return true;
	}
    }

    // End condition.
    if(!hasMoreOperands) {
	return false;
    }

    // Check which one is lower and find it in the other.
    //cout << "Comparing s and r:    s=" << s << " r=" << r << endl;
    if(s<r) {
	if(left->findNext(bindingVarPosLeft, r)) {
	    //cout << "Forward left" << endl;
	    s = left->getVarValue(bindingVarPosLeft);
	} else {
	    //cout << "No more results left" << endl;
	    hasMoreOperands = false;
	    return false;
	}
    } else if(s>r) {
	if(right->findNext(bindingVarPosLeft, s)) {
	    //cout << "Forward right" << endl;
	    r = right->getVarValue(bindingVarPosLeft);
	} else {
	    //cout << "No more results right" << endl;
	    hasMoreOperands = false;
	    return false;
	}
    }

    // We found the same.
    if(s==r) {
	//cout << "Same value left and right: " << s << endl;
    size_t currentr = r;
    size_t currents = s;

	// Save all entries of the left.
	leftOperands.clear();
    size_t count  = 0;
	while( (s=left->getVarValue(bindingVarPosLeft))==currentr) {
	    leftOperands.resize(count+1);
	    leftOperands[count].resize(left->getNumVars());
        for(size_t i=0;i<left->getNumVars();i++) {
		leftOperands[count][i] = left->getVarValue(i);
	    }
	    count++;
	    if(!left->findNext()) {
		hasMoreOperands = false;
		break;
	    }
	}
	//cout << "Left has: " << count << endl;

	// Save all entries of the right.
	rightOperands.clear();
	count = 0;
	while( (r = right->getVarValue(bindingVarPosRight))==currents) {
	    rightOperands.resize(count+1);
	    rightOperands[count].resize(right->getNumVars());
        for(size_t i=0;i<right->getNumVars();i++) {
		rightOperands[count][i] = right->getVarValue(i);
	    }
	    count++;
	    if(!right->findNext()) {
		hasMoreOperands = false;
		break;
	    }
	}
	//cout << "Right has: " << count << endl;

	// Now iterate over the cartesian product of left and right.
	leftCount = rightCount = 0;
	return true;
    }

    // IF any of the previous, we are done.
    //cout << "Anything to do" << endl;
    return false;
}

//virtual void findNext(size_t numvar, size_t value=0);
void MergeJoinBinding::goToStart() {
    left->goToStart();
    right->goToStart();

    hasMoreOperands = true;
    leftOperands.clear();
    rightOperands.clear();

    if(left->findNext()) {
	s = left->getVarValue(bindingVarPosLeft);
    } else {
	hasMoreOperands = false;
    }
    if(right->findNext()) {
	r = right->getVarValue(bindingVarPosRight);
    } else {
	hasMoreOperands = false;
    }
}

size_t MergeJoinBinding::getVarValue(size_t numvar) {
    if(numvar>=getNumVars()) {
	throw std::out_of_range("Accessing out of bound variable");
    }

    if(!varOperand[numvar]) {
	// Left operand
	//cout << "Left count: " << leftCount << "/" << leftOperands.size() <<endl;
	//cout << "\tNumvar " << numvar << " Left at " << vars[numvar] << endl;
	//cout << "\tList size: " << leftOperands[leftCount].size() << endl;
	return leftOperands[leftCount][vars[numvar]];
    } else {
	// Right operand
	//cout << "Right count: " << rightCount << "/" << rightOperands.size() << endl;
	//cout << "\tNumvar " << numvar << " Right at " << vars[numvar] << endl;
	//cout << "\tList size: " << rightOperands[rightCount].size() << endl;
	return rightOperands[rightCount][vars[numvar]];
    }
}

void MergeJoinBinding::searchVar(size_t numvar, size_t value) {
    throw std::logic_error("Unsupported");
}

}
