/*
 * LogStream.hpp
 *
 *  Created on: 17/05/2011
 *      Author: mck
 */

#ifndef LOGSTREAM_HPP_
#define LOGSTREAM_HPP_

#include "StreamElements.hpp"

#include <Array.h>

namespace hdt {

class LogStream : public StreamElements {

private:
	cds_utils::Array *array;

public:
	LogStream();
	virtual ~LogStream();

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

	friend class WaveletStream;
};

}

#endif /* LOGSTREAM_HPP_ */
