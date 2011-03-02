/*
 * PlainTriples.h
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#ifndef PLAINTRIPLES_
#define PLAINTRIPLES_

#include <Triples.hpp>

namespace hdt {

class PlainTriples : public Triples {
public:
	PlainTriples();
	~PlainTriples();

	/**
	 * Returns a vector of triples matching the pattern
	 *
	 * @param triple
	 * @return
	 */
	IteratorTripleID retrieve(TripleID &triple);

	/**
	 * Adds a single triple to
	 *
	 * @param triple
	 * @return
	 */
	bool insert(TripleID &triple);

	/**
	 * Adds a collection of triples
	 *
	 * @param triples
	 * @return
	 */
	bool insert(IteratorTripleID &triples);

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
	int numberOfTriples();

	/**
	 * Saves the triples
	 *
	 * @param output
	 * @return
	 */
	bool save(std::ostream &output);

	/**
	 * Loads triples from a file
	 *
	 * @param input
	 * @return
	 */
	void load(std::istream &input);
};

}

#endif /* PLAINTRIPLES_ */
