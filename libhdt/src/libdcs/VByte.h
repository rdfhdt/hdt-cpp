/* VByte.h
 * Copyright (C) 2011, Rodrigo Canovas & Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * This class implements the Variable Byte (VByte) Code.
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
 * Contacting the author:
 *   Rodrigo Canovas:  rcanovas@dcc.uchile.cl
 *   Miguel A. Martinez-Prieto:  migumar2@infor.uva.es
 */


#ifndef _VBYTE_H
#define _VBYTE_H

#include <stdint.h>
#include <iostream>
using namespace std;

namespace csd
{

class VByte
{		
  public:
	/**
	 * Encode value into the buffer using VByte. The caller must make sure that
	 * at least 9 bytes are available in the buffer for writing.
	 * Returns the number of read bytes
	 */
	static size_t encode(unsigned char *buffer, uint64_t value );

	/**
	 * Decode value from the buffer using VByte.
	 */
    static size_t decode(const unsigned char *buffer, const unsigned char *maxPtr, uint64_t *value);

	/**
	 * Decode value from the buffer using VByte.
	 */
    static size_t decode(const unsigned char *buffer, const unsigned char *maxPtr, uint32_t *value);

    /** Encodes the integer 'c' in the sequence of bytes (uchar) 'r'.
     * @out Output stream
	 * @value: number to be encoded.
     */
    static void encode(ostream &out, uint64_t value);

    /** Decodes a VByte number from an Input Stream.
    */
    static uint64_t decode(istream &in);


  };
};

#endif  /* _VBYTE_H */
