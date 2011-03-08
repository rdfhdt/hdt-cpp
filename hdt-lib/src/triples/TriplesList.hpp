/*
 * TriplesList.h
 *
 *  Created on: 04/03/2011
 *      Author: billy
 */

#ifndef TRIPLESLIST_H_
#define TRIPLESLIST_H_

#include <vector>
#include <Triples.hpp>

namespace hdt {

class TriplesList : public ModifiableTriples {
private:
	vector<TripleID> arrayOfTriples;

public:
	TriplesList();
	virtual ~TriplesList();

	// From Triples

	/**
	 * Returns a vector of triples matching the pattern
	 *
	 * @param pattern
	 * @return
	 */
	IteratorTripleID *search(TripleID &pattern);

};

}

#endif /* TRIPLESLIST_H_ */
