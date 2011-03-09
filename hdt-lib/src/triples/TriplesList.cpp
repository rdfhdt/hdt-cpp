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
}

bool TriplesList::insert(const IteratorTripleID *triples)
{
	return false;
}

} // hdt{}
