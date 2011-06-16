/*
 * TripleIterators.hpp
 *
 *  Created on: 13/06/2011
 *      Author: mck
 */

#ifndef TRIPLEITERATORS_HPP_
#define TRIPLEITERATORS_HPP_

#include <SingleTriple.hpp>
#include "TripleOrderConvert.hpp"

namespace hdt {

class PreFetchIteratorTripleID : public IteratorTripleID {

protected:
	TripleID nextTriple, pattern, returnTriple;
	bool hasMoreTriples;
	TripleComponentOrder order;
	void doFetch();
	virtual void getNextTriple()=0;

public:
	PreFetchIteratorTripleID(TripleID &pattern, TripleComponentOrder order);

	bool hasNext();
	TripleID *next();
};

}

#endif /* TRIPLEITERATORS_HPP_ */
