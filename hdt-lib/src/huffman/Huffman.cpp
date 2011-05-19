/* Huffman.cpp
   Copyright (C) 2008, Francisco Claude, all rights reserved.

   Wrapper for huff written by Gonzalo Navarro

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

#include "Huffman.h"

namespace URICompressed{
	
    Huffman::Huffman(uint * symb, size_t n) {
        uint max_v = 0;
        for(size_t i=0;i<n;i++)
            max_v = max(max_v,symb[i]);
        uint * occ = new uint[max_v+1];
        for(size_t i=0;i<max_v+1;i++)
            occ[i] = 0;
        for(size_t i=0;i<n;i++)
            occ[symb[i]]++;
        huff_table = createHuff(occ, max_v);
        delete [] occ;
    }

    Huffman::Huffman(uchar * symb, size_t n) {
        uchar max_v = 0;
        for(size_t i=0;i<n;i++)
            max_v = max(max_v,symb[i]);
        uint * occ = new uint[max_v+1];
        for(size_t i=0;i<(uint)max_v+1;i++)
            occ[i] = 0;
        for(size_t i=0;i<n;i++)
            occ[symb[i]]++;
				huff_table = createHuff(occ, max_v);
				delete [] occ;
    }

    Huffman::Huffman(Array & seq) {
        uint max_v = seq.getMax();
        uint * occ = new uint[max_v+1];
        for(size_t i=0;i<(uint)max_v+1;i++)
            occ[i] = 0;
        for(size_t i=0;i<seq.getLength();i++)
            occ[seq[i]]++;
        huff_table = createHuff(occ, max_v);
        delete [] occ;
    }

    Huffman::Huffman() {
    }

    Huffman::~Huffman() {
        freeHuff(huff_table);
    }

    size_t Huffman::maxLength(){
        return huff_table.depth;
    }

    size_t Huffman::getSize(){
        return sizeof(Huffman)+sizeHuff(huff_table);
    }

    size_t Huffman::encode(uint symb, uint * stream, size_t pos){
        return encodeHuff(huff_table, symb, stream, pos);
    }

    size_t Huffman::decode(uint * symb, uint * stream, size_t pos){
        return decodeHuff(huff_table, symb, stream, pos);
    }

    void Huffman::save(ofstream & fp){
        saveHuff(huff_table,fp);
    }

    Huffman * Huffman::load(ifstream & fp) {
        Huffman * ret = new Huffman();
        ret->huff_table = loadHuff(fp);
        return ret;
    }

};
