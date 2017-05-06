/*
 * File: IntSequence.hpp
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

#ifndef STREAMELEMENTS_HPP_
#define STREAMELEMENTS_HPP_

#include <vector>
#include <iostream>

#include <HDTListener.hpp>

#include <Iterator.hpp>

namespace hdt {

enum SequenceType {
	SEQ_TYPE_LOG=1,
	SEQ_TYPE_INT32,
	SEQ_TYPE_INT64,
	SEQ_TYPE_HUFFMAN,
	SEQ_TYPE_WAVELET,
	SEQ_TYPE_LOG2,
};
/*
class IteratorUInt {
public:
	virtual ~IteratorUInt() { }

	virtual bool hasNext() {
		return false;
	}

    virtual size_t next() {
		return 0;
	}

	virtual void goToStart() {
	}
};
*/


class IntSequence {

public:
	virtual ~IntSequence() { }

	/**
	 * Adds elementss to the stream
	 *
	 * @param elements
	 *            The elements to be added to the stream
	 */
	virtual void add(IteratorUInt &elements)=0;

	/**
	 * Gets the element in a specific position
	 *
	 * @param position
	 *            The position of the element to be returned
	 * @return int
	 */
	virtual size_t get(size_t position)=0;

	/**
	 * Gets the total number of elements in the stream
	 *
	 * @return int
	 */
	virtual size_t getNumberOfElements()=0;

	/**
	 * Returns the size of the stream in bytes
	 *
	 * @return int
	 */
	virtual size_t size()=0;

	/**
	 * Saves the stream to an OutputStream
	 *
	 * @param output
	 *            The OutputStream to be saved to
	 * @throws IOException
	 */
	virtual void save(std::ostream &output)=0;

	/**
	 * Loads a stream from an InputStream
	 *
	 * @param input
	 *            The InputStream to load from
	 */
	virtual void load(std::istream &input)=0;

    virtual size_t load(const unsigned char *ptr, const unsigned char *ptrMax, ProgressListener *listener=NULL)=0;

	virtual std::string getType()=0;

	static IntSequence *getArray(std::string type);

	static IntSequence *getArray(unsigned char type);

	static IntSequence *getArray(std::istream &input);
};

// Iterator of a Stream
class StreamIterator : public IteratorUInt {
private:
	IntSequence *stream;
	size_t pos;

public:
	StreamIterator(IntSequence *elements) : stream(elements), pos(0) {
	}

	bool hasNext(){
		return pos<stream->getNumberOfElements();
	}
    size_t next(){
        return stream->get(pos++);
	}
	void goToStart() {
		pos=0;
	}
};


// Iterator using C++ vector<size_t>
class VectorUIntIterator : public IteratorUInt {
private:
    std::vector<size_t> &triples;
	size_t idx;

public:
    VectorUIntIterator(std::vector<size_t> &v) : triples(v), idx(0) { }

	virtual ~VectorUIntIterator() {	}

	bool hasNext() {
		return idx<triples.size();
	}

    size_t next() {
		return triples[idx++];
	}

	void goToStart() {
		idx=0;
	}
};

// Iterator using C-Style array + count.
class IteratorUintArray : public IteratorUInt {
private:
    size_t *array;
	size_t arraySize;
	size_t idx;

public:
    IteratorUintArray(size_t *array, size_t arraySize) : array(array), arraySize(arraySize) { }

	virtual ~IteratorUintArray() {	}

	bool hasNext() {
		return idx<arraySize;
	}

    size_t next() {
		return array[idx++];
	}

	void goToStart() {
		idx=0;
	}
};

}

#endif /* STREAMELEMENTS_HPP_ */
