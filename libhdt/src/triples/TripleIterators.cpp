/*
 * File: TripleIterators.cpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * Copyright (C) 2012, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
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

	//cout << "Seq NEXT: " << iterator->hasPrevious() << ", " << iterator->hasNext() << endl;
//
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

size_t SequentialSearchIteratorTripleID::estimatedNumResults()
{
	return iterator->estimatedNumResults();
}

bool SequentialSearchIteratorTripleID::canGoTo(){
	return true;
}
void SequentialSearchIteratorTripleID::goTo(size_t pos){
	iterator->goToStart();
	for (size_t i=0;i<=pos;i++){
		doFetchNext();
	}
}
void SequentialSearchIteratorTripleID::skip(size_t pos){
	for (int i=0;i<pos;i++){
		doFetchNext();
	}
}

void SequentialSearchIteratorTripleID::doFetchPrevious()
{
	hasPreviousTriples = false;
	//cout << "Seq PREV: " << iterator->hasPrevious() << ", " << iterator->hasNext() << endl;

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



TripleID *RandomAccessIterator::get(size_t idx)
{
//	cout << "RandomAccessIterator: " << currentIdx << "/" << idx << " PREV/NEXT: "<< it->hasPrevious() << ", " << it->hasNext() << endl;
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
	if(currentIdx!=idx) {
		cerr << "ERROR: " << currentIdx << "!=" << idx << " PREV/NEXT: "<< it->hasPrevious() << ", " << it->hasNext() << endl;
	}

	return current;
}



size_t RandomAccessIterator::getNumElements()
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
