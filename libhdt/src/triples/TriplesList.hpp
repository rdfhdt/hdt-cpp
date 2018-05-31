/*
 * File: TriplesList.hpp
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

#ifndef TRIPLESLIST_H_
#define TRIPLESLIST_H_

#include <Triples.hpp>
#include <HDTSpecification.hpp>
#include "TripleIterators.hpp"

namespace hdt {

class TriplesList : public ModifiableTriples {
private:
	ControlInformation controlInformation;
	HDTSpecification spec;

	std::vector<TripleID> arrayOfTriples;
    TripleID *ptr;
	TripleComponentOrder order;
    size_t numValidTriples;

public:
	TriplesList();
	TriplesList(HDTSpecification &specification);
	virtual ~TriplesList();

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
	float cost(TripleID &triple) const;

	/**
	 * Returns the number of triples contained
	 *
	 * @return
	 */
    size_t getNumberOfElements() const;

	/**
	 * Returns size in bytes
	 */
    size_t size() const;

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

	string getType() const;

	TripleComponentOrder getOrder() const;

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

     void calculateDegree(string path, size_t numPredicates,size_t maxID=0);
     void calculateMinStats(string path, size_t numPredicates);
     void calculateDegreeType(string path, size_t rdftypeID);
     void calculateDegrees(string path,size_t maxSOID=0,size_t numPredicates=0,size_t rdftypeID=0,bool allStats=false);

	// Others

	/**
	 *
	 * @param i
	 * @return
	 */
    TripleID *getTripleID(size_t i);

	friend class TriplesListIterator;
};


class TriplesListIterator : public IteratorTripleID {
private:
	TripleID pattern, returnTriple;
	TriplesList *triples;
	uint64_t pos;

public:
	TriplesListIterator(TriplesList *triples, TripleID &pattern);
	bool hasNext();
	TripleID *next();
	bool hasPrevious();
	TripleID *previous();
	void goToStart();
};

} // namespace hdt

#endif /* TRIPLESLIST_H_ */

