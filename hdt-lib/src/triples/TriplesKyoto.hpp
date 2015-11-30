/*
 * File: TriplesKyoto.hpp
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

#ifndef TRIPLESKYOTO_H_
#define TRIPLESKYOTO_H_

#include <Triples.hpp>
#include <HDTSpecification.hpp>
#include "TripleIterators.hpp"

#include "TriplesComparator.hpp"
#include "../libdcs/CSD.h"

#ifdef HAVE_KYOTO

#include <kcpolydb.h>

using namespace kyotocabinet;

namespace hdt {

class TriplesKyoto : public ModifiableTriples, public Comparator {
private:
	TreeDB db;
	TripleComponentOrder order;
    TriplesComparator comparator;
	HDTSpecification spec;

public:
	TriplesKyoto();
	TriplesKyoto(HDTSpecification &specification);
	virtual ~TriplesKyoto();

    int32_t compare (const char *akbuf, size_t aksiz, const char *bkbuf, size_t bksiz);


    bool isIndexed() const {
        return false;
    }

	// From Triples

	/**
	 * Returns a vector of triples matching the pattern
	 *
	 * @param pattern
	 * @return
	 */
	IteratorTripleID *search(TripleID &pattern);

	IteratorTripleID *searchJoin(TripleID &a, TripleID &b, unsigned short conditions);

	/**
	 * Calculates the cost to retrieve a specific pattern
	 *
	 * @param triple
	 * @return
	 */
	float cost(TripleID &triple);

	/**
	 * Returns the number of triples contained
	 *
	 * @return
	 */
    size_t getNumberOfElements();

	/**
	 * Returns size in bytes
	 */
    size_t size();

	/**
	 * Saves the triples
	 *
	 * @param output
	 * @return
	 */
	void save(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener = NULL);

	/**
	 * Loads triples from a file
	 *
	 * @param input
	 * @return
	 */
	void load(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener = NULL);

	size_t load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener=NULL);

	void load(ModifiableTriples &input, ProgressListener *listener = NULL);

	void generateIndex(ProgressListener *listener);

	void saveIndex(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener);
	void loadIndex(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener);

    size_t loadIndex(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener);

	/**
	 * Populates the header
	 *
	 * @param header
	 * @return
	 */
	void populateHeader(Header &header, string rootNode);

	void startProcessing(ProgressListener *listener=NULL);

	void stopProcessing(ProgressListener *listener=NULL);

	string getType();

	TripleComponentOrder getOrder();


	// From ModifiableTriples

	/**
	 * Adds one or more triples
	 *
	 * @param triples The triples to be inserted
	 * @return boolean
	 */
	void insert(TripleID &triple);

	void insert(IteratorTripleID *triples);

	/**
	 * Deletes one or more triples according to a pattern
	 *
	 * @param pattern
	 *            The pattern to match against
	 * @return boolean
	 */
	bool remove(TripleID &pattern);

	bool remove(IteratorTripleID *pattern);

	/**
	 * Sorts the triples based on an order(TripleComponentOrder)
	 *
	 * @param order The order to sort the triples with
	 */
	void sort(TripleComponentOrder order, ProgressListener *listener = NULL);

	void removeDuplicates(ProgressListener *listener = NULL);

	/**
	 * Sets a type of order(TripleComponentOrder)
	 *
	 * @param order The order to set
	 */
	void setOrder(TripleComponentOrder order);

	friend class TriplesKyotoIterator;
};

class TriplesKyotoIterator : public IteratorTripleID {
private:
	TripleID pattern, returnTriple;
	TreeDB *db;
	DB::Cursor *cur;
	const char *ptr;
	size_t size;
	size_t count, total;
public:
    TriplesKyotoIterator(TreeDB *db);

    ~TriplesKyotoIterator();

    bool hasNext();

    TripleID *next();
};

} // namespace hdt

#endif /* HAVE_KYOTO */

#endif /* TRIPLESKYOTO_H_ */

