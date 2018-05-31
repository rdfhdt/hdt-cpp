/*
 * TriplePatternBinding.hpp
 *
 *  Created on: 24/09/2011
 *      Author: mck
 */

#ifndef TRIPLEPATTERNBINDING_HPP_
#define TRIPLEPATTERNBINDING_HPP_

#include "VarBindingInterface.hpp"

#include <Triples.hpp>
#include "../triples/TripleOrderConvert.hpp"

namespace hdt {

/** Provides a Binding by using a Triple Pattern underneath */
class TriplePatternBinding : public VarBindingInterface {
private:
	TripleID pattern;
	IteratorTripleID *iterator;
	Triples *triples;
	TripleID *currentTriple;
	vector<unsigned char> vars; // Each position means: 1 Subject, 2 Predicate, 3 Object.
	vector<string>varnames;
	//hash_map<string, char> varsByName;

public:
	TriplePatternBinding(Triples *triples, TripleID &pattern, vector<unsigned char> &vars, vector<string> &varnames);

	virtual ~TriplePatternBinding();

    size_t isOrdered(size_t numvar);

    size_t estimatedNumResults();
	ResultEstimationType estimationAccuracy();
	bool findNext();

#if 0
	// Use TriplePattern to jump to next occurence.
    bool findNext(const char *varName, size_t value=0);
#endif

	void goToStart();
    size_t getNumVars();
    size_t getVarValue(size_t numvar);
    size_t getVarValue(const char *varname);
    const char *getVarName(size_t numvar);
    void searchVar(size_t numvar, size_t value);

};

}

#endif /* TRIPLEPATTERNBINDING_HPP_ */
