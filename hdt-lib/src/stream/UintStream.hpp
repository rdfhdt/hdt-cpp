/*
 * UintStream.hpp
 *
 *  Created on: 10/05/2011
 *      Author: mck
 */

#ifndef UINTSTREAM_HPP_
#define UINTSTREAM_HPP_

#include "StreamElements.hpp"

namespace hdt {


#if 0
// Example of Iterator using c++ vector
class IteratorUintVector : public IteratorUint{
private:
	std::vector<unsigned int> &vector;
	unsigned int idx;

public:
	IteratorUintVector(std::vector<unsigned int> &v) : vector(v), idx(0) { }

	virtual ~IteratorUintVector() {	}

	virtual bool hasNext() {
		return idx<vector.size();
	}

	virtual unsigned int next() {
		return vector[idx++];
	}
};
#endif


class UintStream : public StreamElements {
private:
	std::vector<unsigned int> vector;

public:
	UintStream();
	virtual ~UintStream();

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

#endif /* UINTSTREAM_HPP_ */
