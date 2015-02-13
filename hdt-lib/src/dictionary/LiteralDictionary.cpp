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

#if HAVE_CDS

#include "LiteralDictionary.hpp"
#include <HDTVocabulary.hpp>
#include "../libdcs/CSD_PFC.h"
#include "../libdcs/CSD_HTFC.h"
#include "../libdcs/CSD_FMIndex.h"
#include "../libdcs/CSD_Cache.h"
#include "../libdcs/CSD_Cache2.h"

namespace hdt {

LiteralDictionary::LiteralDictionary() : blocksize(8) {
	subjects = new csd::CSD_PFC();
	predicates = new csd::CSD_PFC();
	objectsNotLiterals = new csd::CSD_PFC();
	objectsLiterals = new csd::CSD_FMIndex();
	shared = new csd::CSD_PFC();
}

LiteralDictionary::LiteralDictionary(HDTSpecification & spec) : blocksize(8) {
	subjects = new csd::CSD_PFC();
	predicates = new csd::CSD_PFC();
	objectsNotLiterals = new csd::CSD_PFC();
	objectsLiterals = new csd::CSD_FMIndex();
	shared = new csd::CSD_PFC();

	string blockSizeStr = spec.get("dict.block.size");
	if (blockSizeStr != "") {
		blocksize = atoi(blockSizeStr.c_str());
	}
}

LiteralDictionary::~LiteralDictionary() {
	delete subjects;
	delete predicates;
	delete objectsNotLiterals;
	delete objectsLiterals;
	delete shared;
}

csd::CSD *loadSectionPFC(IteratorUCharString *iterator, uint32_t blocksize, ProgressListener *listener) {
	return new csd::CSD_PFC(iterator, blocksize, listener);
	//return new csd::CSD_HTFC(iterator, blocksize, listener);
}

/*
 * use_sample = true ; enables to locate substrings.
 */
csd::CSD *loadSectionFMIndex(IteratorUCharString *iterator, bool sparse_bitsequence, int bparam,
		size_t bwt_sample, bool use_sample, hdt::ProgressListener *listener) {
	return new csd::CSD_FMIndex(iterator, sparse_bitsequence, bparam, bwt_sample, use_sample, listener);
}

std::string LiteralDictionary::idToString(unsigned int id, TripleComponentRole position) {
	csd::CSD *section = getDictionarySection(id, position);
	unsigned int localid = getLocalId(id, position);

	if (localid <= section->getLength()) {
		const char * ptr = (const char *) section->extract(localid);
		if (ptr != NULL) {
			string out = ptr;
			section->freeString((unsigned char*)ptr);
			return out;
		} else {
			//cout << "Not found: " << id << " as " << position << endl;
		}
	}

	return string();
}

unsigned int LiteralDictionary::stringToId(std::string &key, TripleComponentRole position) {
	unsigned int ret;

	if (key.length() == 0) {
		return 0;
	}

	switch (position) {
	case SUBJECT:
		ret = shared->locate((const unsigned char *) key.c_str(), key.length());
		if (ret != 0) {
			return getGlobalId(ret, SHARED_SUBJECT);
		}
		ret = subjects->locate((const unsigned char *) key.c_str(), key.length());
		if (ret != 0) {
			return getGlobalId(ret, NOT_SHARED_SUBJECT);
		}
		throw "Subject not found in dictionary";
	case PREDICATE:
		ret = predicates->locate((const unsigned char *) key.c_str(), key.length());
		if (ret != 0) {
			return getGlobalId(ret, NOT_SHARED_PREDICATE);
		}
		throw "Predicate not found in dictionary";

	case OBJECT:
		if (key.at(0) == '"') {
			ret = objectsLiterals->locate((const unsigned char *) key.c_str(),
					key.length());
			if (ret != 0) {
				return getGlobalId(ret, NOT_SHARED_OBJECT);
			}
			else{
				throw "Object not found in dictionary";
			}
		} else {
			ret = shared->locate((const unsigned char *) key.c_str(), key.length());
			if (ret != 0) {
				return getGlobalId(ret, SHARED_OBJECT);
			}
			ret = objectsNotLiterals->locate((const unsigned char *) key.c_str(), key.length());
			if (ret != 0) {
				return getGlobalId(ret, NOT_SHARED_OBJECT)+	objectsLiterals->getLength();
			}
			throw "Object not found in dictionary";
		}
	}
}

void LiteralDictionary::load(std::istream & input, ControlInformation & ci,	ProgressListener *listener) {
	std::string format = ci.getFormat();
	if(format!=getType()) {
		throw "Trying to read a LiteralDictionary but the data is not LiteralDictionary";
	}
	this->mapping = ci.getUint("mapping");
	this->sizeStrings = ci.getUint("sizeStrings");

	IntermediateListener iListener(listener);

	iListener.setRange(0, 25);
	iListener.notifyProgress(0, "Dictionary read shared area.");
	delete shared;
	shared = csd::CSD::load(input);
	if (shared == NULL) {
		shared = new csd::CSD_PFC();
		throw "Could not read shared sectionsss.";
	}

	iListener.setRange(25, 50);
	iListener.notifyProgress(0, "Dictionary read subjects.");
	delete subjects;
	subjects = csd::CSD::load(input);
	if (subjects == NULL) {
		subjects = new csd::CSD_PFC();
		throw "Could not read subjects.";
	}
	subjects = new csd::CSD_Cache(subjects);

	iListener.setRange(50, 75);
	iListener.notifyProgress(0, "Dictionary read predicates.");

	delete predicates;
	predicates = csd::CSD::load(input);
	if (predicates == NULL) {
		predicates = new csd::CSD_PFC();
		throw "Could not read predicates.";
	}
	predicates = new csd::CSD_Cache2(predicates);

	iListener.setRange(75, 100);
	iListener.notifyProgress(0, "Dictionary read objects.");

	delete objectsLiterals;
	objectsLiterals = csd::CSD::load(input);
	if (objectsLiterals == NULL) {
		objectsLiterals = new csd::CSD_FMIndex();
		throw "Could not read objects Literals.";
	}
	objectsLiterals = new csd::CSD_Cache(objectsLiterals);

	delete objectsNotLiterals;
	objectsNotLiterals = csd::CSD::load(input);
	if (objectsNotLiterals == NULL) {
		objectsNotLiterals = new csd::CSD_PFC();
		throw "Could not read objects not Literals.";
	}
    objectsNotLiterals = new csd::CSD_Cache(objectsNotLiterals);
}

size_t LiteralDictionary::load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener)
{
    size_t count=0;

    IntermediateListener iListener(listener);
    ControlInformation ci;
    count += ci.load(&ptr[count], ptrMax);

    this->mapping = ci.getUint("mapping");
    this->sizeStrings = ci.getUint("sizeStrings");

    iListener.setRange(0,25);
    iListener.notifyProgress(0, "Dictionary read shared area.");
    delete shared;
    shared = csd::CSD::create(ptr[count]);
    if(shared==NULL){
        shared = new csd::CSD_PFC();
        throw "Could not read shared.";
    }
    count += shared->load(&ptr[count], ptrMax);
    shared = new csd::CSD_Cache(shared);

    iListener.setRange(25,50);
    iListener.notifyProgress(0, "Dictionary read subjects.");
    delete subjects;
    subjects = csd::CSD::create(ptr[count]);
    if(subjects==NULL){
        subjects = new csd::CSD_PFC();
        throw "Could not read subjects.";
    }
    count += subjects->load(&ptr[count], ptrMax);
    subjects = new csd::CSD_Cache(subjects);

    iListener.setRange(50,75);
    iListener.notifyProgress(0, "Dictionary read predicates.");
    delete predicates;
    predicates = csd::CSD::create(ptr[count]);
    if(predicates==NULL){
        predicates = new csd::CSD_PFC();
        throw "Could not read predicates.";
    }
    count += predicates->load(&ptr[count], ptrMax);
    predicates = new csd::CSD_Cache(predicates);

    iListener.setRange(75,85);
    iListener.notifyProgress(0, "Dictionary read objects literals.");
    delete objectsLiterals;
    objectsLiterals = csd::CSD::create(ptr[count]);
    if(objectsLiterals==NULL){
        objectsLiterals = new csd::CSD_PFC();
        throw "Could not read object Literals.";
    }
    count += objectsLiterals->load(&ptr[count], ptrMax);
    objectsLiterals = new csd::CSD_Cache(objectsLiterals);

    iListener.setRange(75,100);
    iListener.notifyProgress(0, "Dictionary read objects Rest.");
    delete objectsNotLiterals;
    objectsNotLiterals = csd::CSD::create(ptr[count]);
    if(objectsNotLiterals==NULL){
        objectsNotLiterals = new csd::CSD_PFC();
        throw "Could not read objects Not Literals.";
    }
    count += objectsNotLiterals->load(&ptr[count], ptrMax);
    objectsNotLiterals = new csd::CSD_Cache(objectsNotLiterals);

    return count;
}

