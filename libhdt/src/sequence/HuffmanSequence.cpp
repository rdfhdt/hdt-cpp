/*
 * File: HuffmanSequence.cpp
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
#include "HuffmanSequence.hpp"

namespace hdt {

HuffmanSequence::HuffmanSequence() : huffman(NULL) {

}

HuffmanSequence::~HuffmanSequence() {
	if(huffman!=NULL) {
		delete huffman;
	}
}

size_t HuffmanSequence::get(size_t position)
{
	if(position<vectorPlain.size())
		return vectorPlain[position];

	return 0;
}

void HuffmanSequence::add(IteratorUInt &elements)
{
	// Fixme: Modify Huffman.cpp class to pass the frequencies and avoid copying.
	while(elements.hasNext()) {
		size_t element = elements.next();
		vectorPlain.push_back(element);
	}

	if(huffman!=NULL) {
		delete huffman;
	}

	huffman = new URICompressed::Huffman(&vectorPlain[0], vectorPlain.size());
}

void HuffmanSequence::load(std::istream & input)
{
	huffman = URICompressed::Huffman::load(input);
    size_t numElements;
    size_t encEntries;
	size_t pos = 0;

	input.read((char *)&numElements, sizeof(unsigned int));
	input.read((char *)&encEntries, sizeof(unsigned int));

	vectorPlain.resize(numElements);
	unsigned int *encV = new unsigned int[encEntries];

	input.read((char*)encV, encEntries*sizeof(unsigned int));

    for(size_t i=0;i<numElements;i++) {
		pos = huffman->decode(&vectorPlain[i], encV, pos);
	}
    delete encV;
}

size_t HuffmanSequence::load(const unsigned char *ptr, const unsigned char *ptrMax, ProgressListener *listener)
{
	 std::stringstream localStream;
	 localStream.rdbuf()->pubsetbuf((char*)ptr, ptrMax-ptr);

	 load(localStream);

	 return localStream.tellg();
}

void HuffmanSequence::save(std::ostream & output)
{
	if(huffman==NULL) {
		throw std::runtime_error("Must add elements to stream before saving");
	}

	// Create encoded vector
	std::vector<unsigned int> vectorEncoded;
	size_t maxEncodedElements = 1 + huffman->maxLength()*vectorPlain.size()/(sizeof(unsigned int)*8);
	vectorEncoded.resize(maxEncodedElements);

	// Encode stream
	size_t pos = 0;
    for(size_t i=0;i<vectorPlain.size();i++){
		pos = huffman->encode(vectorPlain[i], &vectorEncoded[0], pos );
	}

	// Calculate size
    size_t numElements = vectorPlain.size();
    size_t encodedEntries = 1+pos/(sizeof(unsigned int)*8);

#if 0
    size_t bytes = 1+pos/8;
	cout << "Total bits: " << pos << endl;
	cout << "Total entries: " << encodedEntries << endl;
	cout << "Total bytes: " << bytes << endl;
	cout << "Total elements: " << numElements << endl;
#endif

	// Write data to output
	huffman->save(output);
	output.write((char*)&numElements, sizeof(unsigned int));
	output.write((char*)&encodedEntries, sizeof(unsigned int));
	output.write((char*)&vectorEncoded[0], encodedEntries * sizeof(unsigned int));
}

size_t HuffmanSequence::getNumberOfElements()
{
	return vectorPlain.size();
}

size_t HuffmanSequence::size()
{
	if(huffman!=NULL)
		return huffman->getSize();

	return 0;
}

std::string HuffmanSequence::getType()
{
	return HDTVocabulary::SEQ_TYPE_HUFFMAN;
}

}
#else
int HuffmanSequenceDummySymbol;
#endif
