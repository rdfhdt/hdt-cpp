/*
 * File: KyotoDictionary.cpp
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

#include <sstream>

#include "KyotoDictionary.hpp"

#include <HDTVocabulary.hpp>
#include <HDTEnums.hpp>

namespace hdt {

/* DICTIONARY ENTRY */
KyotoDictionary::KyotoDictionary() {
	this->mapping = MAPPING2;
}

KyotoDictionary::KyotoDictionary(HDTSpecification &specification) : spec(specification) {
	if(spec.get("dictionary.mapping")=="mapping1") {
		this->mapping = MAPPING1;
	} else {
		this->mapping = MAPPING2;
	}
}

KyotoDictionary::~KyotoDictionary() {
	string spath = subjects.path();
	string ppath = predicates.path();
	string opath = objects.path();
	string shpath = shared.path();

	/*subjects.clear();
	predicates.clear();
	objects.clear();
	shared.clear();*/

	subjects.close();
	predicates.close();
	objects.close();
	shared.close();

#if 0
	if(spath.length()>0) {
		unlink(spath.c_str());
	}
	if(ppath.length()>0){
		unlink(ppath.c_str());
	}
	if(opath.length()>0) {
		unlink(opath.c_str());
	}
	if(shpath.length()>0) {
		unlink(shpath.c_str());
	}
#endif
}

std::string KyotoDictionary::idToString(unsigned int id, TripleComponentRole position)
{
	throw "Not implemented";
}

unsigned int KyotoDictionary::stringToId(std::string &key, TripleComponentRole position)
{

	unsigned int ret;

	if(key.length()==0 || key.at(0) == '?') {
		return 0;
	}

	switch (position) {
	case SUBJECT:
		if(shared.get((const char *)key.c_str(),(size_t)key.length(), (char *) &ret, sizeof(ret))) {
			return getGlobalId(ret,SHARED_SUBJECT);
		}
		if(subjects.get((const char *)key.c_str(),(size_t)key.length(), (char *) &ret, sizeof(ret))) {
			return getGlobalId(ret,NOT_SHARED_SUBJECT);
		}
		throw "Subject not found in dictionary";
	case PREDICATE:
		if(predicates.get((const char *)key.c_str(),(size_t)key.length(), (char *) &ret, sizeof(ret))) {
			return getGlobalId(ret, NOT_SHARED_PREDICATE);
		}
		throw "Predicate not found in dictionary";

	case OBJECT:
		if(shared.get((const char *)key.c_str(),(size_t)key.length(), (char *) &ret, sizeof(ret))) {
			return getGlobalId(ret,SHARED_OBJECT);
		}
		if(objects.get((const char *)key.c_str(),(size_t)key.length(), (char *) &ret, sizeof(ret))) {
			return getGlobalId(ret,NOT_SHARED_OBJECT);
		}
		throw "Object not found in dictionary";
	}

	// NOT REACHED
	return 0;
}

void KyotoDictionary::startProcessing(ProgressListener *listener)
{
	// TODO: Add some random suffix to the filenames
#if 1
	unlink("subjects.kct");
	unlink("shared.kct");
	unlink("objects.kct");
	unlink("predicates.kct");
#endif

	subjects.tune_options(TreeDB::TLINEAR /*| TreeDB::TCCOMPESS*/);
	subjects.tune_alignment(0);
	subjects.tune_buckets(1LL*1000*1000);
	subjects.tune_map(256LL *1024*1024);			// 8Gb
	//subjects.tune_page_cache(256LL*1024*1024);	// 512Mb
	if (!subjects.open("subjects.kct", TreeDB::OWRITER | TreeDB::OCREATE)) {
		cerr << "subjects db open error: " << subjects.error().name() << endl;
	}

	if (!predicates.open("predicates.kct", ProtoTreeDB::OWRITER | ProtoTreeDB::OCREATE)) {
		cerr << "predicates db open error: " << predicates.error().name() << endl;
	}

	objects.tune_options(TreeDB::TLINEAR /*| TreeDB::TCCOMPESS*/);
	objects.tune_alignment(0);
	objects.tune_buckets(1LL*1000*1000);
	objects.tune_map(256LL*1024*1024);		// 8Gb
	//objects.tune_page_cache(1<<30);	// 1Gb
	if (!objects.open("objects.kct", TreeDB::OWRITER | TreeDB::OCREATE)) {
		cerr << "objects db open error: " << objects.error().name() << endl;
	}

	shared.tune_options(TreeDB::TLINEAR /*| TreeDB::TCCOMPESS*/);
	shared.tune_alignment(0);
	shared.tune_buckets(100LL*1000);
	shared.tune_map(1LL *1024*1024*1024);			// 8GB
	//shared.tune_page_cache(256LL *1024*1024);	// 256Mb
	if (!shared.open("shared.kct", TreeDB::OWRITER | TreeDB::OCREATE)) {
		cerr << "shared db open error: " << shared.error().name() << endl;
	}
}

