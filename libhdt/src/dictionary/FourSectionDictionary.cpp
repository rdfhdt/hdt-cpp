/*
 * File: FourSectionDictionary.cpp
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

#include <algorithm>

#include "FourSectionDictionary.hpp"
#include <HDTVocabulary.hpp>
#include <stdexcept>
#include "../libdcs/CSD_PFC.h"
#include "../libdcs/CSD_HTFC.h"
#include "../libdcs/CSD_Cache.h"
#include "../libdcs/CSD_Cache2.h"

namespace hdt {

FourSectionDictionary::FourSectionDictionary() : blocksize(16)
{
	subjects = new csd::CSD_PFC();
	predicates = new csd::CSD_PFC();
	objects = new csd::CSD_PFC();
	shared = new csd::CSD_PFC();
}

FourSectionDictionary::FourSectionDictionary(HDTSpecification & spec) : blocksize(16)
{
	subjects = new csd::CSD_PFC();
	predicates = new csd::CSD_PFC();
	objects = new csd::CSD_PFC();
	shared = new csd::CSD_PFC();

	string blockSizeStr = spec.getOrEmpty("dict.block.size");

	if(!blockSizeStr.empty() && (blockSizeStr.find_first_not_of("0123456789") == string::npos))
	{
		blocksize = std::stoi( blockSizeStr );
		if ( blocksize <= 0 )
		{
			throw std::runtime_error("blocksize must be greater than 0");
		}
	}
}

FourSectionDictionary::~FourSectionDictionary()
{
	delete subjects;
	delete predicates;
	delete objects;
	delete shared;
}

csd::CSD *loadSection(IteratorUCharString *iterator, uint32_t blocksize, ProgressListener *listener) {
	return new csd::CSD_PFC(iterator, blocksize, listener);
	//return new csd::CSD_HTFC(iterator, blocksize, listener);
}


std::string FourSectionDictionary::idToString(size_t id, TripleComponentRole position)
{
	csd::CSD *section = getDictionarySection(id, position);

    size_t localid = getLocalId(id, position);

	if(localid<=section->getLength()) {
		const char * ptr = (const char *)section->extract(localid);
		if(ptr!=NULL) {
			const string out(ptr);
			section->freeString((unsigned char*)ptr);
			return out;
		} else {
			//cout << "Not found: " << id << " as " << position << endl;
		}
	}

	return string();
}

size_t FourSectionDictionary::stringToId(const std::string &key, TripleComponentRole position)
{
    size_t ret;

        if(key.length()==0) {
		return 0;
        }

	switch (position) {
	case SUBJECT:
		ret = shared->locate((const unsigned char *)key.c_str(), key.length());
		if( ret != 0) {
			return getGlobalId(ret,SHARED_SUBJECT);
		}
		ret = subjects->locate((const unsigned char *)key.c_str(), key.length());
		if(ret != 0) {
			return getGlobalId(ret,NOT_SHARED_SUBJECT);
		}
        return 0;
	case PREDICATE:
		ret = predicates->locate((const unsigned char *)key.c_str(), key.length());
		if(ret!=0) {
			return getGlobalId(ret, NOT_SHARED_PREDICATE);
		}
        return 0;

	case OBJECT:
		ret = shared->locate((const unsigned char *)key.c_str(), key.length());
		if( ret != 0) {
			return getGlobalId(ret,SHARED_OBJECT);
		}
		ret = objects->locate((const unsigned char *)key.c_str(), key.length());
		if(ret != 0) {
			return getGlobalId(ret,NOT_SHARED_OBJECT);
		}
        return 0;
	}
	return 0;
}


void FourSectionDictionary::load(std::istream & input, ControlInformation & ci, ProgressListener *listener)
{
	std::string format = ci.getFormat();
	if(format!=getType()) {
		throw std::runtime_error("Trying to read a FourSectionDictionary but the data is not FourSectionDictionary");
	}
	//this->mapping = ci.getUint("mapping");
	this->mapping = MAPPING2;
	//this->sizeStrings = ci.getUint("sizeStrings");

	IntermediateListener iListener(listener);

	iListener.setRange(0,25);
	iListener.notifyProgress(0, "Dictionary read shared area.");
	delete shared;
	shared = csd::CSD::load(input);
	if(shared==NULL){
		shared = new csd::CSD_PFC();
		throw std::runtime_error("Could not read shared.");
	}
	//shared = new csd::CSD_Cache(shared);

	iListener.setRange(25,50);
	iListener.notifyProgress(0, "Dictionary read subjects.");
	delete subjects;
	subjects = csd::CSD::load(input);
	if(subjects==NULL){
		subjects = new csd::CSD_PFC();
		throw std::runtime_error("Could not read subjects.");
	}
	//subjects = new csd::CSD_Cache(subjects);

	iListener.setRange(50,75);
	iListener.notifyProgress(0, "Dictionary read predicates.");
	delete predicates;
	predicates = csd::CSD::load(input);
	if(predicates==NULL){
		predicates = new csd::CSD_PFC();
		throw std::runtime_error("Could not read predicates.");
	}
	predicates = new csd::CSD_Cache2(predicates);

	iListener.setRange(75,100);
	iListener.notifyProgress(0, "Dictionary read objects.");
	delete objects;
	objects = csd::CSD::load(input);
	if(objects==NULL){
		objects = new csd::CSD_PFC();
		throw std::runtime_error("Could not read objects.");
	}
	//objects = new csd::CSD_Cache(objects);
}

size_t FourSectionDictionary::load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener)
{
    size_t count=0;

    IntermediateListener iListener(listener);
    ControlInformation ci;
    count += ci.load(&ptr[count], ptrMax);

    //this->mapping = ci.getUint("mapping");
    this->mapping = MAPPING2;
    //this->sizeStrings = ci.getUint("sizeStrings");

    iListener.setRange(0,25);
    iListener.notifyProgress(0, "Dictionary read shared area.");
    delete shared;
    shared = csd::CSD::create(ptr[count]);
    if(shared==NULL){
        shared = new csd::CSD_PFC();
        throw std::runtime_error("Could not read shared.");
    }
    count += shared->load(&ptr[count], ptrMax);
    //shared = new csd::CSD_Cache(shared);

    iListener.setRange(25,50);
    iListener.notifyProgress(0, "Dictionary read subjects.");
    delete subjects;
    subjects = csd::CSD::create(ptr[count]);
    if(subjects==NULL){
        subjects = new csd::CSD_PFC();
        throw std::runtime_error("Could not read subjects.");
    }
    count += subjects->load(&ptr[count], ptrMax);
    //subjects = new csd::CSD_Cache(subjects);

    iListener.setRange(50,75);
    iListener.notifyProgress(0, "Dictionary read predicates.");
    delete predicates;
    predicates = csd::CSD::create(ptr[count]);
    if(predicates==NULL){
        predicates = new csd::CSD_PFC();
        throw std::runtime_error("Could not read predicates.");
    }
    count += predicates->load(&ptr[count], ptrMax);
    predicates = new csd::CSD_Cache2(predicates);

    iListener.setRange(75,100);
    iListener.notifyProgress(0, "Dictionary read objects.");
    delete objects;
    objects = csd::CSD::create(ptr[count]);
    if(objects==NULL){
        objects = new csd::CSD_PFC();
        throw std::runtime_error("Could not read objects.");
    }
    count += objects->load(&ptr[count], ptrMax);
    //objects = new csd::CSD_Cache(objects);

    return count;
}


void FourSectionDictionary::import(Dictionary *other, ProgressListener *listener) {

	try {
		IntermediateListener iListener(listener);

		NOTIFY(listener, "DictionaryPFC loading subjects", 0, 100);
		iListener.setRange(0, 20);
		IteratorUCharString *itSubj = other->getSubjects();
		delete subjects;
		subjects=NULL;
		delete subjects;
		subjects = loadSection(itSubj, blocksize, &iListener);
		delete itSubj;

		NOTIFY(listener, "DictionaryPFC loading predicates", 25, 30);
		iListener.setRange(20, 21);
		IteratorUCharString *itPred = other->getPredicates();
		delete predicates;
		predicates=NULL;
		predicates = loadSection(itPred, blocksize, &iListener);
		delete itPred;

		NOTIFY(listener, "DictionaryPFC loading objects", 30, 90);
		iListener.setRange(21, 90);
		IteratorUCharString *itObj = other->getObjects();
		delete objects;
		objects=NULL;
		objects = loadSection(itObj, blocksize, &iListener);
		delete itObj;

		NOTIFY(listener, "DictionaryPFC loading shared", 90, 100);
		iListener.setRange(90, 100);
		IteratorUCharString *itShared = other->getShared();
		delete shared;
		shared=NULL;
		shared = loadSection(itShared, blocksize, &iListener);
		delete itShared;

		this->sizeStrings = other->size();
		this->mapping = other->getMapping();
	} catch (std::exception& e) {
		delete subjects;
		delete predicates;
		delete objects;
		delete shared;
		subjects = new csd::CSD_PFC();
		predicates = new csd::CSD_PFC();
		objects = new csd::CSD_PFC();
		shared = new csd::CSD_PFC();
		throw;
	}
}

IteratorUCharString *FourSectionDictionary::getSubjects() {
	return subjects->listAll();
}

IteratorUCharString *FourSectionDictionary::getPredicates() {
	return predicates->listAll();
}

IteratorUCharString *FourSectionDictionary::getObjects() {
	return objects->listAll();
}

IteratorUCharString *FourSectionDictionary::getShared() {
	return shared->listAll();
}

void FourSectionDictionary::save(std::ostream & output, ControlInformation & controlInformation, ProgressListener *listener)
{
	controlInformation.setFormat(HDTVocabulary::DICTIONARY_TYPE_FOUR);

	controlInformation.setUint("mapping", this->mapping);
	controlInformation.setUint("sizeStrings", this->sizeStrings);

	controlInformation.save(output);

	IntermediateListener iListener(listener);

	iListener.setRange(0,10);
	iListener.notifyProgress(0, "Dictionary save shared area.");
	shared->save(output);

	iListener.setRange(10,45);
	iListener.notifyProgress(0, "Dictionary save subjects.");
	subjects->save(output);

	iListener.setRange(45,60);
	iListener.notifyProgress(0, "Dictionary save predicates.");
	predicates->save(output);

	iListener.setRange(60,100);
	iListener.notifyProgress(0, "Dictionary save objects.");
	objects->save(output);
}


void FourSectionDictionary::populateHeader(Header & header, string rootNode)
{
	header.insert(rootNode, HDTVocabulary::DICTIONARY_TYPE, getType());
#if 0
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMSUBJECTS, getNsubjects());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMPREDICATES, getNpredicates());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMOBJECTS, getNobjects());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXSUBJECTID, getMaxSubjectID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXPREDICATEID, getMaxPredicateID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXOBJECTTID, getMaxObjectID());
#endif
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMSHARED, getNshared());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAPPING, this->mapping);
	header.insert(rootNode, HDTVocabulary::DICTIONARY_SIZE_STRINGS, size());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_BLOCK_SIZE, this->blocksize);
}

size_t FourSectionDictionary::getNsubjects(){
	return shared->getLength()+subjects->getLength();
}
size_t FourSectionDictionary::getNpredicates(){
	return predicates->getLength();
}
size_t FourSectionDictionary::getNobjects(){
	return shared->getLength()+objects->getLength();
}
size_t FourSectionDictionary::getNobjectsLiterals() {
	throw std::logic_error("Not implemented");
}
size_t FourSectionDictionary::getNobjectsNotLiterals() {
	throw std::logic_error("Not implemented");
}
size_t FourSectionDictionary::getNshared(){
	return shared->getLength();
}


size_t FourSectionDictionary::getMaxID()
{
    size_t s = subjects->getLength();
    size_t o = objects->getLength();
    size_t sh = shared->getLength();
    size_t max = s>o ? s : o;

	if(mapping ==MAPPING2) {
		return sh+max;
	} else {
		return sh+s+o;
	}
}

size_t FourSectionDictionary::getMaxSubjectID()
{
	return getNsubjects();
}

size_t FourSectionDictionary::getMaxPredicateID()
{
	return predicates->getLength();
}

size_t FourSectionDictionary::getMaxObjectID()
{
    size_t s = subjects->getLength();
    size_t o = objects->getLength();
    size_t sh = shared->getLength();

	if(mapping ==MAPPING2) {
		return sh+o;
	} else {
		return sh+s+o;
	}
}

size_t FourSectionDictionary::getNumberOfElements()
{
	return shared->getLength()+subjects->getLength()+predicates->getLength()+objects->getLength();
}

uint64_t FourSectionDictionary::size()
{
	return shared->getSize()+subjects->getSize()+predicates->getSize()+objects->getSize();
}

string FourSectionDictionary::getType()
{
	return HDTVocabulary::DICTIONARY_TYPE_FOUR;
}

size_t FourSectionDictionary::getMapping() {
	return mapping;
}


csd::CSD *FourSectionDictionary::getDictionarySection(size_t id, TripleComponentRole position) {
	switch (position) {
	case SUBJECT:
		if(id<=shared->getLength()) {
			return shared;
		} else {
			return subjects;
		}

	case PREDICATE:
		return predicates;

	case OBJECT:
		if(id<=shared->getLength()) {
			return shared;
		} else {
			return objects;
		}
	}

	throw std::runtime_error("Item not found");
}

size_t FourSectionDictionary::getGlobalId(size_t mapping, size_t id, DictionarySection position) {
	switch (position) {
	case NOT_SHARED_SUBJECT:
		return shared->getLength()+id;

	case NOT_SHARED_PREDICATE:
		return id;

	case NOT_SHARED_OBJECT:
		if(mapping==MAPPING2) {
			return shared->getLength()+id;
		} else {
			return shared->getLength()+subjects->getLength()+id;
		}

	case SHARED_SUBJECT:
	case SHARED_OBJECT:
		return id;
	}

	throw std::runtime_error("Item not found");
}


size_t FourSectionDictionary::getGlobalId(size_t id, DictionarySection position) {
	return getGlobalId(this->mapping, id, position);
}

size_t FourSectionDictionary::getLocalId(size_t mapping, size_t id, TripleComponentRole position) {
	switch (position) {
	case SUBJECT:
		if(id<=shared->getLength()) {
			return id;
		} else {
			return id-shared->getLength();
		}
		break;
	case OBJECT:
		if(id<=shared->getLength()) {
			return id;
		} else {
			if(mapping==MAPPING2) {
				return id-shared->getLength();
			} else {
				return 2+id-shared->getLength()-subjects->getLength();
			}
		}
		break;
	case PREDICATE:
		return id;
	}

	throw std::runtime_error("Item not found");
}

size_t FourSectionDictionary::getLocalId(size_t id, TripleComponentRole position) {
	return getLocalId(mapping,id,position);
}

hdt::IteratorUCharString *FourSectionDictionary::getSuggestions(const char *prefix, TripleComponentRole role){
	if(role==PREDICATE) {
			return predicates->getSuggestions(prefix);
	}

	IteratorUCharString * sharedIt = shared->getSuggestions(prefix);
	IteratorUCharString * subjectIt;
	IteratorUCharString * objectIt;

	// Merge results from shared and subjects/objects keeping order
	if(role==SUBJECT) {
		subjectIt = subjects->getSuggestions(prefix);
		return new MergeIteratorUCharString(sharedIt,subjectIt);
	} else if(role==OBJECT){
		objectIt = objects->getSuggestions(prefix);
		return new MergeIteratorUCharString(sharedIt,objectIt);
	}
	return NULL;

}

hdt::IteratorUInt *FourSectionDictionary::getIDSuggestions(const char *prefix, TripleComponentRole role){
	if(role==PREDICATE) {
			return predicates->getIDSuggestions(prefix);
	}

	IteratorUInt * sharedIt = shared->getIDSuggestions(prefix);
	IteratorUInt * subjectIt;
	IteratorUInt * objectIt;

	// Merge results from shared and subjects/objects keeping order
	if(role==SUBJECT) {
		subjectIt = subjects->getIDSuggestions(prefix);
		return new SequentialIteratorUInt(sharedIt,subjectIt,shared->getLength());
	} else if(role==OBJECT){
		objectIt = objects->getIDSuggestions(prefix);
		return new SequentialIteratorUInt(sharedIt,objectIt,shared->getLength());
	}
	return NULL;

}
void FourSectionDictionary::getSuggestions(const char *base, hdt::TripleComponentRole role, std::vector<std::string> &out, int maxResults)
{
	if(role==PREDICATE) {
		predicates->fillSuggestions(base, out, maxResults);
		return;
	}

	vector<string> v1,v2;
	shared->fillSuggestions(base, v1, maxResults);
	if(role==SUBJECT) {
		subjects->fillSuggestions(base, v2, maxResults);
	} else if(role==OBJECT){
		objects->fillSuggestions(base, v2, maxResults);
	}

	// Merge results from shared and subjects/objects keeping order
	merge(v1.begin(),v1.end(), v2.begin(), v2.end(), std::back_inserter(out));

	// Remove possible extra items
	if((maxResults>=0) && (out.size()>static_cast<size_t>(maxResults))) {
		out.resize(maxResults);
	}
}

}
