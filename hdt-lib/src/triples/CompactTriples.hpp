/*
 * CompactTriples.hpp
 *
 *  Created on: 11/05/2011
 *      Author: mck
 */

#ifndef COMPACTTRIPLES_HPP_
#define COMPACTTRIPLES_HPP_

#include <Triples.hpp>
#include <HDTSpecification.hpp>

#include "../stream/UintStream.hpp"

namespace hdt {

class CompactTriples : public Triples  {

private:
	HDTSpecification spec;
	StreamElements *streamY, *streamZ;
	unsigned int numTriples;
	TripleComponentOrder order;

public:
	CompactTriples();
	CompactTriples(HDTSpecification &specification);
	virtual ~CompactTriples();

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

	void populateHeader(Header &header, string rootNode);

	string getType();

	friend class CompactTriplesIterator;
};

class CompactTriplesIterator : public IteratorTripleID {
private:
	CompactTriples *triples;
	TripleID nextv, pattern;
	bool hasNextv;
	unsigned int numTriple;
	unsigned int masterPos;
	unsigned int slavePos;

	unsigned int x, y, z;

	void doFetch();
	void readTriple();
public:
	CompactTriplesIterator(CompactTriples *pt, TripleID &pat);
	virtual ~CompactTriplesIterator();

	bool hasNext();

	TripleID next();
};


}


#endif /* COMPACTTRIPLES_HPP_ */
