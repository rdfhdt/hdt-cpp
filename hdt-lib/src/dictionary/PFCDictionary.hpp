/*
 * PFCDictionary.h
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#ifndef PFCDICTIONARY_HPP_
#define PFCDICTIONARY_HPP_

#include <HDTSpecification.hpp>
#include "PlainDictionary.hpp"

#include "../libdcs/CSD.h"

namespace hdt {

class PFCDictionary : public Dictionary {
private:
	csd::CSD *subjects;
	csd::CSD *predicates;
	csd::CSD *objects;
	csd::CSD *shared;

	unsigned int mapping;
	size_t sizeStrings;
	size_t blocksize;

	//ControlInformation controlInformation;
	HDTSpecification spec;

public:
	PFCDictionary();
	PFCDictionary(PlainDictionary *dictionary);
	PFCDictionary(HDTSpecification &spec);
	~PFCDictionary();

	std::string idToString(unsigned int id, TripleComponentRole position);
	unsigned int stringToId(std::string &str, TripleComponentRole position);

	unsigned int getNumberOfElements();

    unsigned int size();

	unsigned int getNsubjects();
	unsigned int getNpredicates();
	unsigned int getNobjects();
	unsigned int getSsubobj();

	unsigned int getMaxID();
	unsigned int getMaxSubjectID();
	unsigned int getMaxPredicateID();
	unsigned int getMaxObjectID();

	void populateHeader(Header &header, string rootNode);
	bool save(std::ostream &output, ControlInformation &ci, ProgressListener *listener = NULL);
	void load(std::istream &input, ControlInformation &ci, ProgressListener *listener = NULL);

	unsigned int insert(std::string &str, TripleComponentRole position);

	void startProcessing();
	void stopProcessing();

	string getType();

	void import(PlainDictionary *dictionary);

private:
	csd::CSD *getDictionarySection(unsigned int id, TripleComponentRole position);
	unsigned int getGlobalId(unsigned int mapping, unsigned int id, DictionarySection position);
	unsigned int getGlobalId(unsigned int id, DictionarySection position);
	unsigned int getLocalId(unsigned int mapping, unsigned int id, TripleComponentRole position);
	unsigned int getLocalId(unsigned int id, TripleComponentRole position);
};

}

#endif /* PFCDictionary_HPP_ */
