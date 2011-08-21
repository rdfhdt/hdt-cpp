/*
 * File: PlainDictionary.cpp
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

#include <sstream>

#include "PlainDictionary.hpp"
#include "../util/Histogram.h"

#include <HDTVocabulary.hpp>

namespace hdt {

/* DICTIONARY ENTRY */

string DictionaryEntry::toString() {
	string tmp;
	tmp.append(*prefix);
	tmp.append(*str);
	return tmp;
}

bool DictionaryEntry::cmpLexicographic(DictionaryEntry *c1, DictionaryEntry *c2) {
	return c1->toString().compare(c2->toString())<0;
}

bool DictionaryEntry::cmpID(DictionaryEntry *c1, DictionaryEntry *c2) {
	return c1->id>c2->id;
}


PlainDictionary::PlainDictionary() {
	this->mapping = MAPPING2;
}

PlainDictionary::PlainDictionary(HDTSpecification &specification) : spec(specification) {
	if(spec.get("dictionary.mapping")=="mapping1") {
		this->mapping = MAPPING1;
	} else {
		this->mapping = MAPPING2;
	}
}

PlainDictionary::~PlainDictionary() {
	unsigned int i;

	for(i=0;i<subjects_shared.size();i++) {
		delete subjects_shared[i];
	}

	for(i=0;i<subjects_not_shared.size();i++) {
		delete subjects_not_shared[i];
	}

	for(i=0;i<objects_not_shared.size();i++) {
		delete objects_not_shared[i];
	}

	for(i=0;i<predicates.size();i++) {
		delete predicates[i];
	}

}

std::string PlainDictionary::idToString(unsigned int id, TripleComponentRole position)
{
	vector<DictionaryEntry*> &vector = getDictionaryEntryVector(id, position);

	unsigned int localid = getLocalId(id, position);

	if(localid<vector.size()) {
		DictionaryEntry *entry = vector[localid];
		return entry->toString();
	}

	return string();
}

unsigned int PlainDictionary::stringToId(std::string &key, TripleComponentRole position)
{
	DictEntryIt ret;

	if(key=="")
		return 0;

	switch (position) {
	case SUBJECT:
		ret = hashSubject.find(key.c_str());
		if(ret!=hashSubject.end())
			return ret->second->id;
		else
			throw "Subject not found in dictionary";
	case PREDICATE:
		ret = hashPredicate.find(key.c_str());
		if (ret != hashPredicate.end())
			return ret->second->id;
		else
			throw "Predicate not found in dictionary";
	case OBJECT:
		ret = hashObject.find(key.c_str());
		if (ret != hashObject.end())
			return ret->second->id;
		else
			throw "Object not found in dictionary";
	}
}

void PlainDictionary::startProcessing(ProgressListener *listener)
{
	subjects_not_shared.clear();
	subjects_shared.clear();
	objects_not_shared.clear();
	predicates.clear();

	hashSubject.clear(); //hashSubject.resize(nsubjects);
	hashObject.clear(); //hashObject.resize(nobjects);
	hashPredicate.clear(); //hashPredicate.resize(npredicates);
	prefixes.clear();
}

void PlainDictionary::stopProcessing(ProgressListener *listener)
{
	this->split(listener);
	//std::cout << "Splitted" << std::endl;
	this->lexicographicSort(listener);
	//std::cout << "Sorted" << std::endl;
	//dumpSizes(cout);
}


