/*
 * Triples.hpp
 *
 * Copyright (C) 2011, Javier D. Fernandez, Miguel A. Martinez-Prieto
 *                     Guillermo Rodriguez-Cano, Alejandro Andres,
 *                     Mario Arias
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
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *   Guillermo Rodriguez-Cano:  wileeam@acm.org
 *   Alejandro Andres:          fuzzy.alej@gmail.com
 *   Mario Arias:               mario.arias@gmail.com
 *
 * @version $Id$
 *
 */

#ifndef HDT_TRIPLES_
#define HDT_TRIPLES_

#include <HDTEnums.hpp>
#include <SingleTriple.hpp>
#include <Header.hpp>
#include <ControlInformation.hpp>
#include <HDTListener.hpp>

#include <iostream>
#include <iterator>
#include <vector>

namespace hdt {

class ModifiableTriples;

/**
 * Interface that defines readonly access to a set of triples. It allows loading a saving from a file.
 */

class Triples {
public:

	virtual ~Triples() {
	}

	/**
	 * Allows to search a pattern of triples.
	 *
	 * The returned iterator must be cleaned using C++ "delete" keyword.
	 *
	 * @param pattern
	 * @return Iterator of TripleID matching the specified pattern.
	 */
	virtual IteratorTripleID *search(TripleID &pattern)=0;

	IteratorTripleID *searchAll() {
		TripleID all(0,0,0);
		return search(all);
	}

	/**
	 * Calculates the cost to retrieve a specific pattern
	 *
	 * @param triple
	 * @return
	 */
	virtual float cost(TripleID &triple)=0;

	/**
	 * Returns the number of triples
	 *
	 * @return
	 */
	virtual unsigned int getNumberOfElements()=0;

	/**
	 * Returns size in bytes of the overall structure.
	 */
	virtual unsigned int size()=0;

	/**
	 * Serialize the triples to a stream in implementation-specific format.
	 *
	 * @param output
	 * @return
	 */
	virtual bool save(std::ostream &output, ControlInformation &ci, ProgressListener *listener = NULL)=0;

	/**
	 * Generate triples structure from other triples structure.
	 * @param input
	 */
	virtual void load(ModifiableTriples &input, ProgressListener *listener = NULL)=0;

	/**
	 * Read the triples from a stream, using the same implementation-specific format.
	 *
	 * @param input
	 * @return
	 */
	virtual void load(std::istream &input, ControlInformation &ci, ProgressListener *listener = NULL)=0;

	/**
	 * Adds all known information about the triples to the Header.
	 *
	 * @param header
	 * @return
	 */
	virtual void populateHeader(Header &header, string rootNode)=0;

	virtual string getType()=0;
}; // Triples{}

/**
 * Provides readwrite access to a set of triples.
 *
 */
class ModifiableTriples: public Triples {
public:
	virtual ~ModifiableTriples() {
	}
	/**
	 * Adds one or more triples
	 *
	 * @param triples
	 *            The triples to be inserted
	 * @return boolean
	 */
	virtual bool insert(TripleID &triple)=0;

	virtual bool insert(IteratorTripleID *triples)=0;

	/**
	 * Deletes one or more triples according to a pattern
	 *
	 * @param pattern
	 *            The pattern to match against
	 * @return boolean
	 */
	virtual bool remove(TripleID &pattern)=0;

	virtual bool remove(IteratorTripleID *pattern)=0;

	/**
	 * Sorts the triples based on an order(TripleComponentOrder)
	 *
	 * @param order
	 *            The order to sort the triples with
	 */
	virtual void sort(TripleComponentOrder order, ProgressListener *listener = NULL)=0;

	virtual void removeDuplicates(ProgressListener *listener = NULL)=0;

	/**
	 * Sets a type of order(TripleComponentOrder)
	 *
	 * @param order
	 *            The order to set
	 */
	virtual void setOrder(TripleComponentOrder order)=0;

	/**
	 *  Called before inserting a set of triples
	 *
	 */
	virtual void startProcessing(ProgressListener *listener = NULL)=0;

	/**
	 * Called after all triples have been inserted.
	 */
	virtual void stopProcessing(ProgressListener *listener = NULL)=0;
};

}

#endif /* HDT_TRIPLES_ */

