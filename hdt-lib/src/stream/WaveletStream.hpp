/*
 * WaveletStream.hpp
 *
 *  Created on: 17/05/2011
 *      Author: mck
 */

#ifndef WAVELETSTREAM_HPP_
#define WAVELETSTREAM_HPP_

#include "StreamElements.hpp"

#include <WaveletTreeNoptrs.h>

namespace hdt {

class WaveletStream : public StreamElements {

private:
	cds_static::Sequence *sequence;

public:
	WaveletStream();
	virtual ~WaveletStream();

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

	size_t rank(unsigned int symbol, size_t pos);
	size_t select(unsigned int symbol, size_t pos);
};

}

#endif /* WAVELETSTREAM_HPP_ */
