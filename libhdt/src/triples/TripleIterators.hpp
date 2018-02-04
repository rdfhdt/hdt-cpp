/*
 * File: TripleIterators.hpp
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

#ifndef TRIPLEITERATORS_HPP_
#define TRIPLEITERATORS_HPP_

#include <Iterator.hpp>
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
	size_t estimatedNumResults();
	bool canGoTo();
	void goTo(size_t pos);
	void skip(size_t pos);

	bool hasNext();
	TripleID *next();
	bool hasPrevious();
	TripleID *previous();
	void goToStart();
};


class RandomAccessIterator {
	IteratorTripleID *it;
	TripleID *current;
    size_t currentIdx;
    size_t numElements;
	bool goingUp;
public:
	RandomAccessIterator(IteratorTripleID *other);

    TripleID *get(size_t idx);

    size_t getNumElements();
};

} // namespace hdt

#endif /* TRIPLEITERATORS_HPP_ */
