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
#include "../sequence/WaveletSequence.hpp"
#include "../sequence/LogSequence2.hpp"
#include "../sequence/AdjacencyList.hpp"

#include "TripleOrderConvert.hpp"

#undef size_t

namespace hdt {

class BitmapTriples : public Triples {
private:
	ControlInformation controlInformation;
	HDTSpecification spec;
	IntSequence *arrayY, *arrayZ, *arrayIndex;
	BitSequence375 *bitmapY, *bitmapZ, *bitmapIndex;
	LogSequence2 *predicateCount;
	WaveletSequence *waveletY;

	TripleComponentOrder order;

	void generateWavelet(ProgressListener *listener = NULL);

public:
	BitmapTriples();
	BitmapTriples(HDTSpecification &specification);
	virtual ~BitmapTriples();

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
	unsigned int getNumberOfElements();

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
	friend class ObjectIndexIterator;
};

class BitmapTriplesSearchIterator : public IteratorTripleID {
private:
	BitmapTriples *triples;
	TripleID pattern, returnTriple;
	unsigned int patX, patY, patZ;

	AdjacencyList adjY, adjZ;
	unsigned int posY, posZ;
	unsigned int minY, maxY, minZ, maxZ;
        unsigned int nextY, nextZ, prevY, prevZ;
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
	unsigned int estimatedNumResults();
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
	WaveletSequence *wavelet;
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
	unsigned int estimatedNumResults();
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
	unsigned int posIndex;
	unsigned int predicateOcurrence, numOcurrences;
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
	unsigned int estimatedNumResults();
	ResultEstimationType numResultEstimation();
	TripleComponentOrder getOrder();
	bool canGoTo();
	void goTo(unsigned int pos);
	bool findNextOccurrence(unsigned int value, unsigned char component);
	bool isSorted(TripleComponentRole role);
};

}



#endif /* BITMAPTRIPLES_HPP_ */
