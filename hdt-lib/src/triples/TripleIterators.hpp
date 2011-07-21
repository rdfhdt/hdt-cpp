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
	TripleID nextTriple, previousTriple, pattern, returnTriple;
	bool hasMoreTriples, hasPreviousTriples;
	TripleComponentOrder order;
	void doFetchNext();
	void doFetchPrevious();
	virtual void getNextTriple()=0;
	virtual void getPreviousTriple()=0;

public:
	PreFetchIteratorTripleID(TripleID &pattern, TripleComponentOrder order);

	bool hasNext();
	TripleID *next();
	bool hasPrevious();
	TripleID *previous();
};

class SequentialSearchIteratorTripleID : public IteratorTripleID {
protected:
	TripleID pattern, nextTriple, previousTriple, returnTriple;
	IteratorTripleID *iterator;
	bool hasMoreTriples, hasPreviousTriples;
	bool goingUp;

	void doFetchNext();
	void doFetchPrevious();
public:
	SequentialSearchIteratorTripleID(TripleID &pattern, IteratorTripleID *other);
	~SequentialSearchIteratorTripleID();

	bool hasNext();
	TripleID *next();
	bool hasPrevious();
	TripleID *previous();
	void goToStart();
};


class RandomAccessIterator {
	IteratorTripleID *it;
	TripleID *current;
	unsigned int currentIdx;
	unsigned int numElements;
	bool goingUp;
public:
	RandomAccessIterator(IteratorTripleID *other);

	TripleID *get(unsigned int idx);

	unsigned int getNumElements();
};

}


#endif /* TRIPLEITERATORS_HPP_ */
