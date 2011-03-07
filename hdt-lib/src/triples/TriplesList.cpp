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

IteratorTripleID TripleListDisk::search(TripleID &pattern)
{
	IteratorTripleID &res;
	vector<TripleID>::iterator it;


	for (it = arrayOfTriples.begin() ; it < arrayOfTriples.end(); it++) {

	}

	return res;
}

} // hdt{}
