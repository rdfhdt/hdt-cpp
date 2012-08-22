/* Huffman.h
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

#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <libcdsBasics.h>
#include "huff.h"
#include <Array.h>

using namespace cds_utils;

namespace URICompressed{

    /** Wrapper for the canonical huffman implementation of Gonzalo Navarro.
     *
     *  @author Francisco Claude
     */
    class Huffman{

        public:
            /** Creates the codes for the sequence seq of length n */
            Huffman(uint * seq, uint64_t n);
            Huffman(uchar * seq, uint64_t n);
            Huffman(Array & seq);

            virtual ~Huffman();

            /** Encodes symb into stream at bit-position pos,
             * returns the ending position (bits) */
            virtual uint64_t encode(uint symb, uint * stream, uint64_t pos);

            /** decodes into symb from stream at bit-position
             * pos, returns the new position.*/
            virtual uint64_t decode(uint * symb, uint * stream, uint64_t pos);

            /** Returns the maximum length of a code */
            virtual uint64_t maxLength();

            /** Returns the size of the table */
            virtual uint64_t getSize();

            /** Saves the coder to a file */
            virtual void save(ostream & fp);

            /** Loads a coder from a file*/
            static Huffman * load(istream & fp);

        protected:
            Huffman();
            THuff huff_table;
    };

};
#endif
