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

#ifndef WIN32
#include <unistd.h>
#endif
#include <stdexcept>
#include "KyotoDictionary.hpp"

#include <HDTVocabulary.hpp>
#include <HDTEnums.hpp>

#ifdef HAVE_KYOTO

namespace hdt {

/* DICTIONARY ENTRY */
KyotoDictionary::KyotoDictionary() {
	this->mapping = MAPPING2;
}

KyotoDictionary::KyotoDictionary(HDTSpecification &specification) : spec(specification) {
	string map = spec.getOrEmpty("dictionary.mapping");
	if(map=="mapping1") {
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

std::string KyotoDictionary::idToString(size_t id, TripleComponentRole position)
{
	throw std::logic_error("Not implemented");
}

size_t KyotoDictionary::stringToId(const std::string &key, TripleComponentRole position)
{

    size_t ret;

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
    return 0;
	case PREDICATE:
		if(predicates.get((const char *)key.c_str(),(size_t)key.length(), (char *) &ret, sizeof(ret))) {
			return getGlobalId(ret, NOT_SHARED_PREDICATE);
		}
    return 0;

	case OBJECT:
		if(shared.get((const char *)key.c_str(),(size_t)key.length(), (char *) &ret, sizeof(ret))) {
			return getGlobalId(ret,SHARED_OBJECT);
		}
		if(objects.get((const char *)key.c_str(),(size_t)key.length(), (char *) &ret, sizeof(ret))) {
			return getGlobalId(ret,NOT_SHARED_OBJECT);
		}
    return 0;
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
	subjects.tune_buckets(10LL*1000*1000);
	//subjects.tune_page(32768);
	subjects.tune_page_cache(256LL*1024*1024);
	subjects.tune_map(20LL*1024*1024*1024);
	if (!subjects.open("subjects.kct", TreeDB::OWRITER | TreeDB::OCREATE)) {
		cerr << "subjects db open error: " << subjects.error().name() << endl;
	}

	if (!predicates.open("predicates.kct", ProtoTreeDB::OWRITER | ProtoTreeDB::OCREATE)) {
		cerr << "predicates db open error: " << predicates.error().name() << endl;
	}

	objects.tune_options(TreeDB::TLINEAR /*| TreeDB::TCCOMPESS*/);
	objects.tune_alignment(0);
	objects.tune_buckets(100LL*1000*1000);
	//objects.tune_page(32768);
	objects.tune_page_cache(256LL*1024*1024);			// 8Gb
	objects.tune_map(20LL*1024*1024*1024);		// 8Gb
	//objects.tune_page_cache(1<<30);	// 1Gb
	if (!objects.open("objects.kct", TreeDB::OWRITER | TreeDB::OCREATE)) {
		cerr << "objects db open error: " << objects.error().name() << endl;
	}

	shared.tune_options(TreeDB::TLINEAR /*| TreeDB::TCCOMPESS*/);
	shared.tune_alignment(0);
	shared.tune_buckets(10*100LL*1000);
	//shared.tune_page(32768);
	shared.tune_page_cache(256LL*1024*1024);			// 8Gb
	shared.tune_map(20LL*1024*1024*1024);			// 8GB
	//shared.tune_page_cache(256LL *1024*1024);	// 256Mb
	if (!shared.open("shared.kct", TreeDB::OWRITER | TreeDB::OCREATE)) {
		cerr << "shared db open error: " << shared.error().name() << endl;
	}
}

void KyotoDictionary::updateIDs(DB *db) {
	DB::Cursor *cur=db->cursor();
	cur->jump();
    size_t count=1;
	while (cur->set_value((const char*)&count,sizeof(count), true)) {
		count++;
	}
	delete cur;
}

void KyotoDictionary::stopProcessing(ProgressListener *listener)
{
	// EXTRACT SHARED
#if 0
	// Go over subjects and objects in parallel
	DB::Cursor* curSubj = subjects.cursor();
	DB::Cursor* curObj = objects.cursor();
	curSubj->jump();
	curObj->jump();
	string subjectStr;
	string objectStr;
    size_t count=1;
	NOTIFY(listener, "Extracting Shared", 0, 100);

	bool fetchLeft = true;
	bool fetchRight = true;

	size_t leftCount=1;
	size_t rightCount=1;
	size_t iterations=0;

	size_t totalSubjects = subjects.count();
	size_t totalObjects = objects.count();

	fetchLeft = curSubj->get_key(&subjectStr);
	fetchRight = curObj->get_key(&objectStr);

	while(fetchLeft && fetchRight) {
		int cmp = subjectStr.compare(objectStr);

		//cerr << " Left: "<< subjectStr << endl << "Right: " << objectStr << endl << "Cmp: " << cmp << endl << endl;

		if(cmp==0) {
			//cerr << "New shared: " << subjectStr<<endl;
			curSubj->remove();
			curObj->remove();
			shared.set(subjectStr.c_str(), subjectStr.length(), (const char *)&count, sizeof(count));
			leftCount++;
			rightCount++;
			fetchLeft = curSubj->get_key(&subjectStr);
			fetchRight = curObj->get_key(&objectStr);
		} else if(cmp<0) {
			curSubj->step();
			fetchLeft = curSubj->get_key(&subjectStr);
			leftCount++;
		} else if(cmp>0) {
			curObj->step();
			fetchRight = curObj->get_key(&objectStr);
			rightCount++;
		}

	if((iterations%100000)==0) {
		cerr << "Iterations: "<< (iterations/1000) << "K   " << endl;
			cerr << "Subjects " << leftCount << "/" << totalSubjects << endl;
			cerr << "Objects " << rightCount << "/" << totalObjects << endl;
		}
		iterations++;
	}
	delete curSubj;
	delete curObj;
#else
	// For each Subject, check object.
	DB::Cursor* cur = subjects.cursor();
	cur->jump();
	string ckey;
	size_t count=1;
	size_t totalSubjects=subjects.count();
	NOTIFY(listener, "Extracting Shared", 0, 100);
	while (cur->get_key(&ckey, true)) {

		// Try to remove
		if(objects.remove(ckey)) {
			// IF existed, remove also from subjects
			subjects.remove(ckey);

			// And add to shared
			shared.set(ckey.c_str(), ckey.length(), (const char *)&count, sizeof(count));
		}

		if((count%100000)==0) {
			cerr << "Subjects: "<< (count/1000) << "K / " << (totalSubjects/1000) << "K" << endl;
		}
		count++;
	}
	delete cur;
#endif

	// Update IDs
	// Not needed?
#if 0
	NOTIFY(listener, "Updating IDs Shared", 0, 100);
	updateIDs(&shared);
	NOTIFY(listener, "Updating IDs Subjects", 0, 100);
	updateIDs(&subjects);
	NOTIFY(listener, "Updating IDs Predicates", 0, 100);
	updateIDs(&predicates);
	NOTIFY(listener, "Updating IDs Objects", 0, 100);
	updateIDs(&objects);
#endif

#if 0
	// Defrag
	NOTIFY(listener, "Defrag Subjects", 0, 100);
	subjects.defrag();
	NOTIFY(listener, "Defrag Shared", 0, 100);
	shared.defrag();
	NOTIFY(listener, "Defrag Objects", 0, 100);
	objects.defrag();
#endif

	dumpSizes(cerr);
}


void KyotoDictionary::save(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener)
{

	throw std::logic_error("Not implemented");
}

size_t KyotoDictionary::load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener) {
	throw std::logic_error("Not implemented");
}

void KyotoDictionary::load(std::istream & input, ControlInformation &ci, ProgressListener *listener)
{
	throw std::logic_error("Not implemented");

}

void KyotoDictionary::import(Dictionary *other, ProgressListener *listener) {

	throw std::logic_error("Not implemented");
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

size_t KyotoDictionary::getNumberOfElements()
{
	return subjects.count()+predicates.count()+objects.count()+shared.count();
}

uint64_t KyotoDictionary::size()
{
    return sizeStrings;
}


size_t KyotoDictionary::insert(const std::string & str, TripleComponentRole pos)
{

	if(str=="") return 0;

    size_t value=0;

	if(pos==SUBJECT) {
			if(!subjects.set(str.c_str(), str.length(), (const char*)&value, sizeof(value))) cerr << "set error: " << subjects.error().name() << endl;
	} else if(pos==PREDICATE) {
			if(!predicates.set(str.c_str(), str.length(), (const char*)&value, sizeof(value))) cerr << "set error: " << predicates.error().name() << endl;
	} else if(pos==OBJECT) {
			if(!objects.set(str.c_str(), str.length(), (const char*)&value, sizeof(value))) cerr << "set error: " << objects.error().name() << endl;;
	}
	return 0;
}


size_t KyotoDictionary::getGlobalId(size_t mapping, size_t id, DictionarySection position) {
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

		throw std::runtime_error("Item not found");
}


size_t KyotoDictionary::getGlobalId(size_t id, DictionarySection position) {
	return getGlobalId(this->mapping, id, position);
}

size_t KyotoDictionary::getLocalId(size_t mapping, size_t id, TripleComponentRole position) {
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

		throw std::runtime_error("Item not found");
}

size_t KyotoDictionary::getLocalId(size_t id, TripleComponentRole position) {
	return getLocalId(mapping,id,position);
}



/** Get Max Id
 * @return The expected result
 */
size_t KyotoDictionary::getMaxID() {
    size_t s = subjects.count();
    size_t o = objects.count();
    size_t shared = subjects.count();
    size_t max = s>o ? s : o;

	if(mapping ==MAPPING2) {
		return shared+max;
	} else {
		return shared+s+o;
	}
}

size_t KyotoDictionary::getMaxSubjectID() {
    size_t sh = shared.count();
    size_t s = subjects.count();

	return sh+s;
}

size_t KyotoDictionary::getMaxPredicateID() {
	return predicates.count();
}

size_t KyotoDictionary::getMaxObjectID() {
    size_t sh = shared.count();
    size_t s = subjects.count();
    size_t o = objects.count();

	if(mapping ==MAPPING2) {
		return sh+o;
	} else {
		return sh+s+o;
	}
}

size_t KyotoDictionary::getNsubjects() {
	return shared.count()+subjects.count();
}

size_t KyotoDictionary::getNpredicates() {
	return predicates.count();
}

size_t KyotoDictionary::getNobjects() {
	return shared.count()+objects.count();
}

size_t KyotoDictionary::getNshared() {
	return shared.count();
}

void KyotoDictionary::populateHeader(Header &header, string rootNode)
{

}

void KyotoDictionary::dumpSizes(ostream &out) {
	cerr << endl;
	cerr << "\n\t [Dictionary stats:\n";
	cerr << "\t   shared subjects-objects:" << getNshared() << "\n";
	cerr << "\t   not shared subjects:" << getNsubjects() - getNshared() << "\n";
	cerr << "\t   not shared objects:" << getNobjects() - getNshared() << "\n";
	cerr << "\t total subjects:" << getNsubjects() << "\n";
	cerr << "\t total objects:" << getNobjects() << "\n";
	cerr << "\t total predicates:" << getNpredicates() << " ]\n\n";
	cerr << endl;
}


string KyotoDictionary::getType() {
	return HDTVocabulary::DICTIONARY_TYPE_PLAIN;
}

size_t KyotoDictionary::getMapping() {
	return mapping;
}

void KyotoDictionary::getSuggestions(const char *base, hdt::TripleComponentRole role, std::vector<std::string> &out, int maxResults)
{
    throw std::logic_error("getSuggestions not implemented");
}
hdt::IteratorUCharString * KyotoDictionary::getSuggestions(const char *base, hdt::TripleComponentRole role)
{
    throw std::logic_error("getSuggestions not implemented");
}

hdt::IteratorUInt *KyotoDictionary::getIDSuggestions(const char *prefix, TripleComponentRole role){
	  throw std::logic_error("getIDSuggestions not implemented");
}

}

#endif
