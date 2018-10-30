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
#include <stdexcept>
#include <iostream>
#include <limits>
#include <HDTVocabulary.hpp>
#include "LogSequence2.hpp"
#include "../libdcs/VByte.h"
#include "../util/crc8.h"
#include "../util/crc32.h"

using namespace std;

namespace hdt {

LogSequence2::LogSequence2() : numbits(32), numentries(0), IsMapped(false) {
	maxval = maxVal(numbits);

    data.resize(1);
    array = &data[0];
    arraysize = 0;
}

LogSequence2::LogSequence2(unsigned int numbits) : numbits(numbits), numentries(0), IsMapped(false) {
	maxval = maxVal(numbits);

    data.resize(1);
    array = &data[0];
    arraysize = 0;
}

LogSequence2::LogSequence2(unsigned int numbits, size_t capacity) : numbits(numbits), numentries(0), IsMapped(false) {
	maxval = maxVal(numbits);
	size_t totalSize = numElementsFor(numbits, capacity);
    if(totalSize==0) data.reserve(1);
    data.resize(totalSize);
	array = &data[0];
	arraysize = totalSize;
}

LogSequence2::LogSequence2(unsigned int numbits, size_t capacity, bool initialize) : numbits(numbits), IsMapped(false) {
    maxval = maxVal(numbits);
    size_t totalSize = numElementsFor(numbits, capacity);
    if(totalSize==0) data.reserve(1);
    data.resize(totalSize);
    array = &data[0];
    arraysize = totalSize;
    numentries=initialize ? capacity : 0;
}

LogSequence2::~LogSequence2() {

}

size_t LogSequence2::get(size_t position)
{
	if(position>=numentries) {
		throw std::runtime_error("Trying to get an element bigger than the array.");
	}

	return get_field(&array[0], numbits, position);
}

void LogSequence2::add(IteratorUInt &elements)
{
	if(IsMapped) {
		throw std::runtime_error("Data structure read-only when mapped.");
	}

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
	data.clear();
	data.resize(numElementsFor(numbits, numentries));
    array = &data[0];
	arraysize = data.size();

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
	if(IsMapped) {
		throw std::runtime_error("Data structure read-only when mapped.");
	}
	if(position>numentries) {
		throw std::runtime_error("Trying to modify a position out of the structure capacity. Use push_back() instead");
	}
	if(value>maxval) {
		throw std::runtime_error("Trying to insert a value bigger that expected. Please increase numbits when creating the data structure.");
	}
    set_field(array, numbits, position, value);
}

void LogSequence2::push_back(size_t value) {
	if(IsMapped) {
		throw std::runtime_error("Data structure read-only when mapped.");
	}
	size_t neededSize = numElementsFor(numbits, numentries+1);
	if(data.size()<neededSize) {
        data.resize(neededSize*2);
		arraysize=data.size();
        array = &data[0];
	}
	if(value>maxval) {
		throw std::runtime_error("Trying to insert a value bigger that expected. Please increase numbits when creating the data structure.");
	}
	set(numentries, value);
	numentries++;
}

void LogSequence2::resize(size_t newNumEntries) {
	if(IsMapped) {
		throw std::runtime_error("Data structure read-only when mapped.");
	}
	size_t neededSize = numElementsFor(numbits, newNumEntries);
	if(data.size()<neededSize) {
        data.resize(neededSize*2);
		arraysize=data.size();
        array = &data[0];
	}
	numentries=newNumEntries;
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
		data.resize(totalSize);	// Warning: It does not deallocate memory :(
		arraysize = data.size();
        array = &data[0];
	}
}

void LogSequence2::load(std::istream & input)
{
	CRC8 crch;
	CRC32 crcd;
	unsigned char buf[9];

	// Read type
	uint8_t type;
	crch.readData(input, (unsigned char*)&type, sizeof(type));
	if(type!=TYPE_SEQLOG) {
		//throw std::runtime_error("Trying to read a LOGArray but data is not LogArray");
	}

	// Read numbits
	crch.readData(input, (unsigned char*)&numbits, sizeof(numbits));

	// Read numentries
	uint64_t numentries64 = csd::VByte::decode(input);
    size_t pos = csd::VByte::encode(buf, numentries64);
	crch.update(buf, pos);

	// Validate Checksum Header
	crc8_t filecrch = crc8_read(input);
	if(crch.getValue()!=filecrch) {
		throw std::runtime_error("Checksum error while reading LogSequence2 header.");
	}

	// Update local variables and validate
	maxval = maxVal(numbits);
	numentries = (size_t) numentries64;
	if(numbits>sizeof(size_t)*8 || numentries64>std::numeric_limits<size_t>::max()) {
		throw std::out_of_range("This data structure is too big for this machine");
	}

	// Calculate data size, reserve buffer.
	size_t numbytes = numBytesFor(numbits, numentries);
	data.resize(numElementsFor(numbits, numentries));
	arraysize = data.size();
    array = &data[0];

	// Read data
	crcd.readData(input, (unsigned char*)&array[0], numbytes);

	// Validate checksum data
	crc32_t filecrcd = crc32_read(input);
	if(crcd.getValue()!=filecrcd) {
		throw std::runtime_error("Checksum error while reading LogSequence2 Data");
	}

	IsMapped = false;
}

#define CHECKPTR(base, max, size) if(((base)+(size))>(max)) throw std::runtime_error("Could not read completely the HDT from the file.");

size_t LogSequence2::load(const unsigned char *ptr, const unsigned char *ptrMax, ProgressListener *listener) {
	size_t count = 0;

	// Read type
	CHECKPTR(&ptr[count], ptrMax, 1);
	if(ptr[count]!=TYPE_SEQLOG) {
		throw std::runtime_error("Trying to read a LOGArray but data is not LogArray");
	}
	count++;

    // Read numbits
	CHECKPTR(&ptr[count], ptrMax, 1);
	numbits = ptr[count++];

    // Read numentries
    uint64_t numentries64;
    count += csd::VByte::decode(&ptr[count], ptrMax, &numentries64);

    // Validate Checksum Header
    CRC8 crch;
    crch.update(&ptr[0], count);
    CHECKPTR(&ptr[count], ptrMax, 1);
    if(crch.getValue()!=ptr[count++])
        throw std::runtime_error("Checksum error while reading LogSequence2 header.");

    // Update local variables and validate
    maxval = maxVal(numbits);
    numentries = (size_t) numentries64;
    if(numbits>sizeof(size_t)*8 || numentries64>std::numeric_limits<size_t>::max()) {
        throw std::runtime_error("This data structure is too big for this machine");
    }

    // Setup array of data
	arraysize = numBytesFor(numbits, numentries);
	array = (size_t *) &ptr[count];
	count+=arraysize;
    IsMapped = true;

    if(&ptr[count]>=ptrMax)
        throw std::runtime_error("LogSequence2 tries to read beyond the end of the file");

    CHECKPTR(&ptr[count], ptrMax, 4);
    count+=4; // CRC of data

	return count;
}

void LogSequence2::save(std::ostream & out)
{
	CRC8 crch;
	CRC32 crcd;
	unsigned char data[9];
    size_t len;

	// Write type
	uint8_t type = TYPE_SEQLOG;
	crch.writeData(out, &type, sizeof(uint8_t));

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
