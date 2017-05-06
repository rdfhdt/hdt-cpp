/*
 * TriplePatternBinding.cpp
 *
 *  Created on: 24/09/2011
 *      Author: mck
 */
#include <stdexcept>
#include "TriplePatternBinding.hpp"

namespace hdt {

TriplePatternBinding::TriplePatternBinding(Triples *triples, TripleID &pattern, vector<unsigned char> &vars, vector<string> &varnames)
: pattern(pattern),
  triples(triples),
  currentTriple(NULL),
  vars(vars),
  varnames(varnames)
{
	iterator = triples->search(this->pattern);

#if 0
    for(size_t i=0;i<varnames.size();i++) {
		cout << "Indexing " << varnames[i] << " to " << i << endl;
		varsByName[varnames[i]] = i;
	}
#endif
	goToStart();
}

TriplePatternBinding::~TriplePatternBinding() {
	delete iterator;
}

size_t TriplePatternBinding::isOrdered(size_t numvar) {
    //FIXME: CHECK depending on the order
    switch(vars[numvar]) {
    case 1:
	return iterator->isSorted(SUBJECT);
    case 2:
	return iterator->isSorted(PREDICATE);
    case 3:
	return iterator->isSorted(OBJECT);
    }
    throw std::runtime_error("Wrong numvar");
}

size_t TriplePatternBinding::estimatedNumResults() {
	return iterator->estimatedNumResults();
}

ResultEstimationType TriplePatternBinding::estimationAccuracy() {
	return iterator->numResultEstimation();
}

bool TriplePatternBinding::findNext() {
	//cout << "findNext on " << pattern << endl;
	if(iterator->hasNext()) {
		currentTriple = iterator->next();
		//cout << "TripleID: " << *triple << endl;
		return true;
	}
	return false;
}

#if 0
// Use TriplePattern to jump to next occurence.
bool TriplePatternBinding::findNext(const char *varName, size_t value=0) {
    size_t varIdx = getVarIndex(varName);

	if(iterator->findNextOccurrence(value, vars[varIdx])) {
		currentTriple = iterator->next();

		if(getVar(varName)!=value) {
			throw std::runtime_error("Found value is not what we were searching!");
		}
		return true;
	}

	return false;
}
#endif

void TriplePatternBinding::goToStart() {
	iterator->goToStart();
}

size_t TriplePatternBinding::getNumVars() {
	return vars.size();
}
size_t TriplePatternBinding::getVarValue(size_t numvar) {
	if(numvar>vars.size()){
		throw std::runtime_error("Variable not available");
	}
	switch(vars[numvar]) {
	case 1:
		return currentTriple->getSubject();
	case 2:
		return currentTriple->getPredicate();
	case 3:
		return currentTriple->getObject();
	default:
		throw std::runtime_error("Wrong numvar");
	}
}
size_t TriplePatternBinding::getVarValue(const char *varname) {
	return getVarValue(getVarIndex(varname));
}

const char *TriplePatternBinding::getVarName(size_t numvar) {
	if(numvar>vars.size()){
		throw std::runtime_error("Variable not available");
	}
	return varnames[numvar].c_str();
}

void TriplePatternBinding::searchVar(size_t numvar, size_t value) {
	TripleID newPattern = pattern;

	// Search on index.
	if(vars[numvar]==1) {
		if(newPattern.getSubject()==0) {
			newPattern.setSubject(value);
		}
	} else if(vars[numvar]==2) {
		if(newPattern.getPredicate()==0) {
			newPattern.setPredicate(value);
		}
	} else if(vars[numvar]==3) {
		if(newPattern.getObject()==0) {
			newPattern.setObject(value);
		}
	}

	delete iterator;
	iterator = triples->search(newPattern);
}

}
