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
	std::vector<TripleID *>::iterator it = this->arrayOfTriples.begin();
	return new IteratorTripleID(it, pattern);
}


} // hdt{}
