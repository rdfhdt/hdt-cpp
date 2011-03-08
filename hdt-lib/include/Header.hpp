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

class Header : public RDFAccess {

public:
	virtual ~Header() { }

	virtual bool save(std::ostream &output)=0;
	virtual void load(std::istream &input)=0;

	/*
	 * FROM RDFAccess
	 */

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

	/**
	 * Updates a triple with new components
	 *
	 * @param oldTriple
	 *            The triple to be replaced
	 * @param newTriple
	 *            The triple to replace the old one
	 * @return boolean
	 */
	virtual bool edit(TripleString &oldTriple, TripleString &newTriple) = 0;
};

}

#endif /* HEADER_HPP_ */
