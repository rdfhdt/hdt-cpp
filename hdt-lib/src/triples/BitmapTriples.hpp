/*
 * File: BitmapTriples.hpp
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

#ifndef BITMAPTRIPLES_HPP_
#define BITMAPTRIPLES_HPP_

#include <Triples.hpp>
#include <HDTSpecification.hpp>

#include "../bitsequence/BitSequence375.h"
#include "../sequence/LogSequence2.hpp"
#include "../sequence/AdjacencyList.hpp"

#include "predicateindex.hpp"

#include "TripleOrderConvert.hpp"

namespace hdt {

class PredicateIndex;

class BitmapTriples : public Triples {
private:
	ControlInformation controlInformation;
	HDTSpecification spec;
	IntSequence *arrayY, *arrayZ, *arrayIndex;
	BitSequence375 *bitmapY, *bitmapZ, *bitmapIndex;
	IntSequence *predicateCount;
    PredicateIndex *predicateIndex;

	TripleComponentOrder order;
public:
	BitmapTriples();
	BitmapTriples(HDTSpecification &specification);
	virtual ~BitmapTriples();

    bool isIndexed() {
        return arrayIndex && bitmapIndex && predicateIndex;
    }

    size_t getNumAppearances(size_t pred) {
        if(predicateCount) {
            return predicateCount->get(pred-1);
        }
        return 0;
    }

	/**
	 * Returns a vector of triples matching the pattern
	 *
	 * @param triple
	 * @return
	 */
	IteratorTripleID *search(TripleID &triple);

	/**
	 * Calculates the cost to retrieve a specific pattern
	 *
	 * @param triple
	 * @return
	 */
	float cost(TripleID &triple);

	/**
	 * Returns the number of triples
	 *
	 * @return
	 */
    size_t getNumberOfElements();

    size_t size();

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
	void generateIndexFast(ProgressListener *listener);
	void generateIndexMemory(ProgressListener *listener);
	void saveIndex(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener);
	void loadIndex(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener);

    size_t loadIndex(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener);

	void populateHeader(Header &header, string rootNode);

	string getType();
	TripleComponentOrder getOrder();

	friend class BitmapTriplesSearchIterator;
	friend class MiddleWaveletIterator;
	friend class IteratorY;
	friend class ObjectIndexIterator;
    friend class PredicateIndexWavelet;
    friend class PredicateIndexArray;
    friend class BTInterleavedIterator;
};

class BitmapTriplesSearchIterator : public IteratorTripleID {
private:
	BitmapTriples *triples;
	TripleID pattern, returnTriple;
	unsigned int patX, patY, patZ;

	AdjacencyList adjY, adjZ;
    size_t posY, posZ;
    size_t minY, maxY, minZ, maxZ;
    size_t nextY, nextZ, prevY, prevZ;
	unsigned int x, y, z;

	void findRange();
	void getNextTriple();
	void getPreviousTriple();

	void updateOutput();
public:
	BitmapTriplesSearchIterator(BitmapTriples *triples, TripleID &pat);

	bool hasNext();
	TripleID *next();
	bool hasPrevious();
	TripleID *previous();
	void goToStart();
    size_t estimatedNumResults();
	ResultEstimationType numResultEstimation();
	TripleComponentOrder getOrder();
	bool canGoTo();
	void goTo(unsigned int pos);
	bool findNextOccurrence(unsigned int value, unsigned char component);
	bool isSorted(TripleComponentRole role);
};

class MiddleWaveletIterator : public IteratorTripleID {
private:
	BitmapTriples *triples;
	TripleID pattern, returnTriple;

	AdjacencyList adjY, adjZ;
    PredicateIndex *predicateIndex;
	unsigned int patX, patY, patZ;
	unsigned int posY, posZ;
	unsigned int predicateOcurrence, numOcurrences;
	unsigned int prevZ, nextZ;
	unsigned int maxZ;
	unsigned int x, y, z;

	void updateOutput();
public:
	MiddleWaveletIterator(BitmapTriples *triples, TripleID &pat);

	bool hasNext();
	TripleID *next();
	bool hasPrevious();
	TripleID *previous();
	void goToStart();
    size_t estimatedNumResults();
	ResultEstimationType numResultEstimation();
	TripleComponentOrder getOrder();
	bool findNextOccurrence(unsigned int value, unsigned char component);
	bool isSorted(TripleComponentRole role);
};

class IteratorY : public IteratorTripleID {
private:
	BitmapTriples *triples;
	TripleID pattern, returnTriple;

	AdjacencyList adjY, adjZ;
	size_t patX, patY, patZ;
	size_t posY, posZ;
	size_t prevY, prevZ;
	size_t nextY, nextZ;

	size_t x, y, z;

	void updateOutput();
public:
	IteratorY(BitmapTriples *triples, TripleID &pat);

	bool hasNext();
	TripleID *next();
	bool hasPrevious();
	TripleID *previous();
	void goToStart();
    size_t estimatedNumResults();
	ResultEstimationType numResultEstimation();
	TripleComponentOrder getOrder();
	bool findNextOccurrence(unsigned int value, unsigned char component);
	bool isSorted(TripleComponentRole role);
};

class ObjectIndexIterator : public IteratorTripleID {
private:
	BitmapTriples *triples;
	TripleID pattern, returnTriple;

	AdjacencyList adjY, adjZ, adjIndex;
	unsigned int patX, patY, patZ;
    size_t posIndex;
    //size_t predicateOcurrence, numOcurrences;
	long long minIndex, maxIndex;
	unsigned int x, y, z;

	void updateOutput();
	void calculateRange();
	unsigned int getPosZ(unsigned int index);
	unsigned int getY(unsigned int index);
public:
	ObjectIndexIterator(BitmapTriples *triples, TripleID &pat);

	bool hasNext();
	TripleID *next();
	bool hasPrevious();
	TripleID *previous();
	void goToStart();
    size_t estimatedNumResults();
	ResultEstimationType numResultEstimation();
	TripleComponentOrder getOrder();
	bool canGoTo();
	void goTo(unsigned int pos);
	bool findNextOccurrence(unsigned int value, unsigned char component);
	bool isSorted(TripleComponentRole role);
};


class BTInterleavedIterator : public IteratorTripleID {
private:
    BitmapTriples *triples;
    TripleID returnTriple;

    AdjacencyList adjY, adjZ;
    size_t posZ;
    size_t skip;

    void updateOutput();
public:
    BTInterleavedIterator(BitmapTriples *triples, size_t skip);

    bool hasNext();
    TripleID *next();
};




}



#endif /* BITMAPTRIPLES_HPP_ */