class LiteralIterator : public IteratorUCharString {
private:
	IteratorUCharString *child;
	unsigned char *previous, *nextItem;
	bool goon;

public:
	LiteralIterator(IteratorUCharString *child) : child(child), previous(NULL), nextItem(NULL), goon(false) {
		if(child->hasNext()) {
			nextItem = child->next();
		}
    }

	virtual ~LiteralIterator() {
		// Attention: Does not delete child.
	}

	bool hasNext() {
		if(goon) {
			return nextItem!=NULL;
		} else {
			return nextItem!=NULL && nextItem[0]=='"';
		}
	}

	unsigned char *next() {
		if(previous) {
			child->freeStr(previous);
		}
		previous = nextItem;
		if(child->hasNext()) {
			nextItem = child->next();
		} else {
			nextItem = NULL;
		}
		return previous;
	}

    size_t getNumberOfElements() {
		return child->getNumberOfElements();
	}

	void doContinue() {
		goon = true;
	}
};

void LiteralDictionary::import(	Dictionary *other, ProgressListener *listener) {
	try {
		IntermediateListener iListener(listener);

        //NOTIFY(listener, "DictionaryPFC loading shared", 90, 100);
        IteratorUCharString *itShared = other->getShared();
        delete shared;
        iListener.setRange(90, 100);
        shared = loadSectionPFC(itShared, blocksize, &iListener);
        shared = new csd::CSD_Cache(shared);
        delete itShared;

		//NOTIFY(listener, "DictionaryPFC loading subjects", 0, 100);
		IteratorUCharString *itSubj = other->getSubjects();
		delete subjects;
		iListener.setRange(0, 20);
		subjects = loadSectionPFC(itSubj, blocksize, &iListener);
		subjects = new csd::CSD_Cache(subjects);
		delete itSubj;

		//NOTIFY(listener, "DictionaryPFC loading predicates", 25, 30);
		IteratorUCharString *itPred = other->getPredicates();
		delete predicates;
		iListener.setRange(20, 21);
		predicates = loadSectionPFC(itPred, blocksize, &iListener);
		subjects = new csd::CSD_Cache2(subjects);
		delete itPred;

		//NOTIFY(listener, "DictionaryPFC loading objects", 30, 90);
		iListener.setRange(21, 50);
		IteratorUCharString *itObj = other->getObjects();
		LiteralIterator litIt(itObj);

		delete objectsLiterals;
		objectsLiterals = loadSectionFMIndex(&litIt, false, 4, 64, true, &iListener);
		objectsLiterals = new csd::CSD_Cache(objectsLiterals);
		litIt.doContinue();

		iListener.setRange(50, 90);
		delete objectsNotLiterals;
		objectsNotLiterals = loadSectionPFC(&litIt, blocksize, &iListener);
		objectsNotLiterals = new csd::CSD_Cache(objectsNotLiterals);
		delete itObj;

		this->sizeStrings = other->size();
		this->mapping = other->getMapping();
	} catch (const char *e) {
		delete subjects;
		delete predicates;
		delete objectsLiterals;
		delete objectsNotLiterals;
		delete shared;
		subjects = new csd::CSD_PFC();
		predicates = new csd::CSD_PFC();
		objectsNotLiterals = new csd::CSD_PFC();
		objectsLiterals = new csd::CSD_FMIndex();
		shared = new csd::CSD_PFC();
		throw e;
	}
}