void KyotoDictionary::updateIDs(DB *db) {
	DB::Cursor *cur=db->cursor();
	cur->jump();
	unsigned int count=1;
	while (cur->set_value((const char*)&count,sizeof(count), true)) {
		count++;
	}
	delete cur;
}

void KyotoDictionary::stopProcessing(ProgressListener *listener)
{
	// EXTRACT SHARED
	DB::Cursor* cur = subjects.cursor();
	cur->jump();
	string ckey;
	unsigned int count=1;
	NOTIFY(listener, "Extracting Shared", 0, 100);
	while (cur->get_key(&ckey, true)) {

		// Try to remove
		if(objects.remove(ckey)) {
			// IF existed, remove also from subjects
			subjects.remove(ckey);

			// And add to shared
			shared.set(ckey.c_str(), ckey.length(), (const char *)&count, sizeof(count));
		}
	}
	delete cur;

	// Update IDs
	NOTIFY(listener, "Updating IDs Shared", 0, 100);
	updateIDs(&shared);
	NOTIFY(listener, "Updating IDs Subjects", 0, 100);
	updateIDs(&subjects);
	NOTIFY(listener, "Updating IDs Predicates", 0, 100);
	updateIDs(&predicates);
	NOTIFY(listener, "Updating IDs Objects", 0, 100);
	updateIDs(&objects);

#if 0
	// Defrag
	NOTIFY(listener, "Defrag Subjects", 0, 100);
	subjects.defrag();
	NOTIFY(listener, "Defrag Shared", 0, 100);
	shared.defrag();
	NOTIFY(listener, "Defrag Objects", 0, 100);
	objects.defrag();
#endif

	dumpSizes(cout);
}


void KyotoDictionary::save(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener)
{

	throw "Not implemented";
}

size_t KyotoDictionary::load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener) {
	throw "Not implemented";
}

void KyotoDictionary::load(std::istream & input, ControlInformation &ci, ProgressListener *listener)
{
	throw "Not implemented";

}

void KyotoDictionary::import(Dictionary *other, ProgressListener *listener) {

	throw "Not implemented";
}

IteratorUCharString *KyotoDictionary::getSubjects() {
	return new KyotoDictIterator(&this->subjects);
}

IteratorUCharString *KyotoDictionary::getPredicates() {
	return new KyotoDictIterator(&this->predicates);
}

IteratorUCharString *KyotoDictionary::getObjects() {
	return new KyotoDictIterator(&this->objects);
}

IteratorUCharString *KyotoDictionary::getShared() {
	return new KyotoDictIterator(&this->shared);
}

unsigned int KyotoDictionary::getNumberOfElements()
{
	return subjects.count()+predicates.count()+objects.count()+shared.count();
}

unsigned int KyotoDictionary::size()
{
    return sizeStrings;
}


unsigned int KyotoDictionary::insert(std::string & str, TripleComponentRole pos)
{

	if(str=="") return 0;

	unsigned int value=0;

	if(pos==SUBJECT) {
			if(!subjects.set(str.c_str(), str.length(), (const char*)&value, sizeof(value))) cerr << "set error: " << subjects.error().name() << endl;
	} else if(pos==PREDICATE) {
			if(!predicates.set(str.c_str(), str.length(), (const char*)&value, sizeof(value))) cerr << "set error: " << predicates.error().name() << endl;
	} else if(pos==OBJECT) {
			if(!objects.set(str.c_str(), str.length(), (const char*)&value, sizeof(value))) cerr << "set error: " << objects.error().name() << endl;;
	}
	return 0;
}


