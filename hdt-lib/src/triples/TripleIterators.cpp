/*
 * TripleIterators.cpp
 *
 *  Created on: 13/06/2011
 *      Author: mck
 */
#include "TripleIterators.hpp"

namespace hdt {


PreFetchIteratorTripleID::PreFetchIteratorTripleID(TripleID &pattern, TripleComponentOrder order)
	: pattern(pattern), order(order)
{
	//doFetch();
	swapComponentOrder(&this->pattern, SPO, this->order);
}

void PreFetchIteratorTripleID::doFetchNext() {
	if(hasMoreTriples) {
		do {
			getNextTriple();
		} while(hasMoreTriples && (!nextTriple.isValid() || !nextTriple.match(pattern)));
	}
}

void PreFetchIteratorTripleID::doFetchPrevious() {
	if(hasPreviousTriples) {
		do {
			getPreviousTriple();
		} while(hasPreviousTriples && (!previousTriple.isValid() || !previousTriple.match(pattern)));
	}
}

bool PreFetchIteratorTripleID::hasNext() {
	return hasMoreTriples;
}

TripleID *PreFetchIteratorTripleID::next() {
	returnTriple = previousTriple = nextTriple;
	swapComponentOrder(&returnTriple, order, SPO);

	doFetchNext();
	return &returnTriple;
}

bool PreFetchIteratorTripleID::hasPrevious(){
	return hasPreviousTriples;
}

TripleID *PreFetchIteratorTripleID::previous(){
	returnTriple = nextTriple = previousTriple;
	swapComponentOrder(&returnTriple, order, SPO);

	doFetchPrevious();
	return &returnTriple;
}


SequentialSearchIteratorTripleID::SequentialSearchIteratorTripleID(TripleID & pattern, IteratorTripleID *other)
	: pattern(pattern), iterator(other)
{
	hasPreviousTriples = false;
	goingUp = true;
	doFetchNext();
}

SequentialSearchIteratorTripleID::~SequentialSearchIteratorTripleID()
{
	delete iterator;
}


bool SequentialSearchIteratorTripleID::hasNext()
{
	return hasMoreTriples;
}

void SequentialSearchIteratorTripleID::doFetchNext()
{
	hasMoreTriples = false;

	cout << "Seq NEXT: " << iterator->hasPrevious() << ", " << iterator->hasNext() << endl;

	while(iterator->hasNext()){
		TripleID *next = iterator->next();

		if(next->match(pattern)) {
			hasMoreTriples = true;
			hasPreviousTriples = true;
			nextTriple = *next;
			break;
		}
	}
}

TripleID *SequentialSearchIteratorTripleID::next()
{
	if(!goingUp) {
		goingUp = true;
		if(hasPreviousTriples){
			doFetchNext();
		}
		doFetchNext();
	}
	returnTriple = nextTriple;

	doFetchNext();

	return &returnTriple;
}


bool SequentialSearchIteratorTripleID::hasPrevious()
{
	return hasPreviousTriples;
}

void SequentialSearchIteratorTripleID::doFetchPrevious()
{
	hasPreviousTriples = false;
	cout << "Seq PREV: " << iterator->hasPrevious() << ", " << iterator->hasNext() << endl;

	while(iterator->hasPrevious()){
		TripleID *previous = iterator->previous();

		if(previous->match(pattern)) {
			hasPreviousTriples = true;
			hasMoreTriples = true;
			previousTriple = *previous;
			break;
		}
	}
}

TripleID *SequentialSearchIteratorTripleID::previous()
{
	if(goingUp) {
		goingUp = false;
		if(hasMoreTriples) {
			doFetchPrevious();
		}
		doFetchPrevious();
	}
	returnTriple = previousTriple;

	doFetchPrevious();

	return &returnTriple;
}


void SequentialSearchIteratorTripleID::goToStart()
{
	iterator->goToStart();
	doFetchNext();
}



TripleID *RandomAccessIterator::get(unsigned int idx)
{
	while(currentIdx > idx && it->hasPrevious()) {
		if(goingUp) {
			goingUp = false;
			currentIdx++;
		}
		current = it->previous();
		currentIdx--;
		//cout << "PREV" << endl;
	}

	while(currentIdx < idx && it->hasNext()) {
		if(!goingUp){
			goingUp = true;
			currentIdx--;
		}
		current = it->next();
		currentIdx++;
		//cout << "NEXT" << endl;
	}

	return current;
}



unsigned int RandomAccessIterator::getNumElements()
{
	return numElements;
}



RandomAccessIterator::RandomAccessIterator(IteratorTripleID *other) :
		it(other)
{
	it->goToStart();
	numElements = 0;
	while(it->hasNext()) {
		it->next();
		numElements++;
	}
	it->goToStart();
	currentIdx = 0;
	if(it->hasNext()) {
		current = it->next();
	}
	goingUp = true;
}


}