void PlainDictionary::save(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener)
{
	controlInformation.set("codification", HDTVocabulary::DICTIONARY_TYPE_PLAIN);
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

	controlInformation.save(output);

	unsigned int i = 0;
	unsigned int counter=0;
	const char marker = '\n';

	//shared subjects-objects from subjects
	for (i = 0; i < subjects_shared.size(); i++) {
		output << *subjects_shared[i]->prefix;
		output << *subjects_shared[i]->str;
		output.put(marker); //character to split file
		counter++;
		NOTIFYCOND(listener, "PlainDictionary saving shared", counter, getNumberOfElements());
	}

	output.put(marker); //extra line to set the begining of next part of dictionary

	//not shared subjects
	for (i = 0; i < subjects_not_shared.size(); i++) {
		output << *subjects_not_shared[i]->prefix;
		output << *subjects_not_shared[i]->str;
		output.put(marker); //character to split file
		counter++;
		NOTIFYCOND(listener, "PlainDictionary saving subjects", counter, getNumberOfElements());
	}

	output.put(marker); //extra line to set the begining of next part of dictionary

	//not shared objects
	for (i = 0; i < objects_not_shared.size(); i++) {
		output << *objects_not_shared[i]->prefix;
		output << *objects_not_shared[i]->str;
		output.put(marker); //character to split file
		counter++;
		NOTIFYCOND(listener, "PlainDictionary saving objects", counter, getNumberOfElements());
	}

	output.put(marker); //extra line to set the begining of next part of dictionary

	//predicates
	for (i = 0; i < predicates.size(); i++) {
		output << *predicates[i]->prefix;
		output << *predicates[i]->str;
		output.put(marker); //character  to split file
		counter++;
		NOTIFYCOND(listener, "PlainDictionary saving predicates", counter, getNumberOfElements());
	}

	output.put(marker);
}

void PlainDictionary::load(std::istream & input, ControlInformation &ci, ProgressListener *listener)
{
	std::string line;
	unsigned char region = 1;

	startProcessing();

	this->mapping = ci.getUint("$mapping");
	this->sizeStrings = ci.getUint("$sizeStrings");
	unsigned int numElements = ci.getUint("$elements");
	unsigned int numLine = 0;

	IntermediateListener iListener(listener);
	iListener.setRange(0,25);
	iListener.notifyProgress(0, "Dictionary save shared area.");
	while(region<5 && getline(input, line)) {
		//std::cout << line << std::endl;

		if(line!="") {
			if (region == 1) { //shared SO
				NOTIFYCOND(&iListener, "Dictionary loading shared area.", numLine, numElements);
				insert(line, SHARED_SUBJECT);
			} else if (region == 2) { //not shared Subjects
				NOTIFYCOND(&iListener, "Dictionary loading subjects.", numLine, numElements);
				insert(line, NOT_SHARED_SUBJECT);
				NOTIFYCOND(&iListener, "Dictionary loading objects.", numLine, numElements);
			} else if (region == 3) { //not shared Objects
				insert(line, NOT_SHARED_OBJECT);
				NOTIFYCOND(&iListener, "Dictionary loading predicates.", numLine, numElements);
			} else if (region == 4) { //predicates
				insert(line, NOT_SHARED_PREDICATE);
			}
		} else {
			region++;
		}

		numLine++;
	}

	// No stopProcessing() Needed. Dictionary already split and sorted in file.
	updateIDs();
}

unsigned int PlainDictionary::getNumberOfElements()
{
	return subjects_shared.size() + subjects_not_shared.size() + objects_not_shared.size() + predicates.size();
}

unsigned int PlainDictionary::size()
{
    return sizeStrings;
}


