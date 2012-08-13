/*
 * File: AdjacencyList.cpp
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

#include "AdjacencyList.hpp"

namespace hdt {

AdjacencyList::AdjacencyList(IntSequence *el, cds_static::BitSequence *bit) :
		elements(el),
		bitmap(bit) {

}

AdjacencyList::~AdjacencyList() {

}

/**
 * Find pos of the list x
 * @param x
 * @return
 */
size_t AdjacencyList::find(size_t x) {
	if(x<=0) {
		return 0;
	}
#ifdef OLD_BITMAP
	unsigned int first1 = bitmap->select1(x);
	return bitmap->rank0(first1);
#else
	return bitmap->select1(x)+1;
#endif
}

size_t AdjacencyList::findNext(size_t pos)
{
    return bitmap->selectNext1(pos);
}

/**
 * Find element y, in the list x
 * @param x
 * @param y
 * @return
 */
size_t AdjacencyList::find(size_t x, size_t y) {
	size_t begin = find(x);
	size_t end = last(x);
	return binSearch(y, begin, end);
}

/**
 * Find to which list x does the element at globalpos belongs
 * @param globalpos
 * @return
 */
size_t AdjacencyList::findListIndex(size_t globalpos) {
#ifdef OLD_BITMAP
	size_t posz = bitmap->select0(globalpos+1);
	return bitmap->rank1(posz);
#else
	if(globalpos<=0) {
		return 0;
	}
	return bitmap->rank1(globalpos-1);
#endif
}

/**
 * Find last pos of the list x
 * @param x
 * @return
 */
size_t AdjacencyList::last(size_t x) {
#ifdef OLD_BITMAP
	size_t first1 = bitmap->select1(x+1);
	return bitmap->rank0(first1)-1;
#else
	return bitmap->select1(x+1);
#endif
}

/**
 * Count the number of lists in the AdjacencyList
 * @return
 */
size_t AdjacencyList::countListsX() {
#ifdef OLD_BITMAP
	return bitmap->countOnes();
#else
	return bitmap->countOnes();
#endif
}

/**
 * Count the number of items of the list x
 * @param x
 * @return
 */
size_t AdjacencyList::countItemsY(size_t x) {
	return last(x)-find(x)+1;
}

size_t AdjacencyList::search(unsigned int element, size_t begin, size_t end) {
	if(end-begin>10) {
		return binSearch(element,begin,end);
	} else {
		return linSearch(element,begin,end);
	}
}

size_t AdjacencyList::binSearch(unsigned int element, size_t begin, size_t end) {
	while (begin <= end) {
		int mid = (begin + end) / 2;

		unsigned int read = elements->get(mid);

		if (element > read)
			begin = mid + 1;
		else if (element < read)
			end = mid - 1;
		else
			return mid;
	}
	throw "Not found";
}

size_t AdjacencyList::linSearch(unsigned int element, size_t begin, size_t end) {
	while (begin <= end) {
		unsigned int read = elements->get(begin);
		//cout << "\t\tPos: " << begin << " Compare " << element << " with " << read << endl;

		if (element == read)
			return begin;

		begin++;
	}
	return end;
}


unsigned int AdjacencyList::get(size_t pos) {
	return elements->get(pos);
}

size_t AdjacencyList::getSize() {
	return elements->getNumberOfElements();
}

void AdjacencyList::dump() {
	for(unsigned int i=0; i<countListsX() && i<100; i++) {
		cout << " [";
		for(unsigned int j=0; j<countItemsY(i) && i<100; j++) {
			cout << get(find(i)+j);

			if(j!=countItemsY(i)-1)
				cout << ",";
		}
		cout << "] ";
	}
	cout << endl;

#if 0
	cout << "List has " << getSize() << " elements in " << countListsX() << " lists" << endl;

	for(unsigned int i=0; i<countListsX(); i++) {
		cout << "List " << i << " [" << find(i) << ", " << last(i) << "] (" << countItemsY(i)<< ") "<< endl;
		for(unsigned int j=0; j<countItemsY(i); j++) {
			cout << "\tItem " << i << ", "<< j << " => " << get(find(i)+j) << " Found pos: " << find(i, get(find(i)+j)) << " Found list: " << findListIndex(find(i)+j) << endl;
		}
	}
#endif
}

}
