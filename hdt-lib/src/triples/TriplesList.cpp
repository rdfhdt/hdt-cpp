/*
 * TriplesList.cpp
 *
 *  Created on: 04/03/2011
 *      Author: billy
 */

#include "TriplesList.hpp"

namespace hdt {

TriplesList::TriplesList() {
	// TODO Auto-generated constructor stub

}

TriplesList::~TriplesList() {
	// TODO Auto-generated destructor stub
}

// From Triples

IteratorTripleID *TriplesList::search(const TripleID &pattern)
{
	std::vector<TripleID>::iterator it = this->arrayOfTriples.begin();
	return new IteratorTripleID(it, pattern);
} // search()

float TriplesList::cost(const TripleID &pattern)
{
	// TODO: Theoretically define this with the team
	return -1;
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
	// TODO
	return false;
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

} // hdt{}