unsigned int PlainDictionary::insert(std::string & str, TripleComponentRole pos)
{
	if(str=="") return 0;

	if(pos==PREDICATE) {
		DictEntryIt it = hashPredicate.find(str.c_str());
		if(it!=hashPredicate.end()) {
			//cout << "  existing predicate: " << str << endl;
			return it->second->id;
		} else {
			DictionaryEntry *entry = new DictionaryEntry;
			setPrefixAndString(entry, str);
			entry->id = predicates.size()+1;
			sizeStrings += str.length();
			//cout << " Add new predicate: " << str.c_str() << endl;

			hashPredicate[entry->str->c_str()] = entry;
			predicates.push_back(entry);
			return entry->id;
		}
	}

	DictEntryIt subjectIt = hashSubject.find(str.c_str());
	DictEntryIt objectIt = hashObject.find(str.c_str());

	bool foundSubject = subjectIt!=hashSubject.end();
	bool foundObject = objectIt!=hashObject.end();
	//cout << "A: " << foundSubject << " B: " << foundSubject << endl;

	if(pos==SUBJECT) {
		if( !foundSubject && !foundObject) {
			// Did not exist, create new.
			DictionaryEntry *entry = new DictionaryEntry;
			setPrefixAndString(entry, str);
			sizeStrings += str.length();

			//cout << " Add new subject: " << str << endl;
			hashSubject[entry->str->c_str()] = entry;
		} else if(foundSubject) {
			// Already exists in subjects.
			//cout << "   existing subject: " << str << endl;
		} else if(foundObject) {
			// Already exists in objects.
			//cout << "   existing subject as object: " << str << endl;
			hashSubject[objectIt->second->str->c_str()] = objectIt->second;
		}
	} else if(pos==OBJECT) {
		if(!foundSubject && !foundObject) {
			// Did not exist, create new.
			DictionaryEntry *entry = new DictionaryEntry;
			setPrefixAndString(entry, str);
			sizeStrings += str.length();

			//cout << " Add new object: " << str << endl;
			hashObject[entry->str->c_str()] = entry;
		} else if(foundObject) {
			// Already exists in objects.
			//cout << "     existing object: " << str << endl;
		} else if(foundSubject) {
			// Already exists in subjects.
			//cout << "     existing object as subject: " << str << endl;
			hashObject[subjectIt->second->str->c_str()] = subjectIt->second;
		}
	}

	// FIXME: Return inserted index?
	return 0;
}

string intToStr(int val) {
	std::stringstream out;
	out << val;
	return out.str();
}


// PRIVATE

void PlainDictionary::insert(string str, DictionarySection pos) {

	if(str=="") return;

	DictionaryEntry *entry = new DictionaryEntry;
	setPrefixAndString(entry, str);

	switch(pos) {
	case SHARED_SUBJECT:
	case SHARED_OBJECT:
		subjects_shared.push_back(entry);
		//entry->id = subjects_shared.size();
		hashSubject[entry->str->c_str()] = entry;
		hashObject[entry->str->c_str()] = entry;
		break;
	case NOT_SHARED_SUBJECT:
		subjects_not_shared.push_back(entry);
		//entry->id = subjects_shared.size()+subjects_not_shared.size();
		hashSubject[entry->str->c_str()] = entry;
		break;
	case NOT_SHARED_OBJECT:
		objects_not_shared.push_back(entry);
		//entry->id = subjects_shared.size()+objects_not_shared.size();
		hashObject[entry->str->c_str()] = entry;
		break;
	case NOT_SHARED_PREDICATE:
		predicates.push_back(entry);
		//entry->id = predicates.size();
		hashPredicate[entry->str->c_str()] = entry;
	}
}

/** Split
 * @return void
 */
void PlainDictionary::split(ProgressListener *listener) {
	subjects_not_shared.clear();
	subjects_shared.clear();
	objects_not_shared.clear();

	for(DictEntryIt subj_it = hashSubject.begin(); subj_it!=hashSubject.end() && subj_it->first; subj_it++) {
		//cout << "Check Subj: " << subj_it->first << endl;
		DictEntryIt other = hashObject.find(subj_it->first);

		if(other==hashObject.end()) {
			// Only subject
			subjects_not_shared.push_back(subj_it->second);
		} else {
			// Exist in both
			subjects_shared.push_back(subj_it->second);
		}
	}

	for(DictEntryIt obj_it = hashObject.begin(); obj_it!=hashObject.end(); ++obj_it) {
		//cout << "Check Obj: " << obj_it->first << endl;
		DictEntryIt other = hashSubject.find(obj_it->first);

		if(other==hashSubject.end()) {
			// Only object
			objects_not_shared.push_back(obj_it->second);
		}
	}
}

/** Lexicographic Sort
 * @param mapping Description of the param.
 * @return void
 */
