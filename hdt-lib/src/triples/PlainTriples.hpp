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

#include "TripleIterators.hpp"


#include "../stream/UintStream.hpp"
#include "../stream/LogStream.hpp"

namespace hdt {

class PlainTriples : public Triples {

private:
	ControlInformation controlInformation;
	HDTSpecification spec;
	TripleComponentOrder order;
	StreamElements *streamX, *streamY, *streamZ;

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
	void save(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener = NULL);

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

	friend class PlainTriplesIterator;
};

class PlainTriplesIterator : public IteratorTripleID {
private:
	TripleID pattern, returnTriple;
	TripleComponentOrder order;
	PlainTriples *triples;
	int64_t pos;

	void updateOutput();
public:
	PlainTriplesIterator(PlainTriples *triples, TripleID &pattern, TripleComponentOrder order);
	bool hasNext();
	TripleID *next();
	bool hasPrevious();
	TripleID *previous();
	void goToStart();
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
