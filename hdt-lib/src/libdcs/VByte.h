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

#include <iostream>
using namespace std;

#include <libcdsBasics.h>
using namespace cds_utils;

namespace csd
{
static const uint64_t SSIZE = 4;  // Size reserved for encoded sequences.

class VByte
{		
  public:
    /** Encodes the integer 'c' in the sequence of bytes (uchar) 'r'.
	@c: number to be encoded.
	@r: encoded sequence.
    */
    static uint encode(uint c, uchar *r);

    /** Decodes the sequence of bytes (uchar) 'r' into the integer 'c'.
	@c: decoded number.
	@r: sequence to be decoded.
    */
    static uint decode(uint *c, uchar *r);
  private:
    /** Set the p-th bit of the byte e.
        @e: byte to be shifted.
	@p: position to be set.
    */
    static void bitset(uchar * e, uint64_t p);
  };
};

#endif  /* _VBYTE_H */
