/*
 * TriplePosition.hpp
 *
 * Copyright (C) 2011, Javier D. Fernandez, Miguel A. Martinez-Prieto
 *                     Guillermo Rodriguez-Cano, Alejandro Andres,
 *                     Mario Arias
 * All rights reserved.
 *
 * Abstract class to implement different forms of the Dictionary component
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
 *   Alejandro Andres:
 *   Mario Arias:
 *
 * @version $Id$
 *
 */

#ifndef HDT_ENUMS_
#define HDT_ENUMS_

namespace hdt {

	enum TripleComponentRole
	{
		SUBJECT,
		PREDICATE,
		OBJECT
	};

	enum DictionarySection
	{
		SHARED_SUBJECT,
		NOT_SHARED_SUBJECT,
		SHARED_OBJECT,
		NOT_SHARED_OBJECT,
		NOT_SHARED_PREDICATE
	};

	enum Mapping
	{
		MAPPING1,
		MAPPING2
	};

	enum TripleComponentOrder {
		/**
		 * Subject, predicate, object
		 */
		SPO,
		/**
		 * Subject, object, predicate
		 */
		SOP,
		/**
		 * Predicate, subject, object
		 */
		PSO,
		/**
		 * Predicate, object, subject
		 */
		POS,
		/**
		 * Object, subject, predicate
		 */
		OSP,
		/**
		 * Object, predicate, subject
		 */
		OPS
	};

	enum RDFNotation {
		/**
		 * XML notation
		 *
		 * @see <a href="http://en.wikipedia.org/wiki/RDF/XML">Wikipedia</a>
		 */
		XML,
		/**
		 * N-TRIPLES notation
		 *
		 * @see <a href="http://en.wikipedia.org/wiki/N-Triples">Wikipedia</a>
		 */
		NTRIPLE,
		/**
		 * TURTLE notation
		 *
		 * @see <a href="http://en.wikipedia.org/wiki/Turtle_(syntax)">Wikipedia</a>
		 */
		TURTLE,
		/**
		 * Notation 3 notation
		 *
		 * @see <a href="http://en.wikipedia.org/wiki/Notation_3">Wikipedia</a>
		 */
		N3
	};
}

#endif
