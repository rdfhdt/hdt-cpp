/*
 * Header.hpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#ifndef HEADER_
#define HEADER_

#include <RDF.hpp>
#include <iostream>

namespace hdt {

/**
 * Describes the Header of any HDT.
 */
class Header : public RDFStorage {

public:
	virtual ~Header() { }

	virtual bool save(std::ostream &output)=0;
	virtual void load(std::istream &input)=0;

	/**
	 * Search a Triple Pattern.
	 *
	 * Searches all triples that match the supplied Triple Pattern. The pattern is composed by
	 * a subject, a predicate and and object, where NULL or empty string "" mean match any.
	 *
	 * @param subject
	 * @param predicate
	 * @param object
	 * @return
	 */
	virtual IteratorTripleString *search(const char *subject, const char *predicate, const char *object) = 0;

	/**
	 * Insert a triple to the collection.
	 *
	 * @param triples New triple to be added.
	 */
	virtual void insert(TripleString &triple) = 0;

	/**
	 * Insert a set of triples, as specified in the iterator.
	 *
	 * @param triples Iterator of Triples
	 */
	virtual void insert(IteratorTripleString *triples) = 0;

	/**
	 * Removes from the collection all triples that match the supplied Triple pattern.
	 *
	 * @param triples
	 */
	virtual void remove(TripleString &triples) = 0;

	/**
	 * Remove a set of patterns.
	 * Removes all triples that match the specified triple patterns.
	 *
	 * @param triples Iterator of TripleString indicating the triple patterns to be removed.
	 */
	virtual void remove(IteratorTripleString *triples) = 0;
};

}

#endif /* HEADER_HPP_ */
