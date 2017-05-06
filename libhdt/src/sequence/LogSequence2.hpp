/*
 * File: LogSequence2.hpp
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

#ifndef LOGSTREAM2_HPP_
#define LOGSTREAM2_HPP_

#include <inttypes.h>
#include <iostream>
#include <vector>

#include "IntSequence.hpp"

namespace hdt {

class LogSequence2 : public IntSequence {

private:
	std::vector<size_t> data;
	size_t *array;
	size_t arraysize;
	unsigned char numbits;
	size_t numentries;
	size_t maxval;

	bool IsMapped;

	static const uint8_t TYPE_SEQLOG = 1;
	static const unsigned int W = sizeof(size_t)*8;

	/** size_t's required to represent n integers of e bits each */
	inline size_t numElementsFor(const size_t bitsField, const size_t numEntries) {
		return (((uint64_t)bitsField*numEntries+W-1)/W);
	}

	/** Number of bytes required to represent n integers of e bits each */
	inline size_t numBytesFor(const size_t bitsField, const size_t numEntries) {
		return ((uint64_t)bitsField*numEntries+7)/8;
	}

	/** Retrieve a given index from array A where every value uses len bits
	 * @param data Array
	 * @param bitsField Length in bits of each field
	 * @param index Position to store in
	 */
	inline size_t get_field(const size_t *data, const size_t bitsField, const size_t index) {
		size_t bitPos = index*bitsField;
		size_t i=bitPos/W;
		size_t j=bitPos%W;
		size_t result;
		if (j+bitsField <= W) {
			result = (data[i] << (W-j-bitsField)) >> (W-bitsField);
		} else {
			result = data[i] >> j;
			result = result | (data[i+1] << ( (W<<1) -j-bitsField)) >> (W-bitsField);
		}
		return result;
	}

	/** Store a given value in index into array A where every value uses bitsField bits
	 * @param data Array
	 * @param bitsField Length in bits of each field
	 * @param index Position to store in
	 * @param value Value to be stored
	 */
	inline void set_field(size_t *data, const size_t bitsField, const size_t index, const size_t value) {
		size_t bitPos = index*bitsField;
		size_t i=bitPos/W;
		size_t j=bitPos%W;

		size_t mask = ~(~((size_t)0) << bitsField) << j;
		data[i] = (data[i] & ~mask) | (value << j);

		if (j+bitsField>W) {
			mask = (~((size_t)0) << (bitsField+j-W));
			data[i+1] = (data[i+1] & mask)| value >> (W-j);
		}
	}

public:
	LogSequence2();
	/**
	 * Create a data structure where each entry needs numbits
	 */
	LogSequence2(unsigned int numbits);

	/**
	 * Create a data structure where each entry needs numbits, reserve enough space to store capacity entries.
	 */
	LogSequence2(unsigned int numbits, size_t capacity);


    /**
     * Create a data structure where each entry needs numbits, initialize the booked entries to zero, so getNumberOfElements
     * returns 0 and one is able to do set(pos) where pos<capacity.
     */
    LogSequence2(unsigned int numbits, size_t capacity, bool initialize);

	/**
	 * Destructor.
	 */
	virtual ~LogSequence2();

	/**
	 * Adds elementss to the stream
	 *
	 * @param elements
	 *            The elements to be added to the stream
	 */
	void add(IteratorUInt &elements);

	/**
	 * Gets the element in a specific position
	 *
	 * @param position
	 *            The position of the element to be returned
	 * @return int
	 */
	size_t get(size_t position);

	/**
	 * Sets the element in a specific position
	 *
	 * @param position The position of the new element
	 * @param value Value to be inserted.
	 *
	 */
	void set(size_t position, size_t value);

	/**
	 * Append an element to the back of the array.
	 */
	void push_back(size_t element);

	/**
	 * Resize the data structure so it fits numEntries.
	 * If it had more entries than requested, the data structure is shrinked and the remaining elements are discarded.
	 * If it had fewer elements than requested, it grows the data structure with empty values.
	 */
	void resize(size_t numEntries);

	/**
	 * Try to reduce the number of bits needed to store all the sequence. Useful when we don't know beforehand how many elements
	 * to store. We can estimate the higher bound of number of bits, then call reduceBits() to make sure that the data structure
	 * is compact. This operation does not require additional memory.
	 */
	void reduceBits();

	/**
	 * Gets the total number of elements in the stream
	 *
	 * @return int
	 */
	size_t getNumberOfElements();

	/**
	 * Gets the total size in bytes of the data structure.
	 */
	size_t size();

	/**
	 * Saves the stream to an OutputStream
	 *
	 * @param output
	 *            The OutputStream to be saved to
	 * @throws IOException
	 */
	void save(std::ostream &output);

	/**
	 * Loads a stream from an InputStream
	 *
	 * @param input
	 *            The InputStream to load from
	 */
	void load(std::istream &input);
	
	/**
	 * Load a stream from a pointer. To be used with MMAP.
	 */
    size_t load(const unsigned char *ptr, const unsigned char *ptrMax, ProgressListener *listener=NULL);

	/**
	 * Get type of the data structure.
	 */
	std::string getType();
};


inline unsigned int bits(size_t n) {
    unsigned int b = 0;
	while (n) { b++; n >>= 1; }
	return b;
}

inline size_t maxVal(unsigned int numbits) {
    // The << operator is undefined for the number of bits of the word,
    // Therefore we need to check for corner cases.
    if(numbits==32) {
        return 0xFFFFFFFFU;
    } else if(numbits==64) {
        return (size_t)0xFFFFFFFFFFFFFFFFULL;
    } else {
        return ~((size_t)-1<<numbits);
    }
}

}

#endif /* LOGSTREAM_HPP_ */
