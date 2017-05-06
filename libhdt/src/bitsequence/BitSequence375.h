/* BitSequence375.h

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 */

#ifndef _STATIC_BITSEQUENCE_375_H
#define _STATIC_BITSEQUENCE_375_H

#include <stdint.h>
#include <iostream>
#include <vector>

#include "BitSeq.h"

namespace hdt
{

class BitSequence375 : public BitSeq
{
private:
	const static uint8_t TYPE_BITMAP_PLAIN = 1;
	const static unsigned char WORDSIZE = 8*sizeof(size_t);
	const static unsigned char BLOCKS_PER_SUPER = 256/WORDSIZE;

	/** Length of the bitstring */
	size_t numbits;
	size_t numwords;
	size_t numones;

	vector<size_t> data;
    size_t *array;
	
	bool isMapped;

	vector<size_t> superblocks;	// superblock counters
	vector<unsigned char> blocks;	// block counters

	bool indexReady;

	static size_t binsearch (size_t *data, size_t size, size_t val)
	{
		size_t i,j,m;
		i = 0; j = size;

		while (i+1 < j)
		{ 
			m = (i+j)/2;

			if (data[m] >= val) {
				j = m;
			} else  {
				i = m;
			}
		}

		return i;
	}

	static size_t binsearch0 (size_t *data, size_t size, size_t val)
	{
		size_t i,j,m;
		size_t zeros;
		i = 0; j = size;

		while (i+1 < j)
		{ 
			m = (i+j)/2;
			zeros = m*256-data[m];

			if (zeros >= val) j = m;
			else i = m;
		}

		return i;
	}

	inline size_t numBytes(size_t bits) const {
		return bits==0 ? 1 : ((bits-1)>>3) + 1;
	}

	inline size_t numWords(size_t bits) const {
		return bits==0 ? 1 : ((bits-1)/WORDSIZE) + 1;
	}

	void buildIndex();

public:
	BitSequence375();
	BitSequence375(size_t capacity);
	BitSequence375(size_t *bitarray, size_t numbits);
	~BitSequence375();

	bool access(const size_t i) const;
	size_t rank1(const size_t i) const;
	size_t rank0(const size_t i) const;
	size_t selectPrev1(const size_t start) const;
	size_t selectNext1(const size_t start) const;
    size_t select0(const size_t x) const;
    size_t select1(const size_t x) const;
	size_t getNumBits() const;

	size_t countOnes() const;
	size_t countZeros() const;
	size_t getSizeBytes() const;

	void trimToSize();

	// Additional:
	void set(const size_t i, bool val);
	void append(bool bit);

	/*load-save functions*/
	void save(ostream & f) const;
    size_t load(const unsigned char *ptr, const unsigned char*maxPtr, ProgressListener *listener=NULL);

	static BitSequence375 * load(istream & f);
};

}
#endif
