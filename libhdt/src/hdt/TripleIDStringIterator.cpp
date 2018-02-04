/*
 * TripleIDStringIterator.cpp
 *
 *  Created on: 11/08/2012
 *      Author: mck
 */


#include "TripleIDStringIterator.hpp"

namespace hdt {

TripleIDStringIterator::TripleIDStringIterator(Dictionary *dict, IteratorTripleID *iterator) : dict(dict), iterator(iterator){

}

TripleIDStringIterator::~TripleIDStringIterator() {
	delete iterator;
}

bool TripleIDStringIterator::hasNext() {
	return iterator->hasNext();
}

TripleString *TripleIDStringIterator::next() {
	TripleID *tid = iterator->next();
	dict->tripleIDtoTripleString(*tid, result);
	return &result;
}

bool TripleIDStringIterator::hasPrevious() {
	return iterator->hasPrevious();
}

TripleString *TripleIDStringIterator::previous() {
	TripleID *tid = iterator->previous();
	dict->tripleIDtoTripleString(*tid, result);
	return &result;
}

void TripleIDStringIterator::goToStart() {
	iterator->goToStart();
}
bool TripleIDStringIterator::canGoTo() {
	return iterator->canGoTo();
}
size_t TripleIDStringIterator::estimatedNumResults() {
	return iterator->estimatedNumResults();
}
ResultEstimationType TripleIDStringIterator::numResultEstimation() {
	return iterator->numResultEstimation();
}
void TripleIDStringIterator::skip(size_t pos) {
	iterator->skip(pos);
}

}

