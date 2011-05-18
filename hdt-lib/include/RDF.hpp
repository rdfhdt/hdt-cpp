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

class RDFAccess {
	/**
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
	 * @param subject
	 * @param predicate
	 * @param object
	 * @return
	 */
	virtual IteratorTripleString *search(const char *subject, const char *predicate, const char *object) = 0;

	/**
	 *
	 * @param triples
	 */
	virtual void insert(TripleString &triple) = 0;

	virtual void insert(IteratorTripleString *triple) = 0;

	/**
	 * Deletes with pattern matching
	 *
	 * @param triples
	 */
	virtual void remove(TripleString &triples) = 0;

	virtual void remove(IteratorTripleString *triples) = 0;
};


}

#endif /* RDF_HPP_ */
