/*
 * File: HDTEnums.cpp
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

#ifndef HDT_HDTENUMS_HPP_
#define HDT_HDTENUMS_HPP_

#include <string.h>

namespace hdt {

/**
 * Specifies the rol of a component.
 */
enum TripleComponentRole
{
	SUBJECT,	/** Subject */
	PREDICATE,	/** Predicate */
	OBJECT		/** Object */
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

enum HDTCost
{
	CONSTANT,
	LOG_S,
	LOG_P,
	LOG_O,
	SEQUENTIAL_S,
	SEQUENTIAL_P,
	SEQUENTIAL_O,
	SEQUENTIAL_ALL
};

enum TripleComponentOrder {
	/**
	 * Unknown order
	 */
	Unknown=0,
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

TripleComponentOrder parseOrder(const char *str);
const char *getOrderStr(TripleComponentOrder order);

enum RDFNotation {
	XML, // No longer supported.
	NTRIPLES,
	TURTLE,
	N3, // Not supported.
	NQUAD, // Deprecated: use `NQUADS' instead.
	JSON, // Not supported.
	NQUADS,
	TRIG,
};

enum ResultEstimationType {
    UNKNOWN,
    APPROXIMATE,
    UP_TO,
    EXACT
};

}

#endif /* HDT_HDTENUMS_HPP_ */
