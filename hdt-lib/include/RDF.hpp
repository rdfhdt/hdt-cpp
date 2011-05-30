/*
 * RDF.hpp
 *
 *  Created on: 06/03/2011
 *      Author: mck
 */

#ifndef RDF_HPP_
#define RDF_HPP_

#include <SingleTriple.hpp>

namespace hdt {

/**
 * Interface that provides read/search access to a set of triples.
 */

class RDFAccess {
	/**
	 * Returns an iterator over all results matching the provided triple pattern.
	 * @param subject
	 * @param predicate
	 * @param object
	 * @return
	 */
	virtual IteratorTripleString *search(const char *subject, const char *predicate, const char *object) = 0;
};

/**
 * Provides writable access to a RDF repository, search triples, insert, remove.
 */
class RDFStorage : public RDFAccess {

public:

	virtual ~RDFStorage() { }

	/**
	 * Insert a new triple to the store.
	 * @param triple
	 */
	virtual void insert(TripleString &triple) = 0;

	/**
	 * Insert a set of triples to the store, as specified in the iterator.
	 * @param triple
	 */
	virtual void insert(IteratorTripleString *triple) = 0;

	/**
	 * Removes the set of triples that match the specified pattern.
	 *
	 * @param triples
	 */
	virtual void remove(TripleString &triple) = 0;

	/**
	 * Removes the set of triples that match any of the specified patterns.
	 * @param triples
	 */
	virtual void remove(IteratorTripleString *triples) = 0;
};

}

#endif /* RDF_HPP_ */
