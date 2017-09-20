/*
 * File: ArraySequence.cpp
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

#include <HDTVocabulary.hpp>
#include <sstream>
#include "ArraySequence.hpp"

namespace hdt {

ArraySequence::ArraySequence() {

}

ArraySequence::~ArraySequence() {
}

size_t ArraySequence::get(size_t position)
{
	return vector[position];
}

void ArraySequence::add(IteratorUInt &elements)
{
	size_t count = 0;
	while(elements.hasNext()) {
		size_t value = elements.next();
		vector.push_back(value);
		count++;
	}
}

void ArraySequence::load(std::istream & input)
{
	size_t numRead=0;
	size_t numElements;
	size_t readItem;
	input.read((char *)&numElements, sizeof(size_t));

	vector.reserve(numElements);

	while(input.good() && numRead<numElements) {
		input.read((char *) &readItem, sizeof(size_t));
		vector.push_back(readItem);
		numRead++;
    }
}

size_t ArraySequence::load(const unsigned char *ptr, const unsigned char *ptrMax, ProgressListener *listener)
{
	 std::stringstream localStream;
	 localStream.rdbuf()->pubsetbuf((char*)ptr, ptrMax-ptr);

	 load(localStream);

	 return localStream.tellg();
}

void ArraySequence::save(std::ostream & output)
{
	size_t numElements = vector.size();

	output.write((char *)&numElements, sizeof(size_t));

	output.write((char *)&vector[0], sizeof(size_t)*numElements);
}

size_t ArraySequence::getNumberOfElements()
{
	return vector.size();
}

size_t ArraySequence::size()
{
	return vector.size()*sizeof(size_t);
}

std::string ArraySequence::getType()
{
	return HDTVocabulary::SEQ_TYPE_INT32;
}

}