IteratorUCharString *LiteralDictionary::getSubjects() {
	throw "Not implemented";
}

IteratorUCharString *LiteralDictionary::getPredicates() {
	throw "Not implemented";
}

IteratorUCharString *LiteralDictionary::getObjects() {
	throw "Not implemented";
}

IteratorUCharString *LiteralDictionary::getShared() {
	throw "Not implemented";
}

uint32_t LiteralDictionary::substringToId(unsigned char *s, uint32_t len, uint32_t **occs){

	if(len==0) {
		return 0;
	}

	csd::CSD_FMIndex *fmIndex=NULL;

	csd::CSD_Cache *cache = dynamic_cast<csd::CSD_Cache *>(objectsLiterals);
	if(cache!=NULL) {
		fmIndex = dynamic_cast<csd::CSD_FMIndex  *>(cache->getChild());
	} else {
		fmIndex = dynamic_cast<csd::CSD_FMIndex  *>(objectsLiterals);
	}

	if(fmIndex!=NULL) {
		uint32_t ret = fmIndex->locate_substring(s,len,occs);
		for (int i=0;i<ret;i++){
			(*occs)[i] = this->getGlobalId((*occs)[i], NOT_SHARED_OBJECT);
		}
		return ret;
	}

	cerr << "Warning, trying to call LiteralDictionary::substringToId() but it was not an FM-Index.";
	return 0;
}

