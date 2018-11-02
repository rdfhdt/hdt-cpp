/*
 * File: PFCDictionary.hpp
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

#ifndef PFCDICTIONARY_HPP_
#define PFCDICTIONARY_HPP_

#include <Iterator.hpp>
#include <HDTSpecification.hpp>
#include <Dictionary.hpp>

#include "../libdcs/CSD.h"

namespace hdt {

class FourSectionDictionary : public Dictionary {
private:
	csd::CSD *subjects;
	csd::CSD *predicates;
	csd::CSD *objects;
	csd::CSD *shared;

    size_t mapping;
	uint64_t sizeStrings;
	uint32_t blocksize;

	//ControlInformation controlInformation;
	HDTSpecification spec;

public:
	FourSectionDictionary();
	FourSectionDictionary(HDTSpecification &spec);
	~FourSectionDictionary();

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

#endif /* PFCDictionary_HPP_ */
