/* BitSequence.h
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * BitSequence definition
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
 */

#ifndef _BITSEQ_H
#define _BITSEQ_H

#include <iostream>

#include <HDTListener.hpp>

using namespace std;

namespace hdt
{

class BitSeq
{

public:
	virtual ~BitSeq() {};

	/** Returns the number of zeros until position i */
	virtual size_t rank0(const size_t i) const=0;

	/** Returns the position of the i-th zero
	 * @return (size_t)-1 if i=0, len if i>num_zeros or the position */
	virtual size_t select0(const size_t i) const=0;

	/** Returns the number of ones until position i */
	virtual size_t rank1(const size_t i) const=0;

	/** Returns the position of the i-th one
	 * @return (size_t)-1 if i=0, len if i>num_ones or the position */
	virtual size_t select1(const size_t i) const=0;

	/** Return the first position starting at i that contains a 1.
	 * In case there are no more ones in the bitsequence, the function
	 * returns the length of the bitmap
	 */
	virtual size_t selectNext1(const size_t i) const=0;

	/** Return the first position starting at i moving to the left that contains a 1.
	 * In case there are no more ones to the left in the bitsequence, the function
	 * returns (size_t)-1.
	 */
	virtual size_t selectPrev1(const size_t i) const=0;

	/** Returns the i-th bit */
	virtual bool access(const size_t i) const=0;

	/** Returns the length in bits of the bitmap */
	virtual size_t getNumBits() const=0;

	/** Returns how many ones are in the bitstring */
	virtual size_t countOnes() const=0;

	/** Returns how many zeros are in the bitstring */
	virtual size_t countZeros() const=0;

	/** Returns the size of the structure in bytes */
	virtual size_t getSizeBytes() const=0;

	/** Stores the bitmap given a file pointer*/
	virtual void save(ostream & fp) const=0;

	/** Load the data structure from a memory region */
    virtual size_t load(const unsigned char *ptr, const unsigned char*maxPtr, ProgressListener *listener=NULL)=0;

	/** Reads a bitmap determining the type */
	static BitSeq * load(istream & fp);

};

};

#endif							 /* _STATIC_BITSEQUENCE_H */
