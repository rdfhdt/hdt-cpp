/*
 * TriplesComparator.cpp
 *
 * Copyright (C) 2011, Javier D. Fernandez, Miguel A. Martinez-Prieto
 *                     Guillermo Rodriguez-Cano, Alejandro Andres,
 *                     Mario Arias
 * All rights reserved.
 *
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

#include "TriplesComparator.hpp"

namespace hdt {

TriplesComparator::TriplesComparator()
{
	// TODO Auto-generated constructor stub
} // TriplesComparator()

TriplesComparator::TriplesComparator(TripleComponentOrder order)
{
	this->order = order;
} // TriplesComparator()


TriplesComparator::~TriplesComparator()
{
	// TODO Auto-generated destructor stub
} // TriplesComparator()

bool TriplesComparator::operator()(const TripleID &a, const TripleID &b)
{
	//TODO: Revise because STL C++ only allows true if o1 > o2, false otherwise...

	int res;
	TripleID o1 = a;
	TripleID o2 = b;
	/*
	﻿ * Components of the triple. Meaning will be given based on the order
	﻿ * variable, see below
	﻿ */
	unsigned int x1 = 0, y1 = 0, z1 = 0, x2 = 0, y2 = 0, z2 = 0;

	// Some calculations...
	switch( this->order) {
		case SPO:
			// Subjects
			x1 = o1.getSubject();
			x2 = o2.getSubject();
			// Predicates
			y1 = o1.getPredicate();
			y2 = o2.getPredicate();
			// Objects
			z1 = o1.getObject();
			z2 = o2.getObject();
			break;
		case SOP:
			// Subjects
			x1 = o1.getSubject();
			x2 = o2.getSubject();
			// Predicates
			y1 = o1.getObject();
			y2 = o2.getObject();
			// Objects
			z1 = o1.getPredicate();
			z2 = o2.getPredicate();
			break;
		case PSO:
			// Subjects
			x1 = o1.getPredicate();
			x2 = o2.getPredicate();
			// Predicates
			y1 = o1.getSubject();
			y2 = o2.getSubject();
			// Objects
			z1 = o1.getObject();
			z2 = o2.getObject();
			break;
		case POS:
			// Subjects
			x1 = o1.getPredicate();
			x2 = o2.getPredicate();
			// Predicates
			y1 = o1.getObject();
			y2 = o2.getObject();
			// Objects
			z1 = o1.getSubject();
			z2 = o2.getSubject();
			break;
		case OSP:
			// Subjects
			x1 = o1.getObject();
			x2 = o2.getObject();
			// Predicates
			y1 = o1.getSubject();
			y2 = o2.getSubject();
			// Objects
			z1 = o1.getPredicate();
			z2 = o2.getPredicate();
			break;
		case OPS:
			// Subjects
			x1 = o1.getObject();
			x2 = o2.getObject();
			// Predicates
			y1 = o1.getPredicate();
			y2 = o2.getPredicate();
			// Objects
			z1 = o1.getSubject();
			z2 = o2.getSubject();
			break;
		default:
			break;
	}

	// Might as well use TripleID::compare()... right?
	// Actual comparison
	res = x1 - x2;
	// First component is the same
	if ( res == 0) {
		res = y1 - y2;
		// Second component is the same
		if (res == 0) {
			res = z1 - z2;
			// Third component is the same
			if ( res == 0) {
				return true;
			} else {
				return (z2 > z1);
			}
		} else {
			return (y2 > y1);
		}
	} else {
		return (x2 > x1);
	}

} // operator()

} // hdt{}
