/*
 * File: Header.hpp
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

#ifndef HDT_HEADER_HPP_
#define HDT_HEADER_HPP_

#include "RDF.hpp"
#include "ControlInformation.hpp"
#include "HDTListener.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>

namespace hdt {

/**
 * Describes the Header of any HDT.
 */
class Header : public RDFStorage {

public:
	virtual ~Header() { }

	virtual void save(std::ostream &output, ControlInformation &ci, ProgressListener *listener = NULL)=0;
	virtual void load(std::istream &input, ControlInformation &ci, ProgressListener *listener = NULL)=0;

    virtual size_t load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener=NULL)=0;

	/**
	 * Returns the number of triples
	 *
	 * @return
	 */
    virtual size_t getNumberOfElements()=0;

	/**
	 * Search a Triple Pattern.
	 *
	 * Searches all triples that match the supplied Triple Pattern. The pattern is composed by
	 * a subject, a predicate and and object, where NULL or empty string "" mean match any.
	 *
	 * @param subject
	 * @param predicate
	 * @param object
	 * @return
	 */
	virtual IteratorTripleString *search(const char *subject, const char *predicate, const char *object) = 0;

	std::string getProperty(const char *subject, const char *predicate) {
		hdt::IteratorTripleString *it = search(subject, predicate, "");
		std::string out;
		if(it->hasNext()) {
			hdt::TripleString *ts = it->next();
			out = ts->getObject();
		} else {
		    throw std::runtime_error("Not found");
		}
		delete it;

		return out;
	}

	int getPropertyInt(const char *subject, const char *predicate) {
		string str = getProperty(subject, predicate);
		stringstream ss(str);
		char c;
		int value;
		ss >> c;
		ss >> value;
		return value;
	}

	long long getPropertyLong(const char *subject, const char *predicate) {
		string str = getProperty(subject, predicate);
		stringstream ss(str);
		char c;   // First "
		long long value;
		ss >> c;
		ss >> value;
		return value;
	}

	string getSubject(const char *property, const char *value) {
		hdt::IteratorTripleString *it = search("", property, value);
		std::string out;
		if(it->hasNext()) {
			hdt::TripleString *ts = it->next();
                        out = ts->getSubject();
		}
		delete it;

		return out;
	}

	/**
	 * Insert a triple to the collection.
	 *
	 * @param triples New triple to be added.
	 */
	virtual void insert(TripleString &triple) = 0;

	/**
	 * Insert a triple to the collection.
	 *
	 * @param triples New triple to be added.
	 */
	void insert(string subject, string predicate, string object) {
		if(object.length()==0 || (object.at(0)!='<' && object.at(0)!='"' && object.at(0)!='_')) {
			object = "\""+object+"\"";
		}
		TripleString ts(subject, predicate, object);
		insert(ts);
	}

	/**
	 * Insert a triple to the collection.
	 *
	 * @param triples New triple to be added.
	 */
	void insert(string subject, string predicate, long long object) {
		stringstream st;
		st << object;
		//TripleString ts(subject, predicate, "\""+st.str()+"\"^^<http://www.w3.org/2001/XMLSchema#integer>");
		TripleString ts(subject, predicate, "\""+st.str()+"\"");
		insert(ts);
	}

	/**
	 * Insert a set of triples, as specified in the iterator.
	 *
	 * @param triples Iterator of Triples
	 */
	virtual void insert(IteratorTripleString *triples) = 0;

	/**
	 * Removes from the collection all triples that match the supplied Triple pattern.
	 *
	 * @param triples
	 */
	virtual void remove(TripleString &triples) = 0;

	/**
	 * Remove all triples from the storage.
	 */
	virtual void clear()=0;

	virtual string getAnon() = 0;

	virtual string getBaseURI()=0;
};

}

#endif /* HDT_HEADER_HPP_ */
