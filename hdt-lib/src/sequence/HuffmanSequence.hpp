/*
 * File: HuffmanSequence.hpp
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

#ifndef HUFFMANSTREAM_HPP_
#define HUFFMANSTREAM_HPP_

#ifdef HAVE_CDS

#include "IntSequence.hpp"

#include "../huffman/Huffman.h"

namespace hdt {

class HuffmanSequence : public IntSequence {

private:
	// FIXME: Make a version where the compressed version is kept in memory and uncompressed on demand.
	std::vector<unsigned int> vectorPlain;
	URICompressed::Huffman *huffman;

public:
	HuffmanSequence();
	virtual ~HuffmanSequence();

	/* Adds elementss to the stream
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
	 * Gets the total number of elements in the stream
	 *
	 * @return int
	 */
	size_t getNumberOfElements();

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

    size_t load(const unsigned char *ptr, const unsigned char *ptrMax, ProgressListener *listener=NULL);

	std::string getType();
};

}

#endif

#endif /* HUFFMANSTREAM_HPP_ */