void PlainDictionary::lexicographicSort(ProgressListener *listener) {

	//sort shared and not shared subjects
	sort(subjects_shared.begin(), subjects_shared.end(), DictionaryEntry::cmpLexicographic);
	sort(subjects_not_shared.begin(), subjects_not_shared.end(), DictionaryEntry::cmpLexicographic);

	//sort not shared objects
	sort(objects_not_shared.begin(), objects_not_shared.end(), DictionaryEntry::cmpLexicographic);

	//sort predicates
	sort(predicates.begin(), predicates.end(), DictionaryEntry::cmpLexicographic);

	updateIDs();
}

void PlainDictionary::idSort() {
	//sort shared and not shared subjects
	sort(subjects_shared.begin(), subjects_shared.end(), DictionaryEntry::cmpID);
	sort(subjects_not_shared.begin(), subjects_not_shared.end(), DictionaryEntry::cmpID);

	//sort not shared objects
	sort(objects_not_shared.begin(), objects_not_shared.end(), DictionaryEntry::cmpID);

	//sort predicates
	sort(predicates.begin(), predicates.end(), DictionaryEntry::cmpID);

	updateIDs();
}


/** Map
 * @param mapping Description of the param.
 * @return void
 */
void PlainDictionary::updateIDs() {
	unsigned int i;

	for (i = 0; i < subjects_shared.size(); i++) {
		subjects_shared[i]->id = getGlobalId(i, SHARED_SUBJECT);
	}

	for (i = 0; i < subjects_not_shared.size(); i++) {
		subjects_not_shared[i]->id = getGlobalId(i, NOT_SHARED_SUBJECT);
	}

	for (i = 0; i < objects_not_shared.size(); i++) {
		objects_not_shared[i]->id = getGlobalId(i, NOT_SHARED_OBJECT);
	}

	for (i = 0; i < predicates.size(); i++) {
		predicates[i]->id = getGlobalId(i, NOT_SHARED_PREDICATE);
	}
}


void PlainDictionary::convertMapping(unsigned int mapping) {
	switch(mapping) {
	case MAPPING1:
		this->mapping = mapping;
		updateIDs();
		break;
	case MAPPING2:
		this->mapping = mapping;
		updateIDs();
		break;
	default:
		break;
	}
}

void PlainDictionary::setPrefixAndString(DictionaryEntry *entry, const string str) {
	static string empty("");

/*	size_t pos = str.find_last_of('/');
	if(pos!=string::npos) {
		string prefix = str.substr(0, pos+1);

		PrefixIt prefixIt = prefixes.find(prefix.c_str());

		string *foundPrefix;

		if(prefixIt==prefixes.end()) {
			foundPrefix = new string(prefix);
			prefixes[prefix.c_str()] = foundPrefix;
		} else {
			foundPrefix = prefixIt->second;
		}

		entry->prefix = foundPrefix;
		entry->str = new string(str.substr(pos+1));
	} else {*/
		entry->prefix = &empty;
		entry->str = new string(str);
	//}

	//cout<<"Converted: "<< str << " to |" << *entry->prefix << "|" << *entry->str << "|" << endl;
}

vector<DictionaryEntry*> &PlainDictionary::getDictionaryEntryVector(unsigned int id, TripleComponentRole position) {
	switch (position) {
	case SUBJECT:
		if(id<= subjects_shared.size()) {
			return subjects_shared;
		} else {
			return subjects_not_shared;
		}

	case PREDICATE:
		return predicates;

	case OBJECT:
		if(id<= subjects_shared.size()) {
			return subjects_shared;
		} else {
			return objects_not_shared;
		}
	}

	throw "Item not found";
}

unsigned int PlainDictionary::getGlobalId(unsigned int mapping, unsigned int id, DictionarySection position) {
	switch (position) {
		case NOT_SHARED_SUBJECT:
			return subjects_shared.size()+id+1;

		case NOT_SHARED_PREDICATE:
			return id+1;

		case NOT_SHARED_OBJECT:
			if(mapping==MAPPING2) {
				return subjects_shared.size()+id+1;
			} else {
				return subjects_shared.size()+subjects_not_shared.size()+id+1;
			}

		case SHARED_SUBJECT:
		case SHARED_OBJECT:
			return id+1;
		}

		throw "Item not found";
}


