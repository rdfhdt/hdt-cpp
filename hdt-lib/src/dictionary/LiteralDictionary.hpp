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

	unsigned int mapping;
	uint64_t sizeStrings;
	uint32_t blocksize;

	//ControlInformation controlInformation;
	HDTSpecification spec;

public:
	LiteralDictionary();
	LiteralDictionary(HDTSpecification &spec);
	~LiteralDictionary();

	std::string idToString(unsigned int id, TripleComponentRole position);
	unsigned int stringToId(std::string &str, TripleComponentRole position);

	/** Returns the number of IDs that contain s[1,..len] as a substring. It also
	 * return in occs the IDs. Otherwise return 0.
	 *  @s: the substring to be located.
	 *  @len: the length (in characters) of the string s.
	 *  @occs: pointer where the ID located will be stored.
	 * */
	uint32_t substringToId(unsigned char *s, uint32_t len, uint32_t **occs);

	unsigned int getNumberOfElements();

    unsigned int size();

	unsigned int getNsubjects();
	unsigned int getNpredicates();
	unsigned int getNobjects();
	unsigned int getNshared();

	unsigned int getNobjectsNotLiterals();
	unsigned int getNobjectsLiterals();

	unsigned int getMaxID();
	unsigned int getMaxSubjectID();
	unsigned int getMaxPredicateID();
	unsigned int getMaxObjectID();

	void populateHeader(Header &header, string rootNode);
	void save(std::ostream &output, ControlInformation &ci, ProgressListener *listener = NULL);
	void load(std::istream &input, ControlInformation &ci, ProgressListener *listener = NULL);

	size_t load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener=NULL);

    void import(Dictionary *other, ProgressListener *listener=NULL);

    IteratorUCharString *getSubjects();
    IteratorUCharString *getPredicates();
    IteratorUCharString *getObjects();
    IteratorUCharString *getShared();

	unsigned int insert(std::string &str, TripleComponentRole position);

	void startProcessing(ProgressListener *listener = NULL);
	void stopProcessing(ProgressListener *listener = NULL);

	string getType();
	unsigned int getMapping();

	void getSuggestions(const char *base, TripleComponentRole role, std::vector<string> &out, int maxResults);

private:
	csd::CSD *getDictionarySection(unsigned int id, TripleComponentRole position);
	unsigned int getGlobalId(unsigned int mapping, unsigned int id, DictionarySection position);
	unsigned int getGlobalId(unsigned int id, DictionarySection position);
	unsigned int getLocalId(unsigned int mapping, unsigned int id, TripleComponentRole position);
	unsigned int getLocalId(unsigned int id, TripleComponentRole position);
};

}

#endif /* LiteralDictionary_HPP_ */
