/*
 * PlainTriples.h
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#ifndef PLAINTRIPLES_
#define PLAINTRIPLES_

#include <Triples.hpp>
#include <HDTSpecification.hpp>


#include "../stream/UintStream.hpp"
#include "../stream/LogStream.hpp"

namespace hdt {

class PlainTriples : public Triples {

private:
	ControlInformation controlInformation;
	HDTSpecification spec;
	TripleComponentOrder order;
	StreamElements *subjects, *predicates, *objects;

	TripleID getTripleID(unsigned int pos);

public:
	PlainTriples();
	PlainTriples(HDTSpecification &spec);
	~PlainTriples();

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

	friend class PlainTriplesIterator;
};


class PlainTriplesIterator : public IteratorTripleID {
private:
	PlainTriples *triples;
	TripleID nextv, pattern;
	bool hasNextv;
	unsigned int pos;

	void doFetch();
public:
	PlainTriplesIterator(PlainTriples *pt, TripleID &pat);
	virtual ~PlainTriplesIterator();


	bool hasNext();

	TripleID next();
};


class ComponentIterator : public IteratorUint {
private:
	TripleComponentRole role;
	IteratorTripleID *it;

public:
	ComponentIterator(IteratorTripleID *iterator, TripleComponentRole component);

	bool hasNext();
	unsigned int next();
};

}

#endif /* PLAINTRIPLES_ */
