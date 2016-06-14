/*
 * File: HDT.hpp
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

#ifndef HDT_HDT_HPP_
#define HDT_HDT_HPP_

#include "RDF.hpp"
#include "HDTSpecification.hpp"
#include "HDTEnums.hpp"
#include "HDTListener.hpp"
#include "Header.hpp"
#include "Dictionary.hpp"
#include "Triples.hpp"
#include "RDFParser.hpp"
#include "RDFSerializer.hpp"

#include <iostream>
#include <set>

namespace hdt {

/**
 * Main HDT class. Represents an abstract access to a HDT object.
 * It provides methods to get the Dictionary, Header and Triples.
 * It also allows to search for simple triple patterns.
 */
class HDT : public RDFAccess
{
public:
	virtual ~HDT() { };

	/**
	 * Obtain the Header part of this HDT.
	 */
	virtual Header *getHeader() = 0;

	/**
	 * Obtain the Dictionary part of this HDT
	 */
	virtual Dictionary *getDictionary() = 0;

	/**
	 * Obtain the Triples of this HDT.
	 */
	virtual Triples *getTriples() = 0;

	/**
	 * Export the current HDT to RDF in the specified notation.
	 * @param output
	 * @param notation
	 */
	virtual void saveToRDF(RDFSerializer &serializer, ProgressListener *listener = NULL) = 0;

	/**
	 * Save the current HDT into a file in a compact manner.
	 * @param output
	 */
	virtual void saveToHDT(const char *fileName, ProgressListener *listener = NULL) = 0;

	/**
	 * Save the current HDT into a stream in a compact manner.
	 * @param output
	 */
	virtual void saveToHDT(std::ostream &out, ProgressListener *listener = NULL) = 0;

	/*
	 * FROM RDFAccess
	 */

	/**
	 * Search all triples that match the specified pattern.
	 * @param subject
	 * @param predicate
	 * @param object
	 * @return
	 */
	virtual IteratorTripleString *search(const char *subject, const char *predicate, const char *object) = 0;

    IteratorTripleString *search(TripleString &pattern) {
        return search(pattern.getSubject().c_str(), pattern.getPredicate().c_str(), pattern.getObject().c_str());
    }

    virtual bool isIndexed() const = 0;
};

/**
 * ModifiableHDT is a HDT that provides read/write operations. In addition to read operations,
 * it allows to insert and remove triples.
 */
class ModifiableHDT : public HDT {
public:
	virtual ~ModifiableHDT(){ }

	/**
	 * Insert a single triple to the HDT.
	 * @param triple Triple to be added.
	 */
	virtual void insert(TripleString &triple) = 0;

	/**
	 * Insert a set of triples to the HDT.
	 * @param triples Iterator to a set of triples to be added.
	 */
	virtual void insert(IteratorTripleString *triples) = 0;

	/**
	 * Remove a set of triples that match the specified pattern.
	 *
	 * @param triples
	 */
	virtual void remove(TripleString &triples) = 0;

	/**
	 * Remove all triples that match any of the specified patterns.
	 * @param triples
	 */
	virtual void remove(IteratorTripleString *triples) = 0;

};

} // namespace hdt

#endif /* HDT_HDT_HPP_ */
