/*
 * File: PlainDictionary.hpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * Copyright (C) 2012, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
 * All rights reserved.
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
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#ifndef PLAINDICTIONARY_H_
#define PLAINDICTIONARY_H_

#include <Iterator.hpp>
#include <HDTSpecification.hpp>
#include <Dictionary.hpp>

#include <string.h>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>

#ifndef WIN32
#include <ext/hash_map>
#else
#include <unordered_map>
#endif

//#define GOOGLE_HASH

#ifdef GOOGLE_HASH
#include <sparsehash/sparse_hash_map>

using google::sparse_hash_map;      // namespace where class lives by default
using __gnu_cxx::hash;  // or __gnu_cxx::hash, or maybe tr1::hash, depending on your OS

#else

#ifndef WIN32
namespace std { using namespace __gnu_cxx; }
#endif

#endif


namespace hdt {

struct DictionaryEntry {
public:
    size_t id;
	char *str;

	bool static cmpLexicographic(DictionaryEntry *c1, DictionaryEntry *c2);
	bool static cmpID(DictionaryEntry *c1, DictionaryEntry *c2);
};

struct str_cmp {
	bool operator()(const char* s1, const char* s2) const {
		return strcmp(s1, s2) == 0;
	}
};

typedef std::pair<const char*, DictionaryEntry *> DictEntryPair;

#ifdef GOOGLE_HASH
typedef sparse_hash_map<const char *, DictionaryEntry *, hash<const char *>, str_cmp> DictEntryHash;
#else

#ifdef WIN32
/*typedef std::hash_map<const char *, DictionaryEntry *, hash<const char *>, str_cmp> DictEntryHash;*/
typedef unordered_map<const char *, DictionaryEntry *, hash<const char *>, str_cmp> DictEntryHash;
#else
typedef std::hash_map<const char *, DictionaryEntry *, __gnu_cxx::hash<const char *>, str_cmp> DictEntryHash;

#endif
#endif

typedef DictEntryHash::const_iterator DictEntryIt;


class PlainDictionary : public ModifiableDictionary {
private:
	std::vector<DictionaryEntry*> predicates;
	std::vector<DictionaryEntry*> shared;
	std::vector<DictionaryEntry*> subjects;
	std::vector<DictionaryEntry*> objects;
	DictEntryHash hashSubject;
	DictEntryHash hashPredicate;
	DictEntryHash hashObject;
    size_t mapping;
	uint64_t sizeStrings;

	//ControlInformation controlInformation;
	HDTSpecification spec;

// Public Interface
public:
	PlainDictionary();
	PlainDictionary(HDTSpecification &spec);
	~PlainDictionary();

    std::string idToString(size_t id, TripleComponentRole position);
    size_t stringToId(const std::string &str, TripleComponentRole position);

    size_t getNumberOfElements();

    uint64_t size();

    size_t getNsubjects();
    size_t getNpredicates();
    size_t getNobjects();
    size_t getNobjectsLiterals();
    size_t getNobjectsNotLiterals();
    size_t getNshared();

    size_t getMaxID();
    size_t getMaxSubjectID();
    size_t getMaxPredicateID();
    size_t getMaxObjectID();

	void populateHeader(Header &header, string rootNode);
	void save(std::ostream &output, ControlInformation &ci, ProgressListener *listener = NULL);
	void load(std::istream &input, ControlInformation &ci, ProgressListener *listener = NULL);

	size_t load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener=NULL);

    void import(Dictionary *other, ProgressListener *listener=NULL);

    IteratorUCharString *getSubjects();
    IteratorUCharString *getPredicates();
    IteratorUCharString *getObjects();
    IteratorUCharString *getShared();

// ModifiableDictionary
    size_t insert(const std::string &str, TripleComponentRole position);

	void startProcessing(ProgressListener *listener = NULL);
	void stopProcessing(ProgressListener *listener = NULL);

	string getType();
    size_t getMapping();

	void getSuggestions(const char *base, TripleComponentRole role, std::vector<string> &out, int maxResults);

    hdt::IteratorUCharString *getSuggestions(const char *prefix, TripleComponentRole role);

    hdt::IteratorUInt *getIDSuggestions(const char *prefix, TripleComponentRole role);

// Private methods
private:
	void insert(std::string entry, DictionarySection pos);

	void split(ProgressListener *listener = NULL);
	void lexicographicSort(ProgressListener *listener = NULL);
	void idSort();
	void updateIDs();

    std::vector<DictionaryEntry*> &getDictionaryEntryVector(size_t id, TripleComponentRole position);

public:
    size_t getGlobalId(size_t mapping, size_t id, DictionarySection position);
    size_t getGlobalId(size_t id, DictionarySection position);
    size_t getLocalId(size_t mapping, size_t id, TripleComponentRole position);
    size_t getLocalId(size_t id, TripleComponentRole position);

    void convertMapping(size_t mapping);
    void updateID(size_t oldid, size_t newid, DictionarySection position);
};


class DictIterator : public IteratorUCharString {
private:
	std::vector<DictionaryEntry *> &vector;
    size_t pos;
public:
	DictIterator(std::vector<DictionaryEntry *> &vector) : vector(vector), pos(0){

	}
	virtual ~DictIterator() { }

	virtual bool hasNext() {
		return pos<vector.size();
	}

	virtual unsigned char *next() {
		return (unsigned char*)vector[pos++]->str;
	}

    virtual size_t getNumberOfElements() {
		return vector.size();
	}
};


}

#endif /* PLAINDICTIONARY_H_ */
