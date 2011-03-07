/*
 * UintStream.hpp
 *
 *  Created on: 07/03/2011
 *      Author: mck
 */

#ifndef UINTSTREAM_HPP_
#define UINTSTREAM_HPP_

#include <vector>
#include <iostream>

namespace hdt {


// Todo: Move to new header file Iterators.hpp??
class IteratorUint {
public:
	virtual bool hasNext() {
		return false;
	}

	virtual unsigned int next() {
		return 0;
	}
};


// Example of Iterator using c++ vector
class IteratorUintVector : public IteratorUint{
private:
	std::vector<unsigned int> &vector;
	unsigned int idx;

public:
	IteratorUintVector(std::vector<unsigned int> v) : vector(v), idx(0) { }

	virtual ~IteratorUintVector() {	}

	virtual bool hasNext() {
		return idx<vector.size();
	}

	virtual unsigned int next() {
		idx++;
		return vector[idx];
	}
};

class UintStream {

public:
	/**
	 * Adds an element to the stream
	 *
	 * @param element
	 *            The element to be added to the stream
	 */
	virtual void add(IteratorUint &elements);

	/**
	 * Gets the element in a specific position
	 *
	 * @param position
	 *            The position of the element to be returned
	 * @return int
	 */
	virtual unsigned int get(unsigned int position);

	/**
	 * Gets the total number of elements in the stream
	 *
	 * @return int
	 */
	virtual unsigned int getNumberOfElements();

	/**
	 * Saves the stream to an OutputStream
	 *
	 * @param output
	 *            The OutputStream to be saved to
	 * @throws IOException
	 */
	virtual void save(std::ostream &output);

	/**
	 * Loads a stream from an InputStream
	 *
	 * @param input
	 *            The InputStream to load from
	 */
	void load(std::istream &input);
};


}

#endif /* UINTSTREAM_HPP_ */
