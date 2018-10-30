/*
 * File: LiteralDictionary.hpp
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

#ifndef LITERALDICTIONARY_HPP_
#define LITERALDICTIONARY_HPP_

#include <Iterator.hpp>
#include <Dictionary.hpp>
#include <HDTSpecification.hpp>

#include "../libdcs/CSD.h"

namespace hdt {

class LiteralDictionary : public Dictionary {
private:
	csd::CSD *subjects;
	csd::CSD *predicates;
	csd::CSD *objectsNotLiterals;
	csd::CSD *objectsLiterals;
	csd::CSD *shared;

    size_t mapping;
	uint64_t sizeStrings;
	uint32_t blocksize;

	//ControlInformation controlInformation;
	HDTSpecification spec;

public:
	LiteralDictionary();
	LiteralDictionary(HDTSpecification &spec);
	~LiteralDictionary();

    std::string idToString(size_t id, TripleComponentRole position);
    size_t stringToId(const std::string &str, TripleComponentRole position);

	/** Returns the number of IDs that contain s[1,..len] as a substring. It also
	 * return in occs the IDs. Otherwise return 0.
	 *  @s: the substring to be located.
	 *  @len: the length (in characters) of the string s.
	 *  @occs: pointer where the ID located will be stored.
	 * */
    size_t substringToId(unsigned char *s, size_t len, uint32_t **occs);
    size_t substringToId(unsigned char *s, size_t len, size_t offset, size_t limit, bool deduplicate, uint32_t **occs, uint32_t* num_occ);

    size_t getNumberOfElements();

    uint64_t size();

    size_t getNsubjects();
    size_t getNpredicates();
    size_t getNobjects();
    size_t getNshared();

    size_t getNobjectsNotLiterals();
    size_t getNobjectsLiterals();

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
    size_t getMapping();

	void getSuggestions(const char *base, TripleComponentRole role, std::vector<string> &out, int maxResults);

    hdt::IteratorUCharString *getSuggestions(const char *prefix, TripleComponentRole role);
    hdt::IteratorUInt *getIDSuggestions(const char *prefix, TripleComponentRole role);


private:
    csd::CSD *getDictionarySection(size_t id, TripleComponentRole position);
    size_t getGlobalId(size_t mapping, size_t id, DictionarySection position);
    size_t getGlobalId(size_t id, DictionarySection position);
    size_t getLocalId(size_t mapping, size_t id, TripleComponentRole position);
    size_t getLocalId(size_t id, TripleComponentRole position);
};

}

#endif /* LiteralDictionary_HPP_ */