void LiteralDictionary::save(std::ostream & output,	ControlInformation & controlInformation, ProgressListener *listener) {
	controlInformation.setFormat(HDTVocabulary::DICTIONARY_TYPE_LITERAL);

	controlInformation.setUint("mapping", this->mapping);
	controlInformation.setUint("sizeStrings", this->sizeStrings);

	controlInformation.save(output);

	IntermediateListener iListener(listener);

	iListener.setRange(0, 10);
	iListener.notifyProgress(0, "Dictionary save shared area.");
	shared->save(output);

	iListener.setRange(10, 45);
	iListener.notifyProgress(0, "Dictionary save subjects.");
	subjects->save(output);

	iListener.setRange(45, 60);
	iListener.notifyProgress(0, "Dictionary save predicates.");
	predicates->save(output);

	iListener.setRange(60, 80);
	iListener.notifyProgress(0, "Dictionary save literal objects.");
	objectsLiterals->save(output);

	iListener.setRange(80, 100);
    iListener.notifyProgress(0, "Dictionary save non literal objects.");
    objectsNotLiterals->save(output);
}

void LiteralDictionary::populateHeader(Header & header, string rootNode) {
	header.insert(rootNode, HDTVocabulary::DICTIONARY_TYPE,
			HDTVocabulary::DICTIONARY_TYPE_LITERAL);
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMSUBJECTS,
			getNsubjects());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMPREDICATES,
			getNpredicates());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMOBJECTS,
			getNobjects());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMSHARED, getNshared());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXSUBJECTID,
			getMaxSubjectID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXPREDICATEID,
			getMaxPredicateID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXOBJECTTID,
			getMaxObjectID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAPPING, this->mapping);
	header.insert(rootNode, HDTVocabulary::DICTIONARY_SIZE_STRINGS, size());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_BLOCK_SIZE,
			this->blocksize);
}

unsigned int LiteralDictionary::getNsubjects() {
	return shared->getLength() + subjects->getLength();
}
unsigned int LiteralDictionary::getNpredicates() {
	return predicates->getLength();
}
unsigned int LiteralDictionary::getNobjects() {
	return shared->getLength() + objectsNotLiterals->getLength() + objectsLiterals->getLength();
}
unsigned int LiteralDictionary::getNshared() {
	return shared->getLength();
}

unsigned int LiteralDictionary::getMaxID() {
	unsigned int s = subjects->getLength();
	unsigned int o = objectsLiterals->getLength()+objectsNotLiterals->getLength();
	unsigned int sh = shared->getLength();
	unsigned int max = s > o ? s : o;

	if (mapping == MAPPING2) {
		return sh + max;
	} else {
		return sh + s + o;
	}
}

unsigned int LiteralDictionary::getMaxSubjectID() {
	return getNsubjects();
}

unsigned int LiteralDictionary::getMaxPredicateID() {
	return predicates->getLength();
}

unsigned int LiteralDictionary::getMaxObjectID() {
	unsigned int s = subjects->getLength();
	unsigned int o = objectsLiterals->getLength()+objectsNotLiterals->getLength();
	unsigned int sh = shared->getLength();

	if (mapping == MAPPING2) {
		return sh + o;
	} else {
		return sh + s + o;
	}
}

size_t LiteralDictionary::getNumberOfElements() {
	return shared->getLength() + subjects->getLength() + predicates->getLength()
			+ objectsLiterals->getLength()+objectsNotLiterals->getLength();
}

