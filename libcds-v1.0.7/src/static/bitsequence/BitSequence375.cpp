/* BitSequence375.cpp
   Copyright (C) 2005, Rodrigo Gonzalez, all rights reserved.

   New RANK, SELECT, SELECT-NEXT and SPARSE RANK implementations.
   Addaptation to libcds by Francisco Claude

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

#include <BitSequence375.h>
#include <cassert>
#include <cmath>
// #include <sys/types.h>
#include <string.h>

namespace cds_static
{

inline uint numBytes(uint64_t bits) {
	return ((bits-1)>>3) + 1;
}

inline uint numWords(uint64_t bits) {
	return ((bits-1)/W) + 1;
}

BitSequence375::BitSequence375( const BitString & bs)
{
	this->numbits = bs.getLength();
	uint *bitarray = bs.getData();

	this->numwords = numWords(numbits);
	this->data = new uint[numwords];

	memcpy(data, bitarray, numwords*sizeof(uint));

	buildIndex();
}


BitSequence375::BitSequence375(uint *bitarray, size_t n)
{
	this->numbits = n;
	this->numwords = numWords(numbits);
	this->data = new uint[numwords];

	memcpy(data, bitarray, numwords*sizeof(uint));

	buildIndex();
}

BitSequence375::~BitSequence375()
{
	delete [] data;
	free (blocks);
	free (superblocks);
}

void BitSequence375::buildIndex()
{
	unsigned int block, countBlock=0, countSuperBlock=0, blockIndex=0, superblockIndex=0;

	// Calculate number of blocks / superblocks
	this->blocksSize = (numbits+W-1)/32;
	this->superblocksSize = (numbits+255)/256;

	// Create the buffers for blocks / superblocks
	this->blocks =(uchar*)malloc(blocksSize*sizeof(uchar));
	this->superblocks = (uint*)malloc(superblocksSize*sizeof(uint));

	// Fill them
	while(blockIndex < blocksSize)
	{
		if(!(blockIndex%8)) 
		{
			countSuperBlock += countBlock;
			superblocks[superblockIndex++] = countSuperBlock;
			countBlock = 0;
		}

		blocks[blockIndex] = countBlock;
		block = data[blockIndex++];
		countBlock += popcount(block);
	}

	pop = countSuperBlock+countBlock;

}

size_t BitSequence375::rank0(const size_t i) const
{
	return i+1-rank1(i);
}

size_t BitSequence375::rank1(const size_t position) const
{
	if(((int)position+1)==0) return 0;

	register unsigned int block;
	if (position > numbits) return pop;
	block = data[position>>LOGW] << (31-position & 0x1F);

	return superblocks[position/256] + blocks[position/32] +
			popcount(block);
}

bool BitSequence375::access(const size_t i) const
{

#if defined(__i386__) || defined(__x86_64__)
	bool ret=0;
		__asm__("bt  %2, (%1); setc %0"
				:"=r"(ret)
				:"r"(data), "r"(i)
				);
		return ret;
#else
	return data[i>>LOGW] & (1u << (i & 0x1F));
#endif
}



void BitSequence375::save(ofstream & f) const
{
	// Write type
	uchar type = BRW32_375;
	saveValue<uchar>(f,type);

	// Write number of total bits
	saveValue<uint64_t>(f,numbits);

	// Write bits, byte-aligned.
	f.write((char*)data, numBytes(numbits));
}

BitSequence375 * BitSequence375::load(ifstream & f)
{
	assert(f.good());
	uchar type = loadValue<uchar>(f);
	if(type!=BRW32_375) {    // throw exception
		abort();
	}
	BitSequence375 * ret = new BitSequence375();

	// Load number of total bits
	ret->numbits = loadValue<uint64_t>(f);

	// Calculate numWords and create array
	ret->numwords = numWords(ret->numbits);
	ret->data = new uint[ret->numwords];

	// Read array from file, byte-aligned.
	size_t bytes = numBytes(ret->numbits);
	f.read((char*)ret->data, bytes);

	if(f.gcount()!=bytes) {
		throw "BitSequence375 error reading array of bits.";
	}

	ret->buildIndex();
	return ret;
}

size_t BitSequence375::getSize() const
{
	return (this->numwords*sizeof(uint)) + (sizeof(uint)*superblocksSize) + (sizeof(uchar)*blocksSize) + (sizeof(BitSequence));
}

size_t BitSequence375::selectPrev1(const size_t start) const
{
	throw "Not implemented SELECTPREV1";
	return 0;
}

#ifdef __SSE4_2__
#warning "Using SSE4.2"
#define first_bit_set(a) __builtin_ffs(a)
#else
#warning "Normal"
#define first_bit_set(a) ffs(a)
#endif

size_t BitSequence375::selectNext1(const size_t fromIndex) const
{
	if (fromIndex < 0) {
		throw "Out of Index";
	}

	unsigned int wordIndex = fromIndex/32;
	if (wordIndex >= numwords)
		return -1;

	unsigned int word = data[wordIndex] & (~((size_t)0) << fromIndex);

	while (true) {
		if (word != 0)
			return (wordIndex * W) + first_bit_set(word);
		if (++wordIndex == numwords)
			return -1;
		word = data[wordIndex];
	}
}

size_t BitSequence375::select1(const size_t x1) const
{
	uint spos,bpos,pos,word,x;
	uchar *blk;
	size_t j = x1;
	if (j > pop) return numbits;
	spos = binsearch(superblocks,(numbits+256-1)/256,j);

	j -= superblocks[spos];
	pos = spos<<8;
	blk = blocks + (pos>>5);
	bpos = 0;

	while ( ((spos*8+bpos) < ((numbits-1)/W)) && (bpos < (1<<3)-1) && (blk[bpos+1] < j)) bpos++;

	pos += bpos<<5;
	word = data[pos>>5];
	j -= blk[bpos];

	while (1) 
	{
		x = popcount8(word);

		if (j <= x) break;
		j -= x; pos += 8;
		word >>= 8;
	}

	while (j) { if (word & 1) j--; word >>= 1; pos++; }

	return pos-1;
}

size_t BitSequence375::select0(const size_t x1) const
{
	uint spos,bpos,pos,word,x;
	uchar *blk;
	size_t j = x1;
	if (j > (numbits-pop)) return numbits;
	spos = binsearch0(superblocks,(numbits+256-1)/256,j);

	j -= 256*spos-superblocks[spos];
	pos = spos<<8;
	blk = blocks + (pos>>5);
	bpos = 0;

	while ( ((spos*8+bpos) < ((numbits-1)/W)) && (bpos < (1<<3)-1) && (((32*(bpos+1))-blk[bpos+1]) < j)) bpos++;

	pos += bpos<<5;	
	word = data[pos>>5];
	j -= (32*bpos)-blk[bpos];

	while (1) 
	{
		x = 8-popcount8(word);

		if (j <= x) break;
		j -= x; pos += 8;
		word >>= 8;
	}

	while (j) { if (!(word & 1)) j--; word >>= 1; pos++; }

	return pos-1;
}
};
