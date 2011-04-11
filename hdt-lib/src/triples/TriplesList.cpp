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

#include "TriplesList.hpp"
#include "TriplesComparator.hpp"

#include <algorithm>


namespace hdt {

TriplesList::TriplesList()
{
	// TODO Auto-generated constructor stub
} // TriplesList()

TriplesList::~TriplesList()
{
	// TODO Auto-generated destructor stub
} // ~TriplesList()

// From Triples

IteratorTripleID *TriplesList::search(const TripleID &pattern)
{
	std::vector<TripleID>::iterator it = this->arrayOfTriples.begin();
	return new IteratorTripleID(it, pattern);
} // search()

float TriplesList::cost(const TripleID &pattern)
{
	// TODO: Theoretically define this with the team
	throw "Not implemented";
} // cost()

unsigned int TriplesList::getNumberOfElements()
{
	return (unsigned int) this->arrayOfTriples.size();
} // getNumberOfElements()

unsigned int TriplesList::size()
{
	return (unsigned int) this->arrayOfTriples.size()*sizeof(TripleID);
	//return (unsigned int) this->getNumberOfElements()*sizeof(TripleID);
} // size()

bool TriplesList::save(const std::ostream &output)
{
	// TODO: Revise & fix

	for( unsigned int i = 0; i < this->getNumberOfElements(); i++ ) {
		TripleID &tid = this->getTripleID(i);
		if ( !tid.isValid() ) {
			//cout << "Write: " << tid << " " << *tid << endl;
			//output.write(tid, sizeof(TripleID));
		}
	}

	return true;
}

void TriplesList::load(const std::istream &input, const Header &header)
{
	// TODO
}

void TriplesList::load(const ModifiableTriples &input)
{
	// TODO
}

void TriplesList::populateHeader(const Header &header)
{
	// TODO
}

void TriplesList::startProcessing()
{
	// TODO
}

void TriplesList::stopProcessing()
{
	// TODO
}


// From ModifiableTriples

bool TriplesList::insert(const TripleID &triple)
{
	// Saving the size of the list of triples before inserting anything
	unsigned int size = this->arrayOfTriples.size();

	// Add the triple
	arrayOfTriples.push_back(triple);

	// Verify the triple has been added (sort of assert...)
	if (this->arrayOfTriples.size() != (size+1)) {
		return false;
	}
	// TODO: Retrieve the last triple added and compare it with the parameter

	return true;
} // insert()

bool TriplesList::insert(IteratorTripleID *triples)
{
	// Supporting counter for the assert
	unsigned int counter = 0;

	// Saving the size of the list of triples before inserting anything
	unsigned int size = this->arrayOfTriples.size();

	// Add all triples
	while( triples->hasNext() ) {
		this->arrayOfTriples.push_back(triples->next());
		counter++;
	}

	// Verify the triples have been added (sort of assert...)
	if (this->arrayOfTriples.size() != (size+counter)) {
		return false;
	}

	return true;
} // insert()

bool TriplesList::remove(TripleID &pattern)
{
	bool isMatch = true;

	// Iterator at the beginning of the vector
	vector<TripleID>::iterator it = this->arrayOfTriples.begin();

	// Iterate through the vector until a match is found
	while ( isMatch = it->match(pattern) ) {
		it++;
	}

	// Check whether there has been a match or not
	if (!isMatch) {
		this->arrayOfTriples.erase(it);
		return true;
	}

	return false;
} // remove()

bool TriplesList::remove(IteratorTripleID *pattern)
{
	// TODO: Revise... this isMatch logic... I don't like it
	bool isMatch = false;

	while( pattern->hasNext() ) {
		TripleID tmp = pattern->next();
		isMatch = remove(tmp);
	}

	return isMatch;
} // remove()

bool TriplesList::edit(TripleID &oldTriple, TripleID &newTriple)
{
	// Iterator at the beginning of the vector
	vector<TripleID>::iterator it = this->arrayOfTriples.begin();


	// TODO: Check that we don't go too far, otherwise we will get out of bounds... check with end() of vector
	// Iterate through the vector until a match is found
	while ( it != this->arrayOfTriples.end() && *it != oldTriple ) {
		it++;
	}

	if ( it != this->arrayOfTriples.end() ) {
		// Replace old values with new ones
		it->replace(newTriple);
		return true;
	}

	return false;

} // edit()

void TriplesList::sort(TripleComponentOrder order)
{
	std::sort(this->arrayOfTriples.begin(), this->arrayOfTriples.end(), TriplesComparator(order));
} // sort()

void TriplesList::setOrder(TripleComponentOrder order)
{
	this->order = order;
} // setOrder()

TripleID& TriplesList::getTripleID(unsigned int i)
{
	return this->arrayOfTriples[i];
} // getTripleID()

} // hdt{}