unsigned int PlainDictionary::getGlobalId(unsigned int id, DictionarySection position) {
	return getGlobalId(this->mapping, id, position);
}

unsigned int PlainDictionary::getLocalId(unsigned int mapping, unsigned int id, TripleComponentRole position) {
	switch (position) {
		case SUBJECT:
			if(id<=subjects_shared.size()) {
				return id-1;
			} else {
				return id-subjects_shared.size()-1;
			}
			break;
		case OBJECT:
			if(id<=subjects_shared.size()) {
				return id-1;
			} else {
				if(mapping==MAPPING2) {
					return id-subjects_shared.size()-1;
				} else {
					return id-subjects_shared.size()-subjects_not_shared.size()-1;
				}
			}
			break;
		case PREDICATE:
			return id-1;
		}

		throw "Item not found";
}

unsigned int PlainDictionary::getLocalId(unsigned int id, TripleComponentRole position) {
	return getLocalId(mapping,id,position);
}



/** Get Max Id
 * @return The expected result
 */
unsigned int PlainDictionary::getMaxID() {
	unsigned int s = subjects_not_shared.size();
	unsigned int o = objects_not_shared.size();
	unsigned int shared = subjects_shared.size();
	unsigned int max = s>o ? s : o;

	if(mapping ==MAPPING2) {
		return shared+max;
	} else {
		return shared+s+o;
	}
}

unsigned int PlainDictionary::getMaxSubjectID() {
	unsigned int shared = subjects_shared.size();
	unsigned int s = subjects_not_shared.size();

	return shared+s;
}

unsigned int PlainDictionary::getMaxPredicateID() {
	return predicates.size();
}

unsigned int PlainDictionary::getMaxObjectID() {
	unsigned int shared = subjects_shared.size();
	unsigned int s = subjects_not_shared.size();
	unsigned int o = objects_not_shared.size();

	if(mapping ==MAPPING2) {
		return shared+o;
	} else {
		return shared+s+o;
	}
}

/** Get Mapping
 * @return The expected result
 */
unsigned int PlainDictionary::getMapping() {
	return mapping;
}

/** Get N Subjects
 * @return The expected result
 */
unsigned int PlainDictionary::getNsubjects() {
	return subjects_shared.size()+subjects_not_shared.size();
}

/** Get N Predicates
 * @return The expected result
 */
unsigned int PlainDictionary::getNpredicates() {
	return predicates.size();
}

/** Get N Objects
 * @return The expected result
 */
unsigned int PlainDictionary::getNobjects() {
	return subjects_shared.size()+objects_not_shared.size();
}

/** Get S Subobj
 * @return The expected result
 */
unsigned int PlainDictionary::getSsubobj() {
	return subjects_shared.size();
}



void PlainDictionary::updateID(unsigned int oldid, unsigned int newid, DictionarySection position) {
	switch (position) {
	case SHARED_SUBJECT:
	case SHARED_OBJECT:
			subjects_shared[oldid]->id = newid;
			break;
	case NOT_SHARED_SUBJECT:
		subjects_not_shared[oldid]->id = newid;
		break;
	case NOT_SHARED_PREDICATE:
		predicates[oldid]->id = newid;
		break;
	case NOT_SHARED_OBJECT:
		objects_not_shared[oldid]->id = newid;
		break;
	}
}

void PlainDictionary::populateHeader(Header &header, string rootNode)
{
	header.insert(rootNode, HDTVocabulary::DICTIONARY_TYPE, HDTVocabulary::DICTIONARY_TYPE_PLAIN);
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMSUBJECTS, getNsubjects());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMPREDICATES, getNpredicates());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMOBJECTS, getNobjects());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMSHARED, getSsubobj());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXSUBJECTID, getMaxSubjectID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXPREDICATEID, getMaxPredicateID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXOBJECTTID, getMaxObjectID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAPPING, getMapping());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_SIZE_STRINGS, size());
}

