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

#include <cassert>
#include <cmath>
#include <string.h>

#ifndef WIN32
#include <sys/mman.h> // For fadvise
#endif

#include "BitSequence375.h"

#include "../util/bitutil.h"
#include "../libdcs/VByte.h"

#include "../util/crc8.h"
#include "../util/crc32.h"

namespace hdt
{


BitSequence375::BitSequence375(): numbits(0), numones(0), numwords(0), indexReady(false), isMapped(false)
{
    data.resize(1); //Ensure valid pointer.
    array = &data[0];
}

BitSequence375::BitSequence375(uint64_t capacity): numbits(0), numones(0), indexReady(false), isMapped(false)
{
    numwords = numWords(numbits);
    data.resize(numwords>0?numwords:1);
    array = &data[0];
}

BitSequence375::BitSequence375(uint32_t *bitarray, uint64_t n) : numbits(n), indexReady(false), isMapped(false)
{
    numwords = numWords(numbits);
    data.resize(numwords>0?numwords:1);
    array = &data[0];

    memcpy(&data[0], bitarray, numwords*sizeof(uint32_t));

	buildIndex();
}

BitSequence375::~BitSequence375()
{

}

void BitSequence375::trimToSize()
{
    if(isMapped) {
        return;
    }
    size_t required = numWords(numbits);
    if(numwords!=required) {
        data.resize(required);
        numwords = required;
        array = &data[0];
    }
}

void BitSequence375::buildIndex()
{
	if(indexReady) return;

	trimToSize();

	uint32_t blockPop=0, superBlockPop=0, blockIndex=0, superblockIndex=0;

	// Reserve the buffers for blocks / superblocks
	blocks.resize(numwords);
	superblocks.resize(1+(numwords-1)/BLOCKS_PER_SUPER);

	// Fill them
	while(blockIndex < numwords)
	{
		if(!(blockIndex%BLOCKS_PER_SUPER))
		{
			superBlockPop += blockPop;
			if(superblockIndex<superblocks.size()) {
				superblocks[superblockIndex++] = superBlockPop;
			}
			blockPop = 0;
		}

		blocks[blockIndex] = blockPop;
		blockPop += popcount32(array[blockIndex]);
		blockIndex++;
	}

	numones = superBlockPop+blockPop;

	indexReady=true;
}

size_t BitSequence375::rank0(const size_t i) const
{
	return i+1-rank1(i);
}

size_t BitSequence375::rank1(const size_t pos) const
{
	if(!indexReady) {
		(const_cast<BitSequence375 *>(this))->buildIndex();;
	}

	if (pos >= numbits){
		return numones;
	}

    uint32_t superBlockIndex = pos/(BLOCKS_PER_SUPER*WORDSIZE);
    uint32_t superBlockRank = superblocks[superBlockIndex];

    uint32_t blockIndex = pos/WORDSIZE;
    uint32_t blockRank = blocks[blockIndex];

    uint32_t chunkIndex = WORDSIZE-1-pos%WORDSIZE;
    uint32_t block = array[blockIndex] << chunkIndex;
    uint32_t chunkRank = popcount32(block);

    return superBlockRank + blockRank + chunkRank;
}

void BitSequence375::set(const size_t i, bool val) {
	if(isMapped) {
		throw "This data structure is readonly when mapped.";
	}

	size_t requiredCapacity = 1+(i >> LOGWORDSIZE);

	if(data.size()<requiredCapacity) {
		data.resize(requiredCapacity*2);
		numwords = requiredCapacity*2;
        array = &data[0];
	}
	if(val) {
		bitset(&array[0], i);
	} else {
		bitclean(&array[0], i);
	}

	numbits = numbits > i ? numbits : i;
	indexReady = false;
}

void BitSequence375::append(bool bit) {
	this->set(numbits, bit);
	numbits++;
}

bool BitSequence375::access(const size_t i) const
{
	return array[i/WORDSIZE] & (1u << (i & 0x1F));
}

void BitSequence375::save(ostream & out) const
{
	CRC8 crch;
	CRC32 crcd;
	unsigned char arr[9];

	// Write type
	unsigned char type=1;
	crch.writeData(out, &type, sizeof(type));

	// Write NumBits
	size_t len = csd::VByte::encode(arr, numbits);
	crch.writeData(out, arr, len);

	// Write header CRC
	crch.writeCRC(out);

	// Write data
	len = numBytes(numbits);
	crcd.writeData(out, (unsigned char*)&array[0], len);

	crcd.writeCRC(out);
}

size_t BitSequence375::load(const unsigned char *ptr, const unsigned char *maxPtr, ProgressListener *listener){
	size_t count=0;

    // Check type
    if(ptr[count++]!=1) {
        throw "Trying to read a BitSequence375 but the type does not match";
    }

    // Read numbits
	count += csd::VByte::decode(&ptr[count], &numbits);

    // CRC
    CRC8 crch;
    crch.update(&ptr[0], count);
    if(ptr[count++]!=crch.getValue()) {
        throw "Wrong checksum in BitSequence375 Header.";
    }

    // Read buffer
    this->numwords = numWords(numbits);
    size_t sizeBytes = numBytes(numbits);
    if(&ptr[count+sizeBytes]>=maxPtr) {
        throw "BitSequence375 tries to read beyond the end of the file";
    }

#ifndef WIN32
    madvise((void*)&ptr[count], sizeBytes, MADV_WILLNEED); // Make sure that bitmaps are kept in memory
#endif
    array = (uint32_t *) &ptr[count];
	isMapped = true;
	count += sizeBytes;

    count += 4; // CRC of data

    // Force index rebuild.
    indexReady = false;
	buildIndex();

	return count;
}

BitSequence375 * BitSequence375::load(istream & in)
{
	CRC8 crch;
	CRC32 crcd;
	unsigned char arr[9];

	// Read Type
	unsigned char type;
	in.read((char*)&type, sizeof(type));
	if(type!=1) {    // throw exception
        throw "Trying to read a BitSequence375 but the type does not match";
	}
	crch.update(&type, sizeof(type));

	BitSequence375 * ret = new BitSequence375();

	// Load number of total bits
	ret->numbits = csd::VByte::decode(in);

	size_t len = csd::VByte::encode(arr, ret->numbits);
	crch.update(arr,len);
	crc8_t filecrch = crc8_read(in);
	if(filecrch!=crch.getValue()) {
		throw "Wrong checksum in BitSequence375 Header.";
	}

	// Calculate numWords and create array
	ret->numwords = ret->numWords(ret->numbits);
	ret->data.resize(ret->numwords);
    ret->array = &ret->data[0];

	// Read array from file, byte-aligned.
	size_t bytes = ret->numBytes(ret->numbits);
	in.read((char*)&ret->data[0], bytes);
	if(in.gcount()!=bytes) {
		throw "BitSequence375 error reading array of bits.";
	}

	crcd.update((unsigned char*)&ret->data[0], bytes);
	crc32_t filecrcd = crc32_read(in);
	if(filecrcd!=crcd.getValue()) {
		throw "Wrong checksum in BitSequence375 Data.";
	}

	ret->buildIndex();
	return ret;
}

size_t BitSequence375::getSizeBytes() const
{
	return (this->numwords*sizeof(uint32_t)) + (sizeof(uint32_t)*superblocks.size()) + (sizeof(unsigned char)*blocks.size()) + (sizeof(BitSeq));
}

size_t BitSequence375::selectPrev1(const size_t start) const
{
	throw "BitSequence375 Not implemented";
	return 0;
}

#ifdef __GNUC__
#define first_bit_set(a) __builtin_ffs(a)
#else
#define first_bit_set(a) ffs(a)
#endif

size_t BitSequence375::selectNext1(const size_t fromIndex) const
{
	uint32_t wordIndex = fromIndex/32;
	if (wordIndex >= numwords)
		return -1;

	uint32_t word = array[wordIndex] & (~((size_t)0) << fromIndex);

	while (true) {
		if (word != 0)
			return (wordIndex * WORDSIZE) + first_bit_set(word);
		if (++wordIndex == numwords)
			return -1;
		word = array[wordIndex];
	}
}

size_t BitSequence375::select1(const size_t x) const
{
	if(!indexReady) {
		(const_cast<BitSequence375 *>(this))->buildIndex();;
	}

	if (x > numones) {
		return numbits;
	}

	uint32_t superBlockIndex = binsearch((uint32_t *)&superblocks[0],superblocks.size(),x);

	// If there is a run of many zeros, two correlative superblocks may have the same value,
	// We need to position at the first of them.
	while(superBlockIndex>0 && (superblocks[superBlockIndex]>=x)) {
		superBlockIndex--;
	}

	uint32_t countdown = x-superblocks[superBlockIndex];
	uint32_t blockIdx = superBlockIndex * BLOCKS_PER_SUPER;

	// Search block
	while(true) {
		if(blockIdx>= (superBlockIndex+1) * BLOCKS_PER_SUPER || blockIdx>=blocks.size()) {
			blockIdx--;
			break;
		}

		if(blocks[blockIdx]>=countdown) {
			// We found it!
			blockIdx--;
			break;
		}
		blockIdx++;
	}
	if(blockIdx>=blocks.size()) {
		blockIdx=0;
	}
	countdown -= blocks[blockIdx];

	// Search bit inside block
	uint32_t bitpos = wordSelect1(array[blockIdx], countdown);

	return blockIdx * WORDSIZE + bitpos - 1;
}

size_t BitSequence375::select0(const size_t x1) const
{
	if(!indexReady) {
		(const_cast<BitSequence375 *>(this))->buildIndex();
	}
	uint32_t spos,bpos,pos,word,x;
	const unsigned char *blk;
	size_t j = x1;
	if (j > (numbits-numones)) return numbits;
	spos = binsearch0((uint32_t*)&superblocks[0],(numbits+256-1)/256,j);

	j -= 256*spos-superblocks[spos];
	pos = spos<<8;
	blk = &blocks[0];
	blk += pos>>5;
	bpos = 0;

	while ( ((spos*8+bpos) < ((numbits-1)/WORDSIZE)) && (bpos < (1<<3)-1) && (((32*(bpos+1))-blk[bpos+1]) < j)) bpos++;

	pos += bpos<<5;	
	word = array[pos>>5];
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

size_t BitSequence375::getNumBits() const {
	return numbits;
}

/** Returns how many zeros are in the bitstring */
size_t BitSequence375::countZeros() const {
	return numbits-countOnes();
}

/** Returns the size of the structure in bytes */
size_t BitSequence375::countOnes() const {
	if(!indexReady) {
		(const_cast<BitSequence375 *>(this))->buildIndex();;
	}
	return numones;
}


}
