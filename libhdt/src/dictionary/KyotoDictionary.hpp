/*
 * File: KyotoDictionary.hpp
 * Last modified: $Date: 2011-08-21 05:35:30 +0100 (dom, 21 ago 2011) $
 * Revision: $Revision: 180 $
 * Last modified by: $Author: mario.arias $
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

#ifndef KYOTODICTIONARY_H_
#define KYOTODICTIONARY_H_

#include <HDTSpecification.hpp>
#include <Dictionary.hpp>
#include <Iterator.hpp>

#include <string.h>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>

#ifdef HAVE_KYOTO

#include <kcpolydb.h>

using namespace kyotocabinet;

namespace hdt {


class KyotoDictionary : public ModifiableDictionary {
// Private attributes
public:

	unsigned int mapping;
	uint64_t sizeStrings;

	//ControlInformation controlInformation;
	HDTSpecification spec;
	TreeDB subjects, objects, shared;
	TreeDB predicates;

// Public Interface
public:
	KyotoDictionary();
	KyotoDictionary(HDTSpecification &spec);
	~KyotoDictionary();

    std::string idToString(size_t id, TripleComponentRole position);
    size_t stringToId(const std::string &str, TripleComponentRole position);

	size_t getNumberOfElements();

    uint64_t size();

    size_t getNsubjects();
    size_t getNpredicates();
    size_t getNobjects();
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

    size_t insert(const std::string &str, TripleComponentRole position);

	void startProcessing(ProgressListener *listener = NULL);
	void stopProcessing(ProgressListener *listener = NULL);

	string getType();
    unsigned int getMapping();

    void getSuggestions(const char *base, TripleComponentRole role, std::vector<string> &out, int maxResults);

    hdt::IteratorUCharString *getSuggestions(const char *prefix, TripleComponentRole role);

    hdt::IteratorUInt *getIDSuggestions(const char *prefix, TripleComponentRole role);

// Private methods
private:
    void updateIDs(DB *db);

public:
    size_t getGlobalId(unsigned int mapping, size_t id, DictionarySection position);
    size_t getGlobalId(size_t id, DictionarySection position);
    size_t getLocalId(size_t mapping, size_t id, TripleComponentRole position);
    size_t getLocalId(size_t id, TripleComponentRole position);

	void dumpSizes(std::ostream &out);

	friend class PFCDictionary;
};

class KyotoDictIterator : public IteratorUCharString {
private:
	DB *db;
	DB::Cursor *cur;
	string key;
	size_t count;
public:
	KyotoDictIterator(DB *db) : db(db), cur(db->cursor()), count(0) {
		cur->jump();
	}

	~KyotoDictIterator() {
		delete cur;
	}

	bool hasNext() {
		return count<db->count();
	}

	unsigned char *next() {
		cur->get_key(&key, true);
		count++;
		return (unsigned char*)(key.c_str());
	}

	size_t getNumberOfElements() {
		return db->count();
	}
};

}

#endif  /* HAVE_KYOTO */

#endif /* KYOTODICTIONARY_H_ */