void PlainDictionary::dumpSizes(ostream &out) {
	cout << "\n\t [Dictionary stats:\n";
	cout << "\t   shared subjects-objects:" << getSsubobj() << "\n";
	cout << "\t   not shared subjects:" << getNsubjects() - getSsubobj() << "\n";
	cout << "\t   not shared objects:" << getNobjects() - getSsubobj() << "\n";
	cout << "\t total subjects:" << getNsubjects() << "\n";
	cout << "\t total objects:" << getNobjects() << "\n";
	cout << "\t total predicates:" << getNpredicates() << " ]\n\n";
}

/** Dump Stats
 * @param output Description of the param.
 * @return void
 */
void PlainDictionary::dumpStats(string &output) {
	unsigned int i = 0;

	const int maxval = 50000;
	const int nbins = 50000;

	Histogram histoURI(0, maxval, nbins);
	Histogram histoLiteral(0, maxval, nbins);
	Histogram histoBlank(0, maxval, nbins);

	string tmp;
	//shared subjects-objects from subjects
	for (i = 0; i < subjects_shared.size(); i++) {
		tmp.clear();

		tmp.append(*subjects_shared[i]->prefix);
		tmp.append(*subjects_shared[i]->str);

		if (tmp[0] == '<') {
			histoURI.Add(tmp.length());
		} else if (tmp[0] == '"') {
			histoLiteral.Add(tmp.length());
		} else if (tmp[0] == '_') {
			histoBlank.Add(tmp.length());
		} else {
			cout << "String not URI/Lit?: " << tmp << endl;
		}
	}

	//not shared subjects
	for (i = 0; i < subjects_not_shared.size(); i++) {
		tmp.clear();
		tmp.append(*subjects_not_shared[i]->prefix);
		tmp.append(*subjects_not_shared[i]->str);

		if (tmp[0] == '<') {
			histoURI.Add(tmp.length());
		} else if (tmp[0] == '"') {
			histoLiteral.Add(tmp.length());
		} else if (tmp[0] == '_') {
			histoBlank.Add(tmp.length());
		} else {
			cout << "String not URI/Lit?: " << tmp << endl;
		}
	}

	//not shared objects
	for (i = 0; i < objects_not_shared.size(); i++) {
		tmp.clear();
		tmp.append(*objects_not_shared[i]->prefix);
		tmp.append(*objects_not_shared[i]->str);

		if (tmp[0] == '<') {
			histoURI.Add(tmp.length());
		} else if (tmp[0] == '"') {
			histoLiteral.Add(tmp.length());
		} else if (tmp[0] == '_') {
			histoBlank.Add(tmp.length());
		} else {
			cout << "String not URI/Lit?: " << tmp << endl;
		}
	}

	//predicates
	for (i = 0; i < predicates.size(); i++) {
		tmp.clear();
		tmp.append(*predicates[i]->prefix);
		tmp.append(*predicates[i]->str);

		if (tmp[0] == '<') {
			histoURI.Add(tmp.length());
		} else if (tmp[0] == '"') {
			histoLiteral.Add(tmp.length());
		} else if (tmp[0] == '_') {
			histoBlank.Add(tmp.length());
		} else {
			cout << "String not URI/Lit?: " << tmp << endl;
		}
	}

	histoURI.end();
	histoURI.dump(output.c_str(), "URI");

	histoLiteral.end();
	histoLiteral.dump(output.c_str(), "Literal");

	histoBlank.end();
	histoBlank.dump(output.c_str(), "Blank");
}

string PlainDictionary::getType() {
	return HDTVocabulary::DICTIONARY_TYPE_PLAIN;
}


}

void hdt::PlainDictionary::getSuggestions(const char *base, hdt::TripleComponentRole role, std::vector<std::string> &out, int maxResults)
{
    throw "getSuggestions not implemented";
}
