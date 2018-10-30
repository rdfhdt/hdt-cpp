/*
 * File: PlainHeader.hpp
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

#ifndef PLAINHEADER_H_
#define PLAINHEADER_H_

#include <HDT.hpp>
#include <Header.hpp>

namespace hdt {

class PlainHeader : public Header {
private:
	HDTSpecification spec;
	vector<TripleString> triples;
    size_t anonCounter;

public:
	PlainHeader();
	PlainHeader(HDTSpecification &specification);
	virtual ~PlainHeader();

    void save(std::ostream &output, ControlInformation &ci, ProgressListener *listener = NULL);

    void load(std::istream &input, ControlInformation &ci, ProgressListener *listener = NULL);

    size_t load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener=NULL);

	/**
	 * Returns the number of triples
	 *
	 * @return
	 */
    size_t getNumberOfElements();

	/**
	 * @param subject
	 * @param predicate
	 * @param object
	 * @return
	 */
	IteratorTripleString *search(const char *subject, const char *predicate, const char *object);

	/**
	 *
	 * @param triples
	 */
	void insert(TripleString &triple);

	void insert(IteratorTripleString *triple);

	/**
	 * Deletes with pattern matching
	 *
	 * @param triples
	 */
	void remove(TripleString &triples);

	void clear();

	string getAnon();

	string getBaseURI();

	friend class PlainHeaderIteratorTripleString;
};



class PlainHeaderIteratorTripleString : public IteratorTripleString {

private:
        PlainHeader *header;
        size_t pos;
        TripleString nextTriple, pattern, returnTriple;
        bool hasMoreTriples;

        void doFetch();
        void getNextTriple();
public:
        PlainHeaderIteratorTripleString(PlainHeader *header, TripleString &pattern);

        bool hasNext();
        TripleString *next();
        void goToStart();
};


}

#endif /* PLAINHEADER_H_ */
