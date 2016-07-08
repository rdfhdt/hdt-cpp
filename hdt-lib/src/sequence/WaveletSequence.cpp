/*
 * File: WaveletSequence.cpp
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
#include <stdexcept>
#ifdef HAVE_CDS

#include <HDTVocabulary.hpp>

#include "WaveletSequence.hpp"

#include "LogSequence.hpp"

namespace hdt {

WaveletSequence::WaveletSequence() : sequence(NULL) {

}

WaveletSequence::WaveletSequence(IntSequence *otherStream) : sequence(NULL) {
	if(otherStream->getType()==HDTVocabulary::SEQ_TYPE_LOG) {
		LogSequence *logStream = static_cast<LogSequence *>(otherStream);
		cds_static::BitSequenceBuilder *builder = new cds_static::BitSequenceBuilderRG(2);
		cds_static::Mapper *mapper = new cds_static::MapperNone();
		sequence = new cds_static::WaveletTreeNoptrs(*logStream->array, builder, mapper);
	} else {
		StreamIterator iterator(otherStream);
		this->add(iterator);
	}
}

WaveletSequence::~WaveletSequence() {
	if(sequence!=NULL)
		delete sequence;
}


size_t WaveletSequence::get(size_t position)
{
	if(sequence==NULL)
		return 0;

	if(position >= sequence->getLength()) {
		return 0;
	} else {
		return sequence->access(position);
	}
}

void WaveletSequence::add(IteratorUInt &elements)
{
	std::vector<unsigned int> vector;

	while(elements.hasNext()) {
		size_t element = elements.next();

		if(element>((size_t)-1)) {
			throw std::runtime_error("Trying to insert a value bigger than the permitted for the architecture");
		}

		vector.push_back(element);
	}

	if(sequence!=NULL) {
		delete sequence;
		sequence=NULL;
	}

	cds_static::BitSequenceBuilder *builder = new cds_static::BitSequenceBuilderRG(20);
	cds_static::Mapper *mapper = new cds_static::MapperNone();

	sequence = new cds_static::WaveletTreeNoptrs(&vector[0], vector.size(), builder, mapper);
}

void WaveletSequence::load(std::istream & input)
{
	if(sequence!=NULL){
		delete sequence;
		sequence=NULL;
	}

    sequence = cds_static::Sequence::load(input);
}

size_t WaveletSequence::load(const unsigned char *ptr, const unsigned char *ptrMax, ProgressListener *listener)
{
	 std::stringstream localStream;
	 localStream.rdbuf()->pubsetbuf((char*)ptr, ptrMax-ptr);

	 load(localStream);

	 return localStream.tellg();
}

void WaveletSequence::save(std::ostream & output)
{
	if(sequence==NULL)
		return;

    sequence->save(output);
}

size_t WaveletSequence::getNumberOfElements()
{
	if(sequence!=NULL)
		return sequence->getLength();
	return 0;
}

size_t WaveletSequence::size()
{
	if(sequence!=NULL)
		return sequence->getSize();

	return 0;
}

std::string WaveletSequence::getType()
{
	return HDTVocabulary::SEQ_TYPE_WAVELET;
}


size_t WaveletSequence::rank(size_t symbol, size_t pos) {
	if(sequence!=NULL) {
		return sequence->rank(symbol, pos);
	}
	return 0;
}

size_t WaveletSequence::select(size_t symbol, size_t pos) {
	if(sequence!=NULL) {
		return sequence->select(symbol, pos);
	}
	return 0;
}

}
#else
int WaveletSequenceDummySymbol;
#endif
