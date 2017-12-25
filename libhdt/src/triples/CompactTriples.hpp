/*
 * File: CompactTriples.hpp
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

#ifndef COMPACTTRIPLES_HPP_
#define COMPACTTRIPLES_HPP_

#include <Triples.hpp>
#include <HDTSpecification.hpp>

#include "TripleIterators.hpp"

#include "../sequence/LogSequence2.hpp"

namespace hdt {

class CompactTriples : public Triples  {

private:
	HDTSpecification spec;
	IntSequence *streamY, *streamZ;
	unsigned int numTriples;
	TripleComponentOrder order;

public:
	CompactTriples();
	CompactTriples(HDTSpecification &specification);
	virtual ~CompactTriples();

    bool isIndexed() const {
        return false;
    }

	/**
	 * Returns a vector of triples matching the pattern
	 *
	 * @param triple
	 * @return
	 */
	IteratorTripleID *search(TripleID &triple);

	IteratorTripleID *searchJoin(TripleID &a, TripleID &b, unsigned short conditions);

	/**
	 * Calculates the cost to retrieve a specific pattern
	 *
	 * @param triple
	 * @return
	 */
	float cost(TripleID &triple) const;

	/**
	 * Returns the number of triples
	 *
	 * @return
	 */
    size_t getNumberOfElements() const;

    size_t size() const;

	/**
	 * Saves the triples
	 *
	 * @param output
	 * @return
	 */
	void save(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener = NULL);

	/**
	 * Loads triples from a file
	 *
	 * @param input
	 * @return
	 */
	void load(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener = NULL);

	size_t load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener=NULL);

	void load(ModifiableTriples &triples, ProgressListener *listener = NULL);

	void generateIndex(ProgressListener *listener);

	void saveIndex(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener);
	void loadIndex(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener);

    size_t loadIndex(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener);

	void populateHeader(Header &header, string rootNode);

	string getType() const;

	TripleComponentOrder getOrder() const;

	friend class CompactTriplesIterator;
};

class CompactTriplesIterator : public IteratorTripleID {
private:
	CompactTriples *triples;
	TripleID pattern, returnTriple;

	unsigned int patX, patY, patZ;

	unsigned int posY, posZ;
	unsigned int x, y, z;
	bool goingUp;

	void findRange();
	void getNextTriple();
	void getPreviousTriple();

	void updateOutput();
public:
	CompactTriplesIterator(CompactTriples *triples, TripleID &pat);

	bool hasNext();
	TripleID *next();
	bool hasPrevious();
	TripleID *previous();
	void goToStart();
};

} // namespace hdt

#endif /* COMPACTTRIPLES_HPP_ */
