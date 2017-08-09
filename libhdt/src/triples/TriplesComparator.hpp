/*
 * File: TriplesComparator.hpp
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

#ifndef TRIPLESCOMPARATOR_HPP_
#define TRIPLESCOMPARATOR_HPP_

#include <Triples.hpp>

namespace hdt {

class TriplesComparator {

	private:
		TripleComponentOrder order;

	public:
		TriplesComparator();
		TriplesComparator(TripleComponentOrder order);
		virtual ~TriplesComparator();
		bool operator()(const TripleID &a, const TripleID &b);
        void setOrder(TripleComponentOrder order);

}; //TriplesComparator{}

} // namespace hdt

#endif /* TRIPLESCOMPARATOR_HPP_ */
