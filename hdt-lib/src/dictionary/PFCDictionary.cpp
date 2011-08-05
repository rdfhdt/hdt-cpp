/*
 * File: PFCDictionary.cpp
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




#include "PFCDictionary.hpp"
#include <HDTVocabulary.hpp>
#include "../libdcs/CSD_PFC.h"

namespace hdt {

class DictIterator : public csd::IteratorUCharString {
private:
	std::vector<DictionaryEntry *> &vector;
	unsigned int pos;
public:
	DictIterator(std::vector<DictionaryEntry *> &vector) : vector(vector), pos(0){

	}

	virtual bool hasNext() {
		return pos<vector.size();
	}

	virtual unsigned char *next() {
		return (unsigned char*)vector[pos++]->str->c_str();
	}
};

PFCDictionary::PFCDictionary() : blocksize(32)
{
	subjects = new csd::CSD_PFC();
	predicates = new csd::CSD_PFC();
	objects = new csd::CSD_PFC();
	shared = new csd::CSD_PFC();
}

PFCDictionary::PFCDictionary(HDTSpecification & spec) : blocksize(32)
{
	subjects = new csd::CSD_PFC();
	predicates = new csd::CSD_PFC();
	objects = new csd::CSD_PFC();
	shared = new csd::CSD_PFC();

	string blockSizeStr = spec.get("dict.block.size");
	if(blockSizeStr!=""){
		blocksize = atoi(blockSizeStr.c_str());
	}
}

PFCDictionary::~PFCDictionary()
{
	delete subjects;
	delete predicates;
	delete objects;
	delete shared;
}

void PFCDictionary::import(PlainDictionary *dictionary, ProgressListener *listener)
{
	delete subjects;
	delete predicates;
	delete objects;
	delete shared;

	DictIterator itSubj(dictionary->subjects_not_shared);
	DictIterator itPred(dictionary->predicates);
	DictIterator itObj(dictionary->objects_not_shared);
	DictIterator itShared(dictionary->subjects_shared);

	NOTIFY(listener, "DictionaryPFC loading subjects", 0, 100);
	subjects = new csd::CSD_PFC(&itSubj, blocksize);

	NOTIFY(listener, "DictionaryPFC loading predicates", 25, 100);
	predicates = new csd::CSD_PFC(&itPred, blocksize);

	NOTIFY(listener, "DictionaryPFC loading objects", 50, 100);
	objects = new csd::CSD_PFC(&itObj, blocksize);

	NOTIFY(listener, "DictionaryPFC loading shared", 75, 100);
	shared = new csd::CSD_PFC(&itShared, blocksize);

	this->sizeStrings = dictionary->sizeStrings;
	this->mapping = dictionary->mapping;

#if 0
	cout << "Shared: " << shared->getLength() << endl;
	cout << "Subjects: " << subjects->getLength() << endl;
	cout << "Predicates: " << predicates->getLength() << endl;
	cout << "Objects: " << objects->getLength() << endl;

	cout << "Ensure same: " << endl;
	for(unsigned int i=1;i<getMaxObjectID();i++){
		string str1 = dictionary->idToString(i, OBJECT);
		string str2 = this->idToString(i, OBJECT);
		unsigned int id1 = dictionary->stringToId(str1, OBJECT);
		unsigned int id2 = this->stringToId(str1, OBJECT);

		if( (str1!=str2) || (id1!=id2)) {
			cout << i << " Objects difer: " << endl;
			cout << "\tPlain: " << id1 << " => " << str1 << endl;
			cout << "\tPFC__: " << id2 << " => " << str2 << endl;
		}
	}
#endif
 }


std::string PFCDictionary::idToString(unsigned int id, TripleComponentRole position)
{
	csd::CSD *section = getDictionarySection(id, position);

	unsigned int localid = getLocalId(id, position);

	if(localid<=section->getLength()) {
		const char * ptr = (const char *)section->extract(localid);
		if(ptr!=NULL) {
			string out = ptr;
			delete ptr;
			return out;
		} else {
			//cout << "Not found: " << id << " as " << position << endl;
		}
	}

	return string();
}

unsigned int PFCDictionary::stringToId(std::string &key, TripleComponentRole position)
{
	unsigned int ret;

	if(key=="")
		return 0;

	switch (position) {
	case SUBJECT:
		ret = shared->locate((const uchar *)key.c_str(), key.length());
		if( ret != 0) {
			return getGlobalId(ret,SHARED_SUBJECT);
		}
		ret = subjects->locate((const uchar *)key.c_str(), key.length());
		if(ret != 0) {
			return getGlobalId(ret,NOT_SHARED_SUBJECT);
		}
		throw "Subject not found in dictionary";
	case PREDICATE:
		ret = predicates->locate((const uchar *)key.c_str(), key.length());
		if(ret!=0) {
			return getGlobalId(ret, NOT_SHARED_PREDICATE);
		}
		throw "Predicate not found in dictionary";

	case OBJECT:
		ret = shared->locate((const uchar *)key.c_str(), key.length());
		if( ret != 0) {
			return getGlobalId(ret,SHARED_OBJECT);
		}
		ret = objects->locate((const uchar *)key.c_str(), key.length());
		if(ret != 0) {
			return getGlobalId(ret,NOT_SHARED_OBJECT);
		}
		throw "Object not found in dictionary";
	}
}


void PFCDictionary::load(std::istream & input, ControlInformation & ci, ProgressListener *listener)
{
	delete subjects;
	delete predicates;
	delete objects;
	delete shared;

	this->mapping = ci.getUint("$mapping");
	this->sizeStrings = ci.getUint("$sizeStrings");

	ifstream *in = dynamic_cast<ifstream *>(&input);

	IntermediateListener iListener(listener);

	iListener.setRange(0,25);
	iListener.notifyProgress(0, "Dictionary read shared area.");
	//cout << "Load shared " << in->tellg() << endl;
	shared = csd::CSD::load(*in);

	iListener.setRange(25,50);
	iListener.notifyProgress(0, "Dictionary read subjects.");
	//cout << "Load subjects " << in->tellg() << endl;
	subjects = csd::CSD::load(*in);

	iListener.setRange(50,75);
	iListener.notifyProgress(0, "Dictionary read predicates.");
	//cout << "Load predicates " << in->tellg() << endl;
	predicates = csd::CSD::load(*in);

	iListener.setRange(75,100);
	iListener.notifyProgress(0, "Dictionary read objects.");
	//cout << "Load objects " << in->tellg() << endl;
	objects = csd::CSD::load(*in);

	//cout << "Dictionary loaded " << in->tellg() << endl;
}

bool PFCDictionary::save(std::ostream & output, ControlInformation & controlInformation, ProgressListener *listener)
{
	controlInformation.set("codification", HDTVocabulary::DICTIONARY_TYPE_PFC);
	controlInformation.set("format", "text/plain");
	controlInformation.setUint("$elements", getNumberOfElements());

	controlInformation.setUint("$subjects", getNsubjects());
	controlInformation.setUint("$objects", getNobjects());
	controlInformation.setUint("$predicates", getNpredicates());
	controlInformation.setUint("$sharedso", getSsubobj());

	controlInformation.setUint("$maxid", getMaxID());
	controlInformation.setUint("$maxsubjectid",getMaxSubjectID());
	controlInformation.setUint("$maxpredicateid", getMaxPredicateID());
	controlInformation.setUint("$maxobjectid", getMaxObjectID());

	controlInformation.setUint("$mapping", this->mapping);
	controlInformation.setUint("$sizeStrings", this->sizeStrings);
	controlInformation.setUint("$blockSize", this->blocksize);

	controlInformation.save(output);

	ofstream *out = dynamic_cast<ofstream *>(&output);
	IntermediateListener iListener(listener);

	iListener.setRange(0,10);
	iListener.notifyProgress(0, "Dictionary save shared area.");
	//cout << "Save shared " << out->tellp() << endl;
	shared->save(*out);

	iListener.setRange(10,45);
	iListener.notifyProgress(0, "Dictionary save subjects.");
	//cout << "Save subjects " << out->tellp() << endl;
	subjects->save(*out);

	iListener.setRange(45,60);
	iListener.notifyProgress(0, "Dictionary save predicates.");
	//cout << "Save predicates " << out->tellp() << endl;
	predicates->save(*out);

	iListener.setRange(60,100);
	iListener.notifyProgress(0, "Dictionary save objects.");
	//cout << "Save objects " << out->tellp() << endl;
	objects->save(*out);

	//cout << "Dictionary saved " << out->tellp() << endl;
}


void PFCDictionary::populateHeader(Header & header, string rootNode)
{
	header.insert(rootNode, HDTVocabulary::DICTIONARY_TYPE, HDTVocabulary::DICTIONARY_TYPE_PFC);
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMSUBJECTS, getNsubjects());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMPREDICATES, getNpredicates());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMOBJECTS, getNobjects());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMSHARED, getSsubobj());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXSUBJECTID, getMaxSubjectID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXPREDICATEID, getMaxPredicateID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXOBJECTTID, getMaxObjectID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAPPING, this->mapping);
	header.insert(rootNode, HDTVocabulary::DICTIONARY_SIZE_STRINGS, size());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_BLOCK_SIZE, this->blocksize);
}

unsigned int PFCDictionary::getNsubjects(){
	return shared->getLength()+subjects->getLength();
}
unsigned int PFCDictionary::getNpredicates(){
	return predicates->getLength();
}
unsigned int PFCDictionary::getNobjects(){
	return shared->getLength()+objects->getLength();
}
unsigned int PFCDictionary::getSsubobj(){
	return shared->getLength();
}


unsigned int PFCDictionary::getMaxID()
{
	unsigned int s = subjects->getLength();
	unsigned int o = objects->getLength();
	unsigned int sh = shared->getLength();
	unsigned int max = s>o ? s : o;

	if(mapping ==MAPPING2) {
		return sh+max;
	} else {
		return sh+s+o;
	}
}

unsigned int PFCDictionary::getMaxSubjectID()
{
	return getNsubjects();
}

unsigned int PFCDictionary::getMaxPredicateID()
{
	return predicates->getLength();
}

unsigned int PFCDictionary::getMaxObjectID()
{
	unsigned int s = subjects->getLength();
	unsigned int o = objects->getLength();
	unsigned int sh = shared->getLength();

	if(mapping ==MAPPING2) {
		return sh+o;
	} else {
		return sh+s+o;
	}
}

unsigned int PFCDictionary::getNumberOfElements()
{
	return shared->getLength()+subjects->getLength()+predicates->getLength()+objects->getLength();
}

unsigned int PFCDictionary::size()
{
	return shared->getSize()+subjects->getSize()+predicates->getSize()+objects->getSize();
}

void PFCDictionary::startProcessing(ProgressListener *listener){
}

void PFCDictionary::stopProcessing(ProgressListener *listener){
	cout << "************ SHARED ***********" << endl;
	shared->dumpAll();

	cout << "************ SUBJECTS ***********" << endl;
	subjects->dumpAll();

	cout << "************ PREDS ***********" << endl;
	predicates->dumpAll();

	cout << "************ OBJS ***********" << endl;
	objects->dumpAll();
}

unsigned int PFCDictionary::insert(std::string & str, TripleComponentRole position)
{
	throw "This dictionary does not support insertions.";
}

string PFCDictionary::getType()
{
	return HDTVocabulary::DICTIONARY_TYPE_PFC;
}


csd::CSD *PFCDictionary::getDictionarySection(unsigned int id, TripleComponentRole position) {
	switch (position) {
	case SUBJECT:
		if(id<=shared->getLength()) {
			//cout << "Section SHARED" << endl;
			return shared;
		} else {
			//cout << "Section SUBJECTS" << endl;
			return subjects;
		}

	case PREDICATE:
		return predicates;

	case OBJECT:
		if(id<=shared->getLength()) {
			//cout << "Section SHARED" << endl;
			return shared;
		} else {
			//cout << "Section OBJECTS" << endl;
			return objects;
		}
	}

	throw "Item not found";
}

unsigned int PFCDictionary::getGlobalId(unsigned int mapping, unsigned int id, DictionarySection position) {
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

		throw "Item not found";
}


unsigned int PFCDictionary::getGlobalId(unsigned int id, DictionarySection position) {
	return getGlobalId(this->mapping, id, position);
}

unsigned int PFCDictionary::getLocalId(unsigned int mapping, unsigned int id, TripleComponentRole position) {
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

		throw "Item not found";
}

unsigned int PFCDictionary::getLocalId(unsigned int id, TripleComponentRole position) {
	return getLocalId(mapping,id,position);
}




}


