/*
 * File: LogSequence2.cpp
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

#include <iostream>
#include <limits>
#include <HDTVocabulary.hpp>
#include "LogSequence2.hpp"
#include "../libdcs/VByte.h"
#include "../util/crc8.h"
#include "../util/crc32.h"

using namespace std;

namespace hdt {

LogSequence2::LogSequence2() : numentries(0), numbits(32) {
	maxval = maxVal(numbits);
}

LogSequence2::LogSequence2(unsigned int numbits) : numentries(0), numbits(numbits) {
	maxval = maxVal(numbits);
}

LogSequence2::LogSequence2(unsigned int numbits, size_t capacity) : numentries(0), numbits(numbits) {
	maxval = maxVal(numbits);
	size_t totalSize = numElementsFor(numbits, capacity);
	array.resize(totalSize);
}

LogSequence2::~LogSequence2() {

}

size_t LogSequence2::get(size_t position)
{
	if(position>=numentries) {
		throw "Trying to get an element bigger than the array.";
	}

	return get_field(&array[0], numbits, position);
}

void LogSequence2::add(IteratorUInt &elements)
{
	size_t max = 0;
	numentries = 0;

	// Count and calculate number of bits needed per element.
	while(elements.hasNext()) {
		size_t element = elements.next();
		max = element > max ? element : max;
		numentries++;
	}

	// Prepare array
	numbits = bits(max);
	array.clear();
	array.resize(numElementsFor(numbits, numentries));

	// Save
	elements.goToStart();
	size_t count = 0;
	while(elements.hasNext()) {
		size_t element = elements.next();
		set_field(&array[0], numbits, count, element);
		count++;
	}

}

void LogSequence2::set(size_t position, size_t value) {
	if(position>numentries) {
		throw "Trying to modify a position out of the structure capacity. Use push_back() instead";
	}
	if(value>maxval) {
		throw "Trying to insert a value bigger that expected. Please increase numbits when creating the data structure.";
	}
	set_field(&array[0], numbits, position, value);
}

void LogSequence2::push_back(size_t value) {
	size_t neededSize = numElementsFor(numbits, numentries+1);
	if(array.size()<neededSize) {
		//array.resize(neededSize);
		array.push_back(0);		// Vector.push_back automatically doubles space when needed.
	}
	if(value>maxval) {
		throw "Trying to insert a value bigger that expected. Please increase numbits when creating the data structure.";
	}
	set(numentries, value);
	numentries++;
}

void LogSequence2::reduceBits() {
	size_t max = 0;
	for(size_t i=0;i<numentries;i++) {
		size_t value = get(i);
		max = value>max ? value : max;
	}
	unsigned int newbits = bits(max);

	if(newbits<numbits) {
		// Go through elements, read one and write one.
		// Since the number of bits is smaller they don't overlap.
		for(size_t i=0;i<numentries;i++) {
			size_t value = get_field(&array[0], numbits, i);
			set_field(&array[0], newbits, i, value);
		}
		numbits = newbits;
		maxval = maxVal(numbits);
		size_t totalSize = numElementsFor(numbits, numentries);
		array.resize(totalSize);	// Warning: It does not deallocate memory :(
	}
}

void LogSequence2::load(std::istream & input)
{
	CRC8 crch;
	CRC32 crcd;
	unsigned char data[9];

	// Read numbits
	crch.readData(input, (unsigned char*)&numbits, sizeof(numbits));

	// Read numentries
	uint64_t numentries64 = csd::VByte::decode(input);
	unsigned int pos = csd::VByte::encode(data, numentries64);
	crch.update(data, pos);

	// Validate Checksum Header
	crc8_t filecrch = crc8_read(input);
	if(crch.getValue()!=filecrch) {
		throw "Checksum error while reading LogSequence2 header.";
	}

	// Update local variables and validate
	maxval = maxVal(numbits);
	numentries = (size_t) numentries64;
	if(numbits>sizeof(size_t)*8 || numentries64>std::numeric_limits<size_t>::max()) {
		throw "This data structure is too big for this machine";
	}

	// Calculate data size, reserve buffer.
	size_t numbytes = numBytesFor(numbits, numentries);
	array.resize(numElementsFor(numbits, numentries));

	// Read data
	crcd.readData(input, (unsigned char*)&array[0], numbytes);

	// Validate checksum data
	crc32_t filecrcd = crc32_read(input);
	if(crcd.getValue()!=filecrcd) {
		throw "Checksum error while reading LogSequence2 Data";
	}
}

void LogSequence2::save(std::ostream & out)
{
	CRC8 crch;
	CRC32 crcd;
	unsigned char data[9];
	unsigned int len;

	// Write numbits
	crch.writeData(out, &numbits, sizeof(numbits));

	// Write numentries
	len=csd::VByte::encode(data, numentries);
	crch.writeData(out, data, len);

	// Write Header CRC
	crch.writeCRC(out);

	// Write data
	size_t numbytes = numBytesFor(numbits, numentries);
	crcd.writeData(out, (unsigned char*)&array[0], numbytes);

	// Write Data CRC
	crcd.writeCRC(out);
}

size_t LogSequence2::getNumberOfElements()
{
	return numentries;
}

size_t LogSequence2::size()
{
	return numBytesFor(numbits, numentries)+sizeof(LogSequence2)+sizeof(vector<size_t>);
}

std::string LogSequence2::getType()
{
	return HDTVocabulary::SEQ_TYPE_LOG2;
}



}
