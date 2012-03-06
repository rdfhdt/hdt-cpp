/*
 * LogStream2.hpp
 *
 *  Created on: 17/05/2011
 *      Author: mck
 */

#ifndef LOGSTREAM2_HPP_
#define LOGSTREAM2_HPP_

#include <iostream>
#include <vector>

#include "StreamElements.hpp"

namespace hdt {

inline unsigned int bits(unsigned int n) {
	unsigned int b = 0;
	while (n) { b++; n >>= 1; }
	return b;
}

class LogStream2 : public StreamElements {

private:
	std::vector<size_t> array;
	unsigned int numbits;
	unsigned int numelements;

	static const unsigned int W = sizeof(size_t)*8;

	/** size_ts required to represent n integers of e bits each */
	inline size_t numElementsFor(const size_t e, const size_t n) {
		return (((unsigned long long)e*n+64-1)/64) * (64/W); //+((unsigned long long)e*n%W>0));
	}

	/** Number of bytes required to represent n integers of e bits each, aligned to 64bit */
	inline size_t numBytesFor(const size_t e, const size_t n) {
		return (((unsigned long long)e*n+64-1)/(64)) * (64/8); //+((unsigned long long)e*n%W>0));
	}


	/** Retrieve a given index from array A where every value uses len bits
	 * @param A Array
	 * @param len Length in bits of each field
	 * @param index Position to be retrieved
	 */
	inline size_t get_field(const size_t *A, const size_t len, const size_t index) {
		size_t bitPos = index*len;
		size_t i=bitPos/W;
		size_t j=bitPos%W;
		size_t result;
		if (j+len <= W) {
			result = (A[i] << (W-j-len)) >> (W-len);
		} else {
			result = A[i] >> j;
			result = result | (A[i+1] << ( (W<<1) -j-len)) >> (W-len);
		}
		return result;
	}

	/** Store a given value in index into array A where every value uses len bits
	 * @param A Array
	 * @param len Length in bits of each field
	 * @param index Position to store in
	 * @param x Value to be stored
	 */
	inline void set_field(size_t *A, const size_t len, const size_t index, const size_t x) {
		size_t bitPos = index*len;
		size_t i=bitPos/W;
		size_t j=bitPos%W;

		size_t mask = ~(~((size_t)0) << len) << j;
		A[i] = (A[i] & ~mask) | (x << j);

		if (j+len>W) {
			mask = (~((size_t)0) << (len+j-W));
			A[i+1] = (A[i+1] & mask)| x >> (W-j);
		}
	}

	void swapBytesEndian();

public:
	LogStream2();
	LogStream2(unsigned int numbits);
	LogStream2(unsigned int numbits, unsigned int numelements);
	virtual ~LogStream2();

	/**
	 * Adds elementss to the stream
	 *
	 * @param elements
	 *            The elements to be added to the stream
	 */
	void add(IteratorUint &elements);

	/**
	 * Gets the element in a specific position
	 *
	 * @param position
	 *            The position of the element to be returned
	 * @return int
	 */
	unsigned int get(unsigned int position);

	void set(unsigned int position, unsigned int value);

	void push_back(unsigned int element);

	void reduceBits();

	/**
	 * Gets the total number of elements in the stream
	 *
	 * @return int
	 */
	unsigned int getNumberOfElements();

	unsigned int size();

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

	std::string getType();
};

}

#endif /* LOGSTREAM_HPP_ */
