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
#include <stdexcept>
#include <cmath>
#include <string.h>

#include "BitSequence375.h"

#include "../util/bitutil.h"
#include "../libdcs/VByte.h"

#include "../util/crc8.h"
#include "../util/crc32.h"

namespace hdt
{


BitSequence375::BitSequence375(): numbits(0), numwords(0), numones(0), isMapped(false), indexReady(false)
{
    data.resize(1); //Ensure valid pointer.
    array = &data[0];
}

BitSequence375::BitSequence375(size_t capacity): numbits(0), numones(0), isMapped(false), indexReady(false)
{
    numwords = numWords(numbits);
    data.resize(numwords);
    array = &data[0];
}

BitSequence375::BitSequence375(size_t *bitarray, size_t n) : numbits(n), isMapped(false), indexReady(false)
{
    numwords = numWords(numbits);
    data.resize(numwords);
    array = &data[0];

    memcpy(&data[0], bitarray, numwords*sizeof(size_t));

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

	size_t blockPop=0, superBlockPop=0;
	size_t blockIndex=0, superblockIndex=0;

	// Reserve the buffers for blocks / superblocks
	blocks.resize(numwords);
	superblocks.resize(1+(numwords-1)/BLOCKS_PER_SUPER);

	//cout << "Blocks: " << numwords << " Super: "<< superblocks.size() << " Size: " << superblocks.size()*8 << endl;

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
		blockPop += popcount64(array[blockIndex]);
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

    size_t superBlockIndex = pos/(BLOCKS_PER_SUPER*WORDSIZE);
    size_t superBlockRank = superblocks[superBlockIndex];

    size_t blockIndex = pos/WORDSIZE;
    size_t blockRank = blocks[blockIndex];

    size_t chunkIndex = WORDSIZE-1-pos%WORDSIZE;
    size_t block = array[blockIndex] << chunkIndex;
    size_t chunkRank = popcount64(block);

    return superBlockRank + blockRank + chunkRank;
}

void BitSequence375::set(const size_t i, bool val) {
	if(isMapped) {
		throw std::runtime_error("This data structure is readonly when mapped.");
	}

	size_t requiredCapacity = 1+(i/WORDSIZE);

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

	numbits = i>=numbits ? i+1 : numbits;
	indexReady = false;
}

void BitSequence375::append(bool bit) {
	// numbits automatically increased when doing the set out of range.
	this->set(numbits, bit);
}

bool BitSequence375::access(const size_t i) const
{
	return bitget(array, i);
}

void BitSequence375::save(ostream & out) const
{
	CRC8 crch;
	CRC32 crcd;
	unsigned char arr[9];

	// Write type
	unsigned char type=TYPE_BITMAP_PLAIN;
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


#define CHECKPTR(base, max, size) if(((base)+(size))>(max)) throw std::runtime_error("Could not read completely the HDT from the file.");

size_t BitSequence375::load(const unsigned char *ptr, const unsigned char *maxPtr, ProgressListener *listener){
	size_t count=0;

    // Check type
	CHECKPTR(&ptr[count], maxPtr, 1);
    if(ptr[count++]!=TYPE_BITMAP_PLAIN) {
        throw std::runtime_error("Trying to read a BitSequence375 but the type does not match");
    }

    // Read numbits
    uint64_t totalBits;
	count += csd::VByte::decode(&ptr[count], maxPtr, &totalBits);
	if(sizeof(size_t)==4 && totalBits>0xFFFFFFFF) {
		throw std::runtime_error("This File is too big to be processed using 32Bit version. Please compile with 64bit support");
	}
	this->numbits = (size_t) totalBits;

    // CRC
    CRC8 crch;
    crch.update(&ptr[0], count);
    CHECKPTR(&ptr[count], maxPtr, 1);
    if(ptr[count++]!=crch.getValue()) {
        throw std::runtime_error("Wrong checksum in BitSequence375 Header.");
    }

    // Read buffer
    this->numwords = numWords(numbits);
    size_t sizeBytes = numBytes(numbits);
    if(&ptr[count+sizeBytes]>=maxPtr) {
        throw std::runtime_error("BitSequence375 tries to read beyond the end of the file");
    }

    array = (size_t *) &ptr[count];
	isMapped = true;
	count += sizeBytes;

	CHECKPTR(&ptr[count], maxPtr, 4);
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
	if(type!=TYPE_BITMAP_PLAIN) {    // throw exception
        throw std::runtime_error("Trying to read a BitmapPlain but the type does not match");
	}
	crch.update(&type, sizeof(type));

	BitSequence375 * ret = new BitSequence375();

	// Load number of total bits
	uint64_t totalBits = csd::VByte::decode(in);
	if(sizeof(size_t)==4 && totalBits>0xFFFFFFFF) {
		throw std::runtime_error("This File is too big to be processed using 32Bit version. Please compile with 64bit support");
	}
	ret->numbits = (size_t) totalBits;


	size_t len = csd::VByte::encode(arr, ret->numbits);
	crch.update(arr,len);
	crc8_t filecrch = crc8_read(in);
	if(filecrch!=crch.getValue()) {
		throw std::runtime_error("Wrong checksum in BitSequence375 Header.");
	}

	// Calculate numWords and create array
	ret->numwords = ret->numWords(ret->numbits);
	ret->data.resize(ret->numwords);
    ret->array = &ret->data[0];

	// Read array from file, byte-aligned.
	size_t bytes = ret->numBytes(ret->numbits);
	in.read((char*)&ret->data[0], bytes);
	streamsize gc = in.gcount();
	if(gc < 0 || static_cast<size_t>(gc)!=bytes) {
		throw std::runtime_error("BitSequence375 error reading array of bits.");
	}

	crcd.update((unsigned char*)&ret->data[0], bytes);
	crc32_t filecrcd = crc32_read(in);
	if(filecrcd!=crcd.getValue()) {
		throw std::runtime_error("Wrong checksum in BitSequence375 Data.");
	}

	ret->buildIndex();
	return ret;
}

size_t BitSequence375::getSizeBytes() const
{
	//return (this->numwords*sizeof(size_t)) + (sizeof(size_t)*superblocks.size()) + (sizeof(unsigned char)*blocks.size()) + (sizeof(BitSeq));
	return this->numwords*sizeof(size_t);
}

size_t BitSequence375::selectPrev1(const size_t start) const
{
	throw std::runtime_error("BitSequence375 selectPrev1 Not implemented");
	return 0;
}

#ifdef __GNUC__
#define first_bit_set(a) __builtin_ffs(a)
#elif defined(WIN32)
#define first_bit_set(a) __lzcnt(a)
#else
#define first_bit_set(a) ffs(a)
#endif

size_t BitSequence375::selectNext1(const size_t fromIndex) const
{
	size_t wordIndex = fromIndex/64;
	if (wordIndex >= numwords)
		return -1;

	size_t word = array[wordIndex] & (~((size_t)0) << fromIndex);

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

	size_t superBlockIndex = binsearch((size_t *)&superblocks[0],superblocks.size(),x);

	// If there is a run of many zeros, two correlative superblocks may have the same value,
	// We need to position at the first of them.
	while(superBlockIndex>0 && (superblocks[superBlockIndex]>=x)) {
		superBlockIndex--;
	}

	size_t countdown = x-superblocks[superBlockIndex];
	size_t blockIdx = superBlockIndex * BLOCKS_PER_SUPER;

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
	size_t bitpos = wordSelect1(array[blockIdx], countdown);

	return blockIdx * WORDSIZE + bitpos - 1;
}

size_t BitSequence375::select0(const size_t x1) const
{
	throw std::runtime_error("Not implemented");
	// FIXME Try on 64Bit.
//	if(!indexReady) {
//		(const_cast<BitSequence375 *>(this))->buildIndex();
//	}
//	size_t spos,bpos,pos,word,x;
//	const unsigned char *blk;
//	size_t j = x1;
//	if (j > (numbits-numones)) return numbits;
//	spos = binsearch0((size_t*)&superblocks[0],(numbits+256-1)/256,j);
//
//	j -= 256*spos-superblocks[spos];
//	pos = spos<<8;
//	blk = &blocks[0];
//	blk += pos>>6;
//	bpos = 0;
//
//	while ( ((spos*8+bpos) < ((numbits-1)/WORDSIZE)) && (bpos < (1<<3)-1) && (((64*(bpos+1))-blk[bpos+1]) < j)) bpos++;
//
//	pos += bpos<<6;
//	word = array[pos>>6];
//	j -= (64*bpos)-blk[bpos];
//
//	while (1)
//	{
//		x = 8-popcount8(word);
//
//		if (j <= x) break;
//		j -= x; pos += 8;
//		word >>= 8;
//	}
//
//	while (j) { if (!(word & 1)) j--; word >>= 1; pos++; }
//
//	return pos-1;
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
