/*
 * BasicHDT.hpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#ifndef BASICHDT_HPP_
#define BASICHDT_HPP_

#include <HDTSpecification.hpp>
#include <HDT.hpp>

namespace hdt {

class BasicHDT : public ModifiableHDT {
private:
	Header *header;
	Dictionary *dictionary;
	ModifiableTriples *triples;
	HDTSpecification spec;

	void createComponents();

public:
	BasicHDT();

	BasicHDT(HDTSpecification &spec);

	virtual ~BasicHDT();

	/**
	 *
	 */
	Header &getHeader();

	/**
	 *
	 */
	Dictionary &getDictionary();

	/**
	 *
	 */
	Triples &getTriples();

	/*
	 * @param input
	 * @param specification
	 */
	void loadFromRDF(std::istream &input, RDFNotation notation);

	/**
	 * @param input
	 */
	void loadFromHDT(std::istream &input);

	/**
	 * @param output
	 * @param notation
	 */
	void saveToRDF(std::ostream &output, RDFNotation notation);

	/**
	 * @param output
	 */
	void saveToHDT(std::ostream &output);


	/*
	 * FROM RDFAccess
	 */

	/**
	 * @param subject
	 * @param predicate
	 * @param object
	 * @return
	 */
	virtual IteratorTripleString *search(const char *subject, const char *predicate, const char *object);

	/**
	 *
	 * @param triples
	 */
	virtual void insert(TripleString &triple);

	virtual void insert(IteratorTripleString *triple);

	/**
	 * Deletes with pattern matching
	 *
	 * @param triples
	 */
	virtual void remove(TripleString &triples);

	virtual void remove(IteratorTripleString *triples);

	/**
	 * Updates a triple with new components
	 *
	 * @param oldTriple
	 *            The triple to be replaced
	 * @param newTriple
	 *            The triple to replace the old one
	 * @return boolean
	 */
	virtual bool edit(TripleString &oldTriple, TripleString &newTriple);

};

class BasicIteratorTripleString : public IteratorTripleString {

private:
	Dictionary *dict;
	IteratorTripleID *iterator;
public:
	BasicIteratorTripleString(Dictionary *dict, IteratorTripleID *iterator) {
		this->dict = dict;
		this->iterator = iterator;
	}

	bool hasNext() {
		return iterator->hasNext();
	}

	TripleString next() {
		TripleID tid = iterator->next();
		return dict->tripleIDtoTripleString(tid);
	}
};

}

#endif /* BASICHDT_HPP_ */
