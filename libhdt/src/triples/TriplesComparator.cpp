/*
 * File: TriplesComparator.cpp
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
#include <stdexcept>
#include "TriplesComparator.hpp"

namespace hdt {

TriplesComparator::TriplesComparator()
{
}

TriplesComparator::TriplesComparator(TripleComponentOrder order)
{
	this->order = order;
}


TriplesComparator::~TriplesComparator()
{
}

bool TriplesComparator::operator()(const TripleID &a, const TripleID &b)
{
	//TODO: Revise because STL C++ only allows true if a > b, false otherwise...
	/*
	﻿ * Components of the triple. Meaning will be given based on the order
	﻿ * variable, see below
	﻿ */
    size_t x1, y1, z1, x2, y2, z2;

	// Some calculations...
	switch( this->order) {
		case Unknown:
		case SPO:
			// Subjects
			x1 = a.getSubject();
			x2 = b.getSubject();
			// Predicates
			y1 = a.getPredicate();
			y2 = b.getPredicate();
			// Objects
			z1 = a.getObject();
			z2 = b.getObject();
			break;
		case SOP:
			// Subjects
			x1 = a.getSubject();
			x2 = b.getSubject();
			// Predicates
			y1 = a.getObject();
			y2 = b.getObject();
			// Objects
			z1 = a.getPredicate();
			z2 = b.getPredicate();
			break;
		case PSO:
			// Subjects
			x1 = a.getPredicate();
			x2 = b.getPredicate();
			// Predicates
			y1 = a.getSubject();
			y2 = b.getSubject();
			// Objects
			z1 = a.getObject();
			z2 = b.getObject();
			break;
		case POS:
			// Subjects
			x1 = a.getPredicate();
			x2 = b.getPredicate();
			// Predicates
			y1 = a.getObject();
			y2 = b.getObject();
			// Objects
			z1 = a.getSubject();
			z2 = b.getSubject();
			break;
		case OSP:
			// Subjects
			x1 = a.getObject();
			x2 = b.getObject();
			// Predicates
			y1 = a.getSubject();
			y2 = b.getSubject();
			// Objects
			z1 = a.getPredicate();
			z2 = b.getPredicate();
			break;
		case OPS:
			// Subjects
			x1 = a.getObject();
			x2 = b.getObject();
			// Predicates
			y1 = a.getPredicate();
			y2 = b.getPredicate();
			// Objects
			z1 = a.getSubject();
			z2 = b.getSubject();
			break;
		default:
			throw std::runtime_error("Invalid TripleComponentOrder");
	}

	// Might as well use TripleID::compare()... right?
	// Actual comparison
	int res = x1 - x2;
	// First component is the same
	if ( res == 0) {
		res = y1 - y2;
		// Second component is the same
		if (res == 0) {
			res = z1 - z2;
			// Third component is the same
			if ( res == 0) {
				return false;
			} else {
				return (z2 > z1);
			}
		} else {
			return (y2 > y1);
		}
	} else {
		return (x2 > x1);
    }

}

void TriplesComparator::setOrder(TripleComponentOrder order)
{
    this->order = order;
}

} // hdt{}
