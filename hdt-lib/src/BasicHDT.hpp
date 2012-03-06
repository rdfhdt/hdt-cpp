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
	string fileName;

	void createComponents();
	void deleteComponents();
	void loadDictionary(const char *fileName, const char *baseUri, RDFNotation notation, ProgressListener *listener);
	void loadTriples(const char *fileName, const char *baseUri, RDFNotation notation, ProgressListener *listener);
	void fillHeader(string &baseUri);

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

	void loadFromRDF(const char *fileName, string baseUri, RDFNotation notation, ProgressListener *listener = NULL);

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

	void generateIndex(ProgressListener *listener = NULL);

	void saveIndex(ProgressListener *listener = NULL);

	void convert(HDTSpecification &spec);

	/**
	 * @param subject
	 * @param predicate
	 * @param object
	 * @return
	 */
	IteratorTripleString *search(const char *subject, const char *predicate, const char *object);
	VarBindingString *searchJoin(vector<TripleString> &patterns, set<string> &vars);
};

class DictionaryLoader : public RDFCallback {
private:
	Dictionary *dictionary;
	ProgressListener *listener;
	unsigned long long size;
	unsigned int count;
public:
	DictionaryLoader(Dictionary *dictionary, ProgressListener *listener, unsigned long long size) : dictionary(dictionary), listener(listener), size(size), count(0) { }
	void processTriple(TripleString &triple, unsigned long long pos);
};

class TriplesLoader : public RDFCallback {
private:
	Dictionary *dictionary;
	ModifiableTriples *triples;
	ProgressListener *listener;
	unsigned long long size;
public:
	TriplesLoader(Dictionary *dictionary, ModifiableTriples *triples, ProgressListener *listener, unsigned long long size) : dictionary(dictionary), triples(triples), listener(listener), size(size) { }
	void processTriple(TripleString &triple, unsigned long long pos);
};


class BasicModifiableHDT : public ModifiableHDT {
private:
	Header *header;
	Dictionary *dictionary;
	ModifiableTriples *triples;
	HDTSpecification spec;
	string fileName;

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

	void loadFromRDF(const char *fileName, string baseUri, RDFNotation notation, ProgressListener *listener = NULL);

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

	void generateIndex(ProgressListener *listener = NULL);

	void saveIndex(ProgressListener *listener = NULL);

	void convert(HDTSpecification &spec);

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
	VarBindingString *searchJoin(vector<TripleString> &patterns, set<string> &vars);

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

class BasicVarBindingString : public VarBindingString {
private:
	map<string, TripleComponentRole> varRole;
	VarBindingID *varID;
	Dictionary *dict;

	unsigned int getVarIndex(const char *varName) {
		for(int i=0;i<getNumVars();i++) {
			if(strcmp(getVarName(i), varName)==0) {
				return i;
			}
		}
		cout << "Var name: " << varName << " not found" << endl;
		throw "Var name does not exist";
	}
public:
	BasicVarBindingString(map<string, TripleComponentRole> &varRole, VarBindingID *varID, Dictionary *dict) :
	varRole(varRole),
	varID(varID),
	dict(dict)
	{

	}

	virtual ~BasicVarBindingString() {
		delete varID;
	}

	virtual bool findNext(){
		return varID->findNext();
	}

	virtual unsigned int getNumVars() {
		return varID->getNumVars();
	}
	virtual string getVar(unsigned int numvar) {
		unsigned int id = varID->getVarValue(numvar);
		string varName(getVarName(numvar));

		return dict->idToString(id, varRole.find(varName)->second);
	}
	virtual const char *getVarName(unsigned int numvar) {
		return varID->getVarName(numvar);
	}
        virtual void goToStart() {
            return varID->goToStart();
        }
        virtual unsigned int estimatedNumResults() {
            return varID->estimatedNumResults();
        }
};

}

#endif /* BASICHDT_HPP_ */
