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

#include "../stream/UintStream.hpp"

#include <libcdsBasics.h>
#include <BitSequenceRG.h>
#include <BitString.h>

namespace hdt {

class BitmapTriples : public Triples {
private:
	ControlInformation controlInformation;
	HDTSpecification spec;
	StreamElements *masterStream, *slaveStream;
	cds_static::BitSequence *bitmapY;
	cds_static::BitSequence *bitmapZ;
	unsigned int numTriples;
	TripleComponentOrder order;
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
	bool save(std::ostream &output, ControlInformation &controlInformation);

	/**
	 * Loads triples from a file
	 *
	 * @param input
	 * @return
	 */
	void load(std::istream &input, ControlInformation &controlInformation);

	void load(ModifiableTriples &triples);

	void populateHeader(Header &header);

	friend class BitmapTriplesIterator;
};

class BitmapTriplesIterator : public IteratorTripleID {
private:
	BitmapTriples *triples;
	TripleID nextv, pattern;
	bool hasNextv;
	unsigned int numTriple;
	unsigned int masterPos, slavePos;

	size_t nextY, nextZ;
	size_t posY, posZ;

	unsigned int x, y, z;

	void doFetch();
	void readTriple();
public:
	BitmapTriplesIterator(BitmapTriples *pt, TripleID &pat);
	virtual ~BitmapTriplesIterator();

	bool hasNext();

	TripleID next();
};

}



#endif /* BITMAPTRIPLES_HPP_ */
