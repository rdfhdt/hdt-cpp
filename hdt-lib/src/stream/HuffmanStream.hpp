/*
 * HuffmanStream.hpp
 *
 *  Created on: 19/05/2011
 *      Author: mck
 */

#ifndef HUFFMANSTREAM_HPP_
#define HUFFMANSTREAM_HPP_

#include "StreamElements.hpp"

#include "../huffman/Huffman.h"

namespace hdt {

class HuffmanStream : public StreamElements {

private:
	// FIXME: Make a version where the compressed version is kept in memory and uncompressed on demand.
	std::vector<unsigned int> vectorPlain;
	URICompressed::Huffman *huffman;

public:
	HuffmanStream();
	virtual ~HuffmanStream();

	/* Adds elementss to the stream
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



#endif /* HUFFMANSTREAM_HPP_ */