uint64_t LiteralDictionary::size() {
	return shared->getSize() + subjects->getSize() + predicates->getSize()
			+ objectsLiterals->getSize()+objectsNotLiterals->getSize();
}

void LiteralDictionary::startProcessing(ProgressListener *listener) {
}

void LiteralDictionary::stopProcessing(ProgressListener *listener) {

}

unsigned int LiteralDictionary::insert(std::string & str,
		TripleComponentRole position) {
	throw "This dictionary does not support insertions.";
}

string LiteralDictionary::getType() {
	return HDTVocabulary::DICTIONARY_TYPE_LITERAL;
}

unsigned int LiteralDictionary::getMapping() {
	return mapping;
}

csd::CSD *LiteralDictionary::getDictionarySection(unsigned int id, TripleComponentRole position) {
	switch (position) {
	case SUBJECT:
		if (id <= shared->getLength()) {
			//cout << "Section SHARED" << endl;
			return shared;
		} else {
			//cout << "Section SUBJECTS" << endl;
			return subjects;
		}

	case PREDICATE:
		return predicates;

	case OBJECT:

		if (id <= shared->getLength()) {
			//cout << "Section SHARED" << endl;
			return shared;
		} else {
			unsigned int localId = 0;
			if (mapping == MAPPING2) {
				localId = id - shared->getLength();
			} else {
				localId = 2+ id - shared->getLength() - subjects->getLength();
			}

			if (localId <= objectsLiterals->getLength()) {
				return objectsLiterals;
			} else {
				return objectsNotLiterals;
			}
		}
	}

	throw "Item not found";
}

unsigned int LiteralDictionary::getGlobalId(unsigned int mapping, unsigned int id, DictionarySection position) {
	switch (position) {
	case NOT_SHARED_SUBJECT:
		return shared->getLength() + id;

	case NOT_SHARED_PREDICATE:
		return id;

	case NOT_SHARED_OBJECT:
		if (mapping == MAPPING2) {
			return shared->getLength() + id;
		} else {
			return shared->getLength() + subjects->getLength() + id;
		}

	case SHARED_SUBJECT:
	case SHARED_OBJECT:
		return id;
	}

	throw "Item not found";
}

unsigned int LiteralDictionary::getGlobalId(unsigned int id, DictionarySection position) {
	return getGlobalId(this->mapping, id, position);
}

unsigned int LiteralDictionary::getLocalId(unsigned int mapping, unsigned int id, TripleComponentRole position) {
	switch (position) {
	case SUBJECT:
		if (id <= shared->getLength()) {
			return id;
		} else {
			return id - shared->getLength();
		}
		break;
	case OBJECT:
		if (id <= shared->getLength()) {
			return id;
		} else {
			unsigned int localId = 0;
			if (mapping == MAPPING2) {
				localId = id - shared->getLength();
			} else {
				localId =  2 + id - shared->getLength() - subjects->getLength();
			}

			if (localId <= objectsLiterals->getLength()) {
							return localId;
			} else {
				return localId-objectsLiterals->getLength();
			}


		}
		break;
	case PREDICATE:
		return id;
	}

	throw "Item not found";
}

unsigned int LiteralDictionary::getLocalId(unsigned int id, TripleComponentRole position) {
	return getLocalId(mapping, id, position);
}

void LiteralDictionary::getSuggestions(const char *base, hdt::TripleComponentRole role, std::vector<std::string> &out, int maxResults) {
	if (role == PREDICATE) {
		predicates->fillSuggestions(base, out, maxResults);
		return;
	}

	vector<string> v1, v2;
	shared->fillSuggestions(base, v1, maxResults);
	if (role == SUBJECT) {
		subjects->fillSuggestions(base, v2, maxResults);
	} else if (role == OBJECT) {
        if (base[0]=='"'){
            objectsLiterals->fillSuggestions(base, v2, maxResults);
        }
        else{
            objectsNotLiterals->fillSuggestions(base, v2, maxResults);
        }
	}

	// Merge results from shared and subjects/objects keeping order
	merge(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(out));

	// Remove possible extra items
	if (out.size() > maxResults) {
		out.resize(maxResults);
	}
}

}

#else
int LiteralDictionayDummySymbol;
#endif
