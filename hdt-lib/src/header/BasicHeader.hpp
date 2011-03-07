/*
 * BasicHeader.h
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifndef BASICHEADER_H_
#define BASICHEADER_H_

#include <HDT.hpp>
#include <Header.hpp>

namespace hdt {

class BasicHeader : public Header {
private:
	HDTSpecification spec;
	HDT *hdt;	// Delegates on HDT for operations.

public:
	BasicHeader();
	virtual ~BasicHeader();
	bool save(std::ostream &output);
	void load(std::istream &input);

	/*
	 * FROM RDFAccess
	 */

	/**
	 * @param subject
	 * @param predicate
	 * @param object
	 * @return
	 */
	IteratorTripleString *search(const char *subject, const char *predicate, const char *object);

	/**
	 *
	 * @param triples
	 */
	void insert(TripleString &triple);

	void insert(IteratorTripleString *triple);

	/**
	 * Deletes with pattern matching
	 *
	 * @param triples
	 */
	void remove(TripleString &triples);

	void remove(IteratorTripleString *triples);

	/**
	 * Updates a triple with new components
	 *
	 * @param oldTriple
	 *            The triple to be replaced
	 * @param newTriple
	 *            The triple to replace the old one
	 * @return boolean
	 */
	bool edit(TripleString &oldTriple, TripleString &newTriple);
};

}

#endif /* BASICHEADER_H_ */
