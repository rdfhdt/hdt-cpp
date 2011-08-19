/*
 * BitmapTriples.hpp
 *
 *  Created on: 12/05/2011
 *      Author: mck
 */

#ifndef BITMAPTRIPLES_HPP_
#define BITMAPTRIPLES_HPP_

#include <Triples.hpp>
#include <HDTSpecification.hpp>

#include <libcdsBasics.h>
#include <BitSequenceRG.h>
#include <BitString.h>

#include "../stream/WaveletStream.hpp"
#include "../stream/AdjacencyList.hpp"
#include "TripleOrderConvert.hpp"

namespace hdt {

class BitmapTriples : public Triples {
private:
	ControlInformation controlInformation;
	HDTSpecification spec;
	StreamElements *streamY, *streamZ, *streamIndex;
	cds_static::BitSequence *bitmapY, *bitmapZ, *bitmapIndex;
	WaveletStream *waveletY;

	unsigned int numTriples;
	TripleComponentOrder order;

	void generateIndex(ProgressListener *listener);
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

	unsigned int size();

	/**
	 * Saves the triples
	 *
	 * @param output
	 * @return
	 */
	bool save(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener = NULL);

	/**
	 * Loads triples from a file
	 *
	 * @param input
	 * @return
	 */
	void load(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener = NULL);

	void load(ModifiableTriples &triples, ProgressListener *listener = NULL);

	void populateHeader(Header &header, string rootNode);

	string getType();

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
};

class MiddleWaveletIterator : public IteratorTripleID {
private:
	BitmapTriples *triples;
	TripleID pattern, returnTriple;

	AdjacencyList adjY, adjZ;
	WaveletStream *wavelet;
	unsigned int patX, patY, patZ;
	unsigned int posY, posZ;
	unsigned int predicateOcurrence, numOcurrences;
	unsigned int prevZ, nextZ;
	unsigned int x, y, z;

	void updateOutput();
public:
	MiddleWaveletIterator(BitmapTriples *triples, TripleID &pat);

	bool hasNext();
	TripleID *next();
	bool hasPrevious();
	TripleID *previous();
	void goToStart();
};

class ObjectIndexIterator : public IteratorTripleID {
private:
	BitmapTriples *triples;
	TripleID pattern, returnTriple;

	AdjacencyList adjY, adjZ, adjIndex;
	unsigned int patX, patY, patZ;
	unsigned int posIndex;
	unsigned int predicateOcurrence, numOcurrences;
	unsigned int minIndex, maxIndex;
	unsigned int x, y, z;

	void updateOutput();
public:
	ObjectIndexIterator(BitmapTriples *triples, TripleID &pat);

	bool hasNext();
	TripleID *next();
	bool hasPrevious();
	TripleID *previous();
	void goToStart();
};

}



#endif /* BITMAPTRIPLES_HPP_ */
