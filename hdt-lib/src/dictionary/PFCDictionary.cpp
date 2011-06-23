	/*
 * PFCDictionary.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
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
}

PFCDictionary::PFCDictionary(HDTSpecification & spec) : blocksize(32)
{
	string blockSizeStr = spec.get("dict.block.size");
	if(blockSizeStr!=""){
		blocksize = atoi(blockSizeStr.c_str());
	}
}

PFCDictionary::~PFCDictionary()
{
}

void PFCDictionary::import(PlainDictionary *dictionary)
{
	DictIterator itSubj(dictionary->subjects_not_shared);
	DictIterator itPred(dictionary->predicates);
	DictIterator itObj(dictionary->objects_not_shared);
	DictIterator itShared(dictionary->subjects_shared);

	subjects = new csd::CSD_PFC(&itSubj, blocksize);
	predicates = new csd::CSD_PFC(&itPred, blocksize);
	objects = new csd::CSD_PFC(&itObj, blocksize);
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
			cout << "\tPFC: " << id2 << " => " << str2 << endl;
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
	this->mapping = ci.getUint("$mapping");
	this->sizeStrings = ci.getUint("$sizeStrings");

	ifstream *in = dynamic_cast<ifstream *>(&input);

	shared = csd::CSD::load(*in);
	subjects = csd::CSD::load(*in);
	predicates = csd::CSD::load(*in);
	objects = csd::CSD::load(*in);
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
	shared->save(*out);
	subjects->save(*out);
	predicates->save(*out);
	objects->save(*out);
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

void PFCDictionary::startProcessing(){
}

void PFCDictionary::stopProcessing(){
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


