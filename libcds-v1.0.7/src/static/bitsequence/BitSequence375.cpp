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
    BitSequence375::BitSequence375( const BitString & bs) 
    {
	size_t n = bs.getLength();
	uint *bitarray = bs.getData();

	this->nn = (n/W)+1;
	this->data = new uint[nn];
        //for(size_t i=0;i<nn;i++) data[i] = bitarray[i];
	memcpy(data, bitarray, nn*sizeof(uint));

	this->n = n; 

	buildIndex();
    }


    BitSequence375::BitSequence375(uint *bitarray, size_t n)
    {    
	this->nn = (n/W)+1;
	this->data = new uint[nn];
        //for(size_t i=0;i<nn;i++) data[i] = bitarray[i];
	memcpy(data, bitarray, nn*sizeof(uint));

	this->n = n; 

	buildIndex();
    }

    BitSequence375::~BitSequence375()
    {
	delete [] data;
	free (bdata);
	free (sdata);
    }

    void BitSequence375::buildIndex() 
    {
	unsigned int nb;
	unsigned int ns;
	unsigned int countB, countS, blockIndex, superblockIndex;
	register unsigned int block;

//	ns = (n/256)+1;
//	nb = (n/32)+1;
	ns = (n+255)/256;
	nb = (n+W-1)/32;

	this->bSize = nb;
	this->sSize = ns;
	this->bdata =(uchar*)malloc(nb*sizeof(uchar));  //	Db = (unsigned char *)malloc(nb*sizeof(unsigned char));
	this->sdata = (uint*)malloc(ns*sizeof(uint));  // 	Ds = (unsigned int *)malloc(ns*sizeof(unsigned int));

	/* Ahora construimos los bloques */
	blockIndex = 0;
	superblockIndex = 0;
	countB = 0;
	countS = 0;

	while(blockIndex < nb)
	{
		if(!(blockIndex%8)) 
		{
			countS += countB;
			sdata[superblockIndex++] = countS;
			countB = 0;
		}
       
		bdata[blockIndex] = countB;
		block = data[blockIndex++];
		countB += popcount(block);
	}

	pop = countS+countB;

    }

    size_t BitSequence375::rank0(const size_t i) const
    {
        return i+1-rank1(i);
    }

    size_t BitSequence375::rank1(const size_t position) const
    {
        if(((int)position+1)==0) return 0;

	register unsigned int block;    
	if (position > n) return pop;	
	block = data[position/32] << (31-position%32);

	return sdata[position/256] + bdata[position/32] + 
           __popcount_tab[block & 0xff] + __popcount_tab[(block>>8) & 0xff] +
           __popcount_tab[(block>>16) & 0xff] + __popcount_tab[block>>24];
    }

    bool BitSequence375::access(const size_t i) const
    {

	return (1u << (i % W)) & data[i/W];
    }

    void BitSequence375::save(ofstream & f) const
    {
        uint wr = BRW32_375;
        saveValue(f,wr);

        saveValue(f,nn);	
        saveValue(f,data, nn);

        //saveValue(f,sSize);
        //saveValue(f,sdata, sSize);
        //saveValue(f,bSize);
        //saveValue<uchar>(f,bdata, bSize);
        //saveValue(f,pop);
        saveValue(f,n);	
    }

    BitSequence375 * BitSequence375::load(ifstream & f) 
    {
        assert(f.good());
        uint type = loadValue<uint>(f);
        if(type!=BRW32_375) {    // throw exception
            abort();
        }
        BitSequence375 * ret = new BitSequence375();
        ret->nn = loadValue<uint>(f);	
        ret->data = loadValue<uint>(f, ret->nn);
        //ret->sSize = loadValue<uint>(f);
        //ret->sdata = loadValue<uint>(f, ret->sSize);
        //ret->bSize = loadValue<uint>(f);
        //ret->bdata = loadValue<uchar>(f, ret->bSize);
        //ret->pop = loadValue<uint>(f);
        ret->n = loadValue<uint>(f);	

	ret->buildIndex();
        return ret;
    }

    size_t BitSequence375::getSize() const
    {
	return (this->nn*sizeof(uint)) + (sizeof(uint)*sSize) + (sizeof(uchar)*bSize) + (sizeof(BitSequence));
    }

    size_t BitSequence375::selectPrev1(const size_t start) const
    {
	throw "Not implemented SELECTPREV1";
	return 0;
    }

    size_t BitSequence375::selectNext1(const size_t fromIndex) const
    {
	if (fromIndex < 0) {
                throw "Out of Index";
	}

	unsigned int wordIndex = fromIndex/32;
	if (wordIndex >= nn)
		return -1;

	unsigned int word = data[wordIndex] & (~((size_t)0) << fromIndex);

	while (true) {
		if (word != 0)
			return (wordIndex * W) + ffs(word);
		if (++wordIndex == nn)
			return -1;
		word = data[wordIndex];
	}
    }

    size_t BitSequence375::select1(const size_t x1) const
    {
	uint spos,bpos,pos,word,x;
	uchar *blk;
	size_t j = x1;
	if (j > pop) return n;
	spos = binsearch(sdata,(n+256-1)/256,j);

	j -= sdata[spos];
	pos = spos<<8;
	blk = bdata + (pos>>5);
	bpos = 0;
    
	while ( ((spos*8+bpos) < ((n-1)/W)) && (bpos < (1<<3)-1) && (blk[bpos+1] < j)) bpos++;
     
	pos += bpos<<5;
	word = data[pos>>5];
	j -= blk[bpos];

	while (1) 
    	{ 
		x = __popcount_tab[word & ((1<<8)-1)]; 

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
	if (j > (n-pop)) return n;
	spos = binsearch0(sdata,(n+256-1)/256,j);

	j -= 256*spos-sdata[spos];
	pos = spos<<8;
	blk = bdata + (pos>>5);
	bpos = 0;

	while ( ((spos*8+bpos) < ((n-1)/W)) && (bpos < (1<<3)-1) && (((32*(bpos+1))-blk[bpos+1]) < j)) bpos++;

	pos += bpos<<5;	
	word = data[pos>>5];
	j -= (32*bpos)-blk[bpos];

	while (1) 
    	{ 
		x = 8-(__popcount_tab[word & ((1<<8)-1)]); 

		if (j <= x) break;
		j -= x; pos += 8;
		word >>= 8;
	}
	
	while (j) { if (!(word & 1)) j--; word >>= 1; pos++; }

	return pos-1;
    }
};