unsigned int KyotoDictionary::getGlobalId(unsigned int mapping, unsigned int id, DictionarySection position) {
	switch (position) {
		case NOT_SHARED_SUBJECT:
			return shared.count()+id+1;

		case NOT_SHARED_PREDICATE:
			return id+1;

		case NOT_SHARED_OBJECT:
			if(mapping==MAPPING2) {
				return shared.count()+id+1;
			} else {
				return shared.count()+subjects.count()+id+1;
			}

		case SHARED_SUBJECT:
		case SHARED_OBJECT:
			return id+1;
		}

		throw "Item not found";
}


unsigned int KyotoDictionary::getGlobalId(unsigned int id, DictionarySection position) {
	return getGlobalId(this->mapping, id, position);
}

unsigned int KyotoDictionary::getLocalId(unsigned int mapping, unsigned int id, TripleComponentRole position) {
	switch (position) {
		case SUBJECT:
			if(id<=shared.count()) {
				return id-1;
			} else {
				return id-shared.count()-1;
			}
			break;
		case OBJECT:
			if(id<=shared.count()) {
				return id-1;
			} else {
				if(mapping==MAPPING2) {
					return id-shared.count()-1;
				} else {
					return id-shared.count()-subjects.count()-1;
				}
			}
			break;
		case PREDICATE:
			return id-1;
		}

		throw "Item not found";
}

unsigned int KyotoDictionary::getLocalId(unsigned int id, TripleComponentRole position) {
	return getLocalId(mapping,id,position);
}



/** Get Max Id
 * @return The expected result
 */
unsigned int KyotoDictionary::getMaxID() {
	unsigned int s = subjects.count();
	unsigned int o = objects.count();
	unsigned int shared = subjects.count();
	unsigned int max = s>o ? s : o;

	if(mapping ==MAPPING2) {
		return shared+max;
	} else {
		return shared+s+o;
	}
}

unsigned int KyotoDictionary::getMaxSubjectID() {
	unsigned int sh = shared.count();
	unsigned int s = subjects.count();

	return sh+s;
}

unsigned int KyotoDictionary::getMaxPredicateID() {
	return predicates.count();
}

unsigned int KyotoDictionary::getMaxObjectID() {
	unsigned int sh = shared.count();
	unsigned int s = subjects.count();
	unsigned int o = objects.count();

	if(mapping ==MAPPING2) {
		return sh+o;
	} else {
		return sh+s+o;
	}
}

unsigned int KyotoDictionary::getNsubjects() {
	return shared.count()+subjects.count();
}

unsigned int KyotoDictionary::getNpredicates() {
	return predicates.count();
}

unsigned int KyotoDictionary::getNobjects() {
	return shared.count()+objects.count();
}

unsigned int KyotoDictionary::getNshared() {
	return shared.count();
}

void KyotoDictionary::populateHeader(Header &header, string rootNode)
{

}

void KyotoDictionary::dumpSizes(ostream &out) {
	cout << endl;
	cout << "\n\t [Dictionary stats:\n";
	cout << "\t   shared subjects-objects:" << getNshared() << "\n";
	cout << "\t   not shared subjects:" << getNsubjects() - getNshared() << "\n";
	cout << "\t   not shared objects:" << getNobjects() - getNshared() << "\n";
	cout << "\t total subjects:" << getNsubjects() << "\n";
	cout << "\t total objects:" << getNobjects() << "\n";
	cout << "\t total predicates:" << getNpredicates() << " ]\n\n";
	cout << endl;
}


string KyotoDictionary::getType() {
	return HDTVocabulary::DICTIONARY_TYPE_PLAIN;
}

unsigned int KyotoDictionary::getMapping() {
	return mapping;
}

void KyotoDictionary::getSuggestions(const char *base, hdt::TripleComponentRole role, std::vector<std::string> &out, int maxResults)
{
    throw "getSuggestions not implemented";
}


}

