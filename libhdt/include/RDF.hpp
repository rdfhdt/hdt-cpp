/*
 * File: RDF.hpp
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

#ifndef HDT_RDF_HPP_
#define HDT_RDF_HPP_

#include "SingleTriple.hpp"
#include "Iterator.hpp"

namespace hdt {

/**
 * Interface that provides read/search access to a set of triples.
 */

class RDFAccess {
public:
	virtual ~RDFAccess() { }

	/**
	 * Returns an iterator over all results matching the provided triple pattern.
	 * @param subject
	 * @param predicate
	 * @param object
	 * @return
	 */
	virtual IteratorTripleString *search(const char *subject, const char *predicate, const char *object) = 0;
};

/**
 * Provides writable access to a RDF repository, search triples, insert, remove.
 */
class RDFStorage : public RDFAccess {
public:
	virtual ~RDFStorage() { }

	/**
	 * Insert a new triple to the store.
	 * @param triple
	 */
	virtual void insert(TripleString &triple) = 0;

	/**
	 * Insert a set of triples to the store, as specified in the iterator.
	 * @param triple
	 */
	virtual void insert(IteratorTripleString *triple) = 0;

	/**
	 * Removes the set of triples that match the specified pattern.
	 *
	 * @param triples
	 */
	virtual void remove(TripleString &triple) = 0;

	/**
	 * Remove all triples from the storage.
	 */
	virtual void clear()=0;
};

}

#endif /* HDT_RDF_HPP_ */
