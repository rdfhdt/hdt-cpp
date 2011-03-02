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

class BasicHDT : public HDT {
private:
	Header *header;
	Dictionary *dictionary;
	Triples *triples;
	HDTSpecification spec;

	void createComponents();

public:
	BasicHDT();

	BasicHDT(HDTSpecification &spec);

	~BasicHDT();

	/*
	 * @param input
	 * @param specification
	 */
	void loadFromRDF(std::istream &input);

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

	/**
	 * @param subject
	 * @param predicate
	 * @param object
	 * @return
	 */
	IteratorTripleString search(const char *subject, const char *predicate, const char *object);

	/**
	 *
	 * @param triples
	 */
	void insert(TripleString &triple);

	void insert(IteratorTripleString &triple);

	/**
	 * Deletes with pattern matching
	 *
	 * @param triples
	 */
	void remove(TripleString &triples);

	void remove(IteratorTripleString &triples);

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
		return dict->tripleIDtoTripleString(iterator->next());
	}
};

}

#endif /* BASICHDT_HPP_ */
