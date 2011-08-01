/*
 * TriplesList.cpp
 *
 * Copyright (C) 2011, Javier D. Fernandez, Miguel A. Martinez-Prieto
 *                     Guillermo Rodriguez-Cano, Alejandro Andres,
 *                     Mario Arias
 * All rights reserved.
 *
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
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *   Guillermo Rodriguez-Cano:  wileeam@acm.org
 *   Alejandro Andres:          fuzzy.alej@gmail.com
 *   Mario Arias:               mario.arias@gmail.com
 *
 * @version $Id$
 *
 */

#include <HDTVocabulary.hpp>

#include "TriplesList.hpp"
#include "TriplesComparator.hpp"
#include "../util/StopWatch.hpp"

#include <algorithm>


namespace hdt {

TriplesList::TriplesList() : numValidTriples(0), order(Unknown)
{
}

TriplesList::TriplesList(HDTSpecification &specification) : numValidTriples(0), spec(specification), order(Unknown) {
}

TriplesList::~TriplesList()
{

}

IteratorTripleID *TriplesList::search(TripleID &pattern)
{
	string patternString = pattern.getPatternString();
	if(patternString=="???") {
		return new TriplesListIterator(this, pattern);
	} else {
		return new SequentialSearchIteratorTripleID(pattern, new TriplesListIterator(this,pattern));
	}
}

float TriplesList::cost(TripleID &pattern)
{
	// TODO: Theoretically define this with the team
	throw "Not implemented";
}

unsigned int TriplesList::getNumberOfElements()
{
	return numValidTriples;
}

unsigned int TriplesList::size()
{
	return numValidTriples*sizeof(TripleID);
}

bool TriplesList::save(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener)
{
	controlInformation.clear();
	controlInformation.setUint("numTriples", numValidTriples);
	controlInformation.set("codification", HDTVocabulary::TRIPLES_TYPE_TRIPLESLIST);
	controlInformation.setUint("triples.component.order", order);
	controlInformation.save(output);

	for( unsigned int i = 0; i < arrayOfTriples.size(); i++ ) {
		if ( arrayOfTriples[i].isValid() ) {
			output.write((char *)&arrayOfTriples[i], sizeof(TripleID));
		}
	}

	return true;
}

void TriplesList::load(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener)
{
	order = (TripleComponentOrder) controlInformation.getUint("triples.component.order");
	unsigned int totalTriples = controlInformation.getUint("numTriples");

	unsigned int numRead=0;
	TripleID readTriple;

	while(input.good() && numRead<totalTriples) {
		input.read((char *)&readTriple, sizeof(TripleID));
		arrayOfTriples.push_back(readTriple);
		numRead++;
		numValidTriples++;
	}
}

void TriplesList::load(ModifiableTriples &input, ProgressListener *listener)
{
	IteratorTripleID *it = input.searchAll();

	while(it->hasNext()) {
		TripleID *triple = it->next();

		this->insert(*triple);
	}

	delete it;
}

void TriplesList::populateHeader(Header &header, string rootNode)
{
	header.insert(rootNode, HDTVocabulary::TRIPLES_TYPE, HDTVocabulary::TRIPLES_TYPE_TRIPLESLIST);
	header.insert(rootNode, HDTVocabulary::TRIPLES_NUM_TRIPLES, getNumberOfElements() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_ORDER, getOrderStr(order) );
}

void TriplesList::startProcessing()
{
}

void TriplesList::stopProcessing()
{
}

string TriplesList::getType() {
	return HDTVocabulary::TRIPLES_TYPE_TRIPLESLIST;
}

// From ModifiableTriples

bool TriplesList::insert(TripleID &triple)
{
	// Add the triple
	order = Unknown;
	arrayOfTriples.push_back(triple);
	numValidTriples++;

	return true;
}

bool TriplesList::insert(IteratorTripleID *triples)
{
	while( triples->hasNext() ) {
		arrayOfTriples.push_back(*triples->next());
		numValidTriples++;
	}
	order = Unknown;

	return true;
}

bool TriplesList::remove(TripleID &pattern)
{
	bool removed=false;
	for(unsigned int i=0; i< arrayOfTriples.size(); i++) {
		TripleID *tid = &arrayOfTriples[i];
		if (tid->match(pattern)) {
			tid->clear();
			numValidTriples--;
			removed=true;
		}
	}
	return removed;
}

bool TriplesList::remove(IteratorTripleID *pattern)
{
	bool removed = false;
	vector<TripleID> allPat;

	while(pattern->hasNext()) {
		allPat.push_back(*pattern->next());
	}

	for(unsigned int i=0; i< arrayOfTriples.size(); i++) {
		TripleID *tid = &arrayOfTriples[i];
		for(int j=0; j<allPat.size(); j++) {
			if (tid->match(allPat[i])) {
				tid->clear();
				numValidTriples--;
				removed = true;
			}
		}
	}
	return removed;
}

void TriplesList::sort(TripleComponentOrder order)
{
	if(this->order != order) {
		StopWatch st;
		std::sort(arrayOfTriples.begin(), arrayOfTriples.end(), TriplesComparator(order));
		cout << "Sorted in " << st << endl;
		this->order = order;
	}
}


void TriplesList::setOrder(TripleComponentOrder order)
{
	this->order = order;


}

TripleID* TriplesList::getTripleID(unsigned int i)
{
	return &this->arrayOfTriples[i];
}

void TriplesList::removeDuplicates() {

	if(arrayOfTriples.size()<=1)
		return;

	if(order==Unknown){
		throw "Cannot remove duplicates on unordered triples";
	}

	unsigned int j = 0;
	StopWatch st;

	for(unsigned int i=1; i<arrayOfTriples.size(); i++) {
		if(arrayOfTriples[i] != arrayOfTriples[j]) {
			j++;
			arrayOfTriples[j] = arrayOfTriples[i];
		}
	}

	cout << "Removed "<< arrayOfTriples.size()-j-1 << " duplicates in " << st << endl;

	arrayOfTriples.resize(j+1);
	numValidTriples = j+1;
}


// ITERATOR

TriplesListIterator::TriplesListIterator(TriplesList *triples, TripleID & pattern) :
		triples(triples), pattern(pattern), pos(0)
{
}

bool TriplesListIterator::hasNext()
{
	return pos<triples->getNumberOfElements();
}

TripleID *TriplesListIterator::next()
{
	returnTriple = *triples->getTripleID(pos++);
	return &returnTriple;
}

bool TriplesListIterator::hasPrevious()
{
	return pos>0;
}

TripleID *TriplesListIterator::previous()
{
	returnTriple = *triples->getTripleID(--pos);
	return &returnTriple;
}

void TriplesListIterator::goToStart()
{
	pos=0;
}




}











