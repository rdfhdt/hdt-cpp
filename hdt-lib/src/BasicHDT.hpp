/*
 * File BasicHDT.hpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * Copyright (C) 2011, Javier D. Fernandez, Miguel A. Martinez-Prieto
 *                     Mario Arias, Alejandro Andres.
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the authors:
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *   Mario Arias:               mario.arias@gmail.com
 *   Alejandro Andres:          fuzzy.alej@gmail.com
 *
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
	void loadDictionary(RDFParser &parser, ProgressListener *listener);
	void loadTriples(RDFParser &parser, ProgressListener *listener);

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
	void loadFromRDF(RDFParser &parser, ProgressListener *listener = NULL);

	/**
	 * @param input
	 */
	void loadFromHDT(std::istream &input, ProgressListener *listener = NULL);

	/**
	 * @param input
	 */
	void loadFromHDT(const char *fileName, ProgressListener *listener = NULL);

	/**
	 * @param output
	 * @param notation
	 */
	void saveToRDF(RDFSerializer &serializer, ProgressListener *listener = NULL);

	/**
	 * @param output
	 */
	void saveToHDT(std::ostream &output, ProgressListener *listener = NULL);

	/**
	 * @param output
	 */
	void saveToHDT(const char *fileName, ProgressListener *listener = NULL);



	/**
	 * @param subject
	 * @param predicate
	 * @param object
	 * @return
	 */
	virtual IteratorTripleString *search(const char *subject, const char *predicate, const char *object);
};


class BasicModifiableHDT : public ModifiableHDT {
private:
	Header *header;
	Dictionary *dictionary;
	ModifiableTriples *triples;
	HDTSpecification spec;

	void createComponents();

public:
	BasicModifiableHDT();

	BasicModifiableHDT(HDTSpecification &spec);

	virtual ~BasicModifiableHDT();

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
	void loadFromRDF(RDFParser &parser, ProgressListener *listener = NULL);

	/**
	 * @param input
	 */
	void loadFromHDT(std::istream &input, ProgressListener *listener = NULL);

	/**
	 * @param input
	 */
	void loadFromHDT(const char *fileName, ProgressListener *listener = NULL);

	/**
	 * @param output
	 * @param notation
	 */
	void saveToRDF(RDFSerializer &serializer, ProgressListener *listener = NULL);

	/**
	 * @param output
	 */
	void saveToHDT(std::ostream &output, ProgressListener *listener = NULL);

	/**
	 * @param output
	 */
	void saveToHDT(const char *fileName, ProgressListener *listener = NULL);


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

};

class BasicIteratorTripleString : public IteratorTripleString {

private:
	Dictionary *dict;
	IteratorTripleID *iterator;
	TripleString result;
public:
	BasicIteratorTripleString(Dictionary *dict, IteratorTripleID *iterator) {
		this->dict = dict;
		this->iterator = iterator;
	}

	bool hasNext() {
		return iterator->hasNext();
	}

	TripleString *next() {
		TripleID *tid = iterator->next();
		dict->tripleIDtoTripleString(*tid, result);
		return &result;
	}

	bool hasPrevious() {
		return iterator->hasPrevious();
	}

	TripleString *previous() {
		TripleID *tid = iterator->previous();
		dict->tripleIDtoTripleString(*tid, result);
		return &result;
	}

	void goToStart() {
		iterator->goToStart();
	}
};

}

#endif /* BASICHDT_HPP_ */
