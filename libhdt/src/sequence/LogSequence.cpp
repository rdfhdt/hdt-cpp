/*
 * File: LogSequence.cpp
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

#ifdef HAVE_CSD

#include <libcdsBasics.h>
#include <HDTVocabulary.hpp>
#include "LogSequence.hpp"

namespace hdt {

LogSequence::LogSequence() : array(NULL) {

}

LogSequence::~LogSequence() {
	if(array!=NULL)
		delete array;
}


size_t LogSequence::get(size_t position)
{
	if(array==NULL)
		return 0;

	if(position >= array->getLength()) {
		return 0;
	} else {
		return array->getField(position);
	}
}

void LogSequence::add(IteratorUInt &elements)
{
    std::vector<size_t> vector;
    size_t max = 0;

	while(elements.hasNext()) {
		size_t element = elements.next();
		if(element>((size_t)-1)) {
			throw std::out_of_range("Error, saving a value out of range");
		}
        vector.push_back(element);
		max = element > max ? element : max;
	}

	if(array!=NULL) {
		delete array;
		array=NULL;
	}

	array = new cds_utils::Array(vector, cds_utils::bits(max));
}

void LogSequence::load(std::istream & input)
{
	if(array!=NULL){
		delete array;
		array=NULL;
	}

    array = new cds_utils::Array(input);
}

size_t LogSequence::load(const unsigned char *ptr, const unsigned char *ptrMax, ProgressListener *listener)
{
	 std::stringstream localStream;
	 localStream.rdbuf()->pubsetbuf((char*)ptr, ptrMax-ptr);

	 load(localStream);

	 return localStream.tellg();
}

void LogSequence::save(std::ostream & output)
{
	if(array==NULL)
		return;

	array->save(output);
}

size_t LogSequence::getNumberOfElements()
{
	if(array!=NULL)
		return array->getLength();
	return 0;
}

size_t LogSequence::size()
{
	if(array!=NULL)
		return array->getSize();

	return 0;
}

std::string LogSequence::getType()
{
	return HDTVocabulary::SEQ_TYPE_LOG2;
}

}
#else
int LogSequenceDummySymbol;
#endif
