/*
 * HDT.hpp
 *
 * Copyright (C) 2011, Javier D. Fernandez, Miguel A. Martinez-Prieto
 *                     Guillermo Rodriguez-Cano, Alejandro Andres,
 *                     Mario Arias
 * All rights reserved.
 *
 * TODO: Define me
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

#include <HDTSpecification.hpp>
#include <HDTEnums.hpp>
#include <Header.hpp>
#include <Dictionary.hpp>
#include <Triples.hpp>

#include <istream>

namespace hdt {

	class HDT
	{
		public:
			/*
			 * @param input
		     * @param specification
		     */
			virtual void loadFromRDF(std::istream &input) = 0;

			/**
		     * @param input
		     */
			virtual void loadFromHDT(std::istream &input) = 0;

		    /**
		     * @param output
		     * @param notation
		     */
		    virtual void saveToRDF(std::ostream &output, RDFNotation notation) = 0;

		    /**
		     * @param output
		     */
		    virtual void saveToHDT(std::ostream &output) = 0;

		    /**
		     * @param subject
		     * @param predicate
		     * @param object
		     * @return
		     */
		    virtual IteratorTripleString search(const char *subject, const char *predicate, const char *object) = 0;

		    /**
		     *
		     * @param triples
		     */
		    virtual void insert(TripleString &triple) = 0;

		    virtual void insert(IteratorTripleString &triple) = 0;

		    /**
		     * Deletes with pattern matching
		     *
		     * @param triples
		     */
		    virtual void remove(TripleString &triples) = 0;

		    virtual void remove(IteratorTripleString &triples) = 0;

			/**
			 *
			 */
			virtual Header &getHeader() = 0;

			/**
			 *
			 */
			virtual Dictionary &getDictionary() = 0;

			/**
			 *
			 */
			virtual Triples &getTriples() = 0;
	};

}

#endif /* HDT_ */
