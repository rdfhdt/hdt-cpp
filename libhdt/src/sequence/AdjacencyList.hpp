/*
 * File: AdjacencyList.hpp
 * Last modified: $Date: 2011-08-21 05:35:30 +0100 (dom, 21 ago 2011) $
 * Revision: $Revision: 180 $
 * Last modified by: $Author: mario.arias $
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


#ifndef ADJACENCYLIST_HPP_
#define ADJACENCYLIST_HPP_

#include "IntSequence.hpp"
#include "../bitsequence/BitSeq.h"

namespace hdt {

class AdjacencyList {
private:
	IntSequence *elements;
	BitSeq *bitmap;

public:
	AdjacencyList(IntSequence *el, BitSeq *bit);
	virtual ~AdjacencyList();

	size_t find(size_t x);
	size_t find(size_t x, size_t y);

        size_t findNext(size_t pos);

	size_t findListIndex(size_t globalpos);

	size_t last(size_t x);

	size_t countListsX();
	size_t countItemsY(size_t x);

    size_t search(size_t element, size_t ini, size_t fin);
    size_t binSearch(size_t element, size_t ini, size_t fin);
    size_t linSearch(size_t element, size_t ini, size_t fin);

    size_t findNextAppearance(size_t oldpos, size_t element);

    size_t findPreviousAppearance(size_t oldpos, size_t element);

    size_t get(size_t pos);

	size_t getSize();

	void dump();
};

}

#endif /* ADJACENCYLIST_HPP_ */
