/*
 * HDT.hpp
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

#ifndef HDT_
#define HDT_

#include <RDF.hpp>
#include <HDTSpecification.hpp>
#include <HDTEnums.hpp>
#include <Header.hpp>
#include <Dictionary.hpp>
#include <Triples.hpp>

#include <istream>

namespace hdt {

class HDT : public RDFAccess
{
public:

	virtual ~HDT() { };

	/**
	 * Obtain the Header part of this HDT.
	 */
	virtual Header &getHeader() = 0;

	/**
	 * Obtain the Dictionary part of this HDT
	 */
	virtual Dictionary &getDictionary() = 0;

	/**
	 * Obtain the Triples of this HDT.
	 */
	virtual Triples &getTriples() = 0;

	/**
	 * Load an RDF file represented in the specified RDFNotation into the current HDT.
	 * @param input
	 * @param specification
	 */
	virtual void loadFromRDF(std::istream &input, RDFNotation notation) = 0;

	/**
	 * Load an HDT file from a stream.
	 * @param input
	 */
	virtual void loadFromHDT(std::istream &input) = 0;

	/**
	 * Export the current HDT to RDF in the specified notation.
	 * @param output
	 * @param notation
	 */
	virtual void saveToRDF(std::ostream &output, RDFNotation notation) = 0;

	/**
	 * Save the current HDT into a stream in a compact manner.
	 * @param output
	 */
	virtual void saveToHDT(std::ostream &output) = 0;


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

};

class ModifiableHDT : public HDT {
public:

	virtual ~ModifiableHDT(){ }

	/**
	 *
	 * @param triples
	 */
	virtual void insert(TripleString &triple) = 0;

	virtual void insert(IteratorTripleString *triple) = 0;

	/**
	 * Deletes with pattern matching
	 *
	 * @param triples
	 */
	virtual void remove(TripleString &triples) = 0;

	virtual void remove(IteratorTripleString *triples) = 0;
};

}

#endif /* HDT_ */
