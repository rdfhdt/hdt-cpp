/*
 * Header.hpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#ifndef HEADER_
#define HEADER_

#include <RDF.hpp>
#include <ControlInformation.hpp>
#include <iostream>
#include <sstream>
#include <HDTListener.hpp>

namespace hdt {

/**
 * Describes the Header of any HDT.
 */
class Header : public RDFStorage {

public:
	virtual ~Header() { }

	virtual bool save(std::ostream &output, ControlInformation &ci, ProgressListener *listener = NULL)=0;
	virtual void load(std::istream &input, ControlInformation &ci, ProgressListener *listener = NULL)=0;

	/**
	 * Returns the number of triples
	 *
	 * @return
	 */
	virtual unsigned int getNumberOfElements()=0;

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
		}
		delete it;

		return out;
	}

	int getPropertyInt(const char *subject, const char *predicate) {
		string str = getProperty(subject, predicate);
		stringstream ss(str);
		int value;
		ss >> value;
		return value;
	}

	long long getPropertyLong(const char *subject, const char *predicate) {
		string str = getProperty(subject, predicate);
		stringstream ss(str);
		long long value;
		ss >> value;
		return value;
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
		if(object.at(0)!='<' && object.at(0)!='"' && object.at(0)!='_') {
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
		TripleString ts(subject, predicate, st.str());
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
	 * Remove a set of patterns.
	 * Removes all triples that match the specified triple patterns.
	 *
	 * @param triples Iterator of TripleString indicating the triple patterns to be removed.
	 */
	virtual void remove(IteratorTripleString *triples) = 0;
};

}

#endif /* HEADER_HPP_ */
