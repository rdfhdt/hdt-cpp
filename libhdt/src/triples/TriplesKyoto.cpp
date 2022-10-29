/*
 * File: TriplesKyoto.cpp
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
#include <stdexcept>
#include <HDTVocabulary.hpp>

#ifndef WIN32
#include <unistd.h>
#endif

#include "TriplesKyoto.hpp"
#include "TriplesComparator.hpp"
#include "../util/StopWatch.hpp"

#include <algorithm>

#include "../util/Histogram.h"

#ifdef HAVE_KYOTO

namespace hdt {

TriplesKyoto::TriplesKyoto() : order(Unknown)
{
}

int32_t TriplesKyoto::compare (const char *akbuf, size_t aksiz, const char *bkbuf, size_t bksiz) {
    if(aksiz==bksiz && bksiz==sizeof(TripleID)) {
        TripleID *a = (TripleID *)akbuf;
        TripleID *b = (TripleID *)bkbuf;

        int result = comparator(*a, *b);
        cout << endl << " compare " << *a << " = " << *b << "    = " << result << endl;
        return result;
    }
    throw std::runtime_error("TripleKyoto: Error comparing buffers");
}

TriplesKyoto::TriplesKyoto(HDTSpecification &specification) : spec(specification) {
	unlink("triples.kct");
	string ord = spec.getOrEmpty("triplesOrder");

    order = parseOrder(ord.c_str());
    if(order==Unknown){
        order = SPO;
    }

    comparator.setOrder(order);

    db.tune_comparator(this);
	db.tune_options(TreeDB::TLINEAR /*| TreeDB::TCCOMPESS*/);
	db.tune_alignment(0);
    db.tune_buckets(1LL*1000*1000);
    db.tune_map(256LL*1024*1024);
    db.tune_page_cache(64LL*1024*1024);
	if (!db.open("triples.kct", TreeDB::OWRITER | TreeDB::OCREATE)) {
		cerr << "Triples DB open error: " << db.error().name() << endl;
	}
}

TriplesKyoto::~TriplesKyoto()
{
	db.close();
    //unlink("triples.kct");
}

IteratorTripleID *TriplesKyoto::search(TripleID &pattern)
{
#if 1
	cout << endl << "DB size: " << db.count() << endl;

	DB::Cursor *cur = db.cursor();
	cur->jump();

	size_t count = 0;
	size_t total = db.count();

	while(count<total-1) {
		size_t fsize;
		TripleID *ptr = (TripleID*)cur->get_key(&fsize, true);
		cout << "Triple: " << *ptr << endl;
		count++;
	}

	cout << "Iterated" << endl;

	delete cur;

	db.close();

	cout << "Closed" << endl;

	exit(0);
#endif

	string patternString = pattern.getPatternString();
	if(patternString=="???") {
		return new TriplesKyotoIterator(&db);
	} else {
		return new SequentialSearchIteratorTripleID(pattern, new TriplesKyotoIterator(&db));
	}
}

IteratorTripleID *TriplesKyoto::searchJoin(TripleID &a, TripleID &b, unsigned short conditions) {
	throw std::logic_error("Not Implemented");
}

float TriplesKyoto::cost(TripleID &pattern)
{
	// TODO: Theoretically define this with the team
	throw std::logic_error("Not Implemented");
}

size_t TriplesKyoto::getNumberOfElements()
{
	return db.count();
}

size_t TriplesKyoto::size()
{
	return db.size();
}

void TriplesKyoto::save(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener)
{

}

void TriplesKyoto::load(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener)
{

}

size_t TriplesKyoto::load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener)
{
    throw std::logic_error("Not Implemented");
}

void TriplesKyoto::load(ModifiableTriples &input, ProgressListener *listener)
{
	IteratorTripleID *it = input.searchAll();

	while(it->hasNext()) {
		TripleID *triple = it->next();

		this->insert(*triple);
	}

	delete it;
}

void TriplesKyoto::generateIndex(ProgressListener *listener) {

}

void TriplesKyoto::saveIndex(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener) {

}

void TriplesKyoto::loadIndex(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener) {

}

size_t TriplesKyoto::loadIndex(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener)
{
	return 0;
}


void TriplesKyoto::populateHeader(Header &header, string rootNode)
{
	header.insert(rootNode, HDTVocabulary::TRIPLES_TYPE, HDTVocabulary::TRIPLES_TYPE_KYOTO);
	header.insert(rootNode, HDTVocabulary::TRIPLES_NUM_TRIPLES, getNumberOfElements() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_ORDER, getOrderStr(order) );
}

void TriplesKyoto::startProcessing(ProgressListener *listener)
{
}

void TriplesKyoto::stopProcessing(ProgressListener *listener)
{
}

string TriplesKyoto::getType() {
    return HDTVocabulary::TRIPLES_TYPE_KYOTO;
}

TripleComponentOrder TriplesKyoto::getOrder()
{
    return order;
}

// From ModifiableTriples

void TriplesKyoto::insert(TripleID &triple)
{
    // Add the triple
    if(!db.set((const char*)&triple, sizeof(TripleID), (const char*)&triple, sizeof(TripleID))) {
		cerr << endl << "set error: " << db.error().name() << endl;
	}

    cout << "Inserted " << triple << " Now: " << db.count() << endl;
}

void TriplesKyoto::insert(IteratorTripleID *triples)
{
	while( triples->hasNext() ) {
		this->insert(*triples->next());
	}
}

bool TriplesKyoto::remove(TripleID &pattern)
{
	throw std::logic_error("Not Implemented");
}

bool TriplesKyoto::remove(IteratorTripleID *pattern)
{
	throw std::logic_error("Not Implemented");
}

void TriplesKyoto::sort(TripleComponentOrder order, ProgressListener *listener)
{
	// Sorted already
    this->order = order;
}


void TriplesKyoto::setOrder(TripleComponentOrder order)
{
	this->order = order;

}

void TriplesKyoto::removeDuplicates(ProgressListener *listener) {
	// Do nothing, doesn't have duplicates
}



// ITERATOR

TriplesKyotoIterator::TriplesKyotoIterator(TreeDB *db) : db(db), cur(db->cursor()), count(0), total(db->count()) {
    cur->jump();

}

TriplesKyotoIterator::~TriplesKyotoIterator() {
    delete cur;
}

bool TriplesKyotoIterator::hasNext() {
    return count<total-1;
}

TripleID *TriplesKyotoIterator::next() {
    const char *ptr = cur->get_key(&size, true);
    count++;

    if(ptr!=NULL && size==sizeof(TripleID)) {
        memcpy(&returnTriple, ptr, size);
        delete [] ptr;

    } else {
        cerr << "Something went wrong, got wrong size for a TripleID Key: "<< size << endl;
    }
    cout << "Return: " << returnTriple << endl;
    return &returnTriple;
}


}

#endif /* HAVE_KYOTO */
