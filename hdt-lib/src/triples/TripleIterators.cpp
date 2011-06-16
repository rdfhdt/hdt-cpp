/*
 * TripleIterators.cpp
 *
 *  Created on: 13/06/2011
 *      Author: mck
 */
#include "TripleIterators.hpp"

namespace hdt {


void PreFetchIteratorTripleID::doFetch() {
	do {
		getNextTriple();
	} while(hasMoreTriples && (!nextTriple.isValid() || !nextTriple.match(pattern)));
}


PreFetchIteratorTripleID::PreFetchIteratorTripleID(TripleID &pattern, TripleComponentOrder order) : pattern(pattern), order(order) {
	//doFetch();
	swapComponentOrder(&this->pattern, SPO, this->order);
}

bool PreFetchIteratorTripleID::hasNext() {
	return hasMoreTriples;
}

TripleID *PreFetchIteratorTripleID::next() {
	returnTriple = nextTriple;
	swapComponentOrder(&returnTriple, order, SPO);

	doFetch();
	return &returnTriple;
}







}
