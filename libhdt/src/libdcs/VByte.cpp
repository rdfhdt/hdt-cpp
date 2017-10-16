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

#include <stdexcept>
#include "VByte.h"

namespace csd
{

	/**
	 * Encode value into the buffer using VByte. The caller must make sure that
	 * at least 10 bytes are available in the buffer for writing.
	 * Returns the number of read bytes
	 */
	size_t VByte::encode(unsigned char *buffer, uint64_t value )
	{
		size_t i= 0;

		while (value>127)
		{
			buffer[i] = (unsigned char)(value&127);
			i++;
			value>>=7;
		}

		buffer[i] = (unsigned char)(value|0x80);
		i++;

		return i;
	}

	/**
	 * Decode value from the buffer using VByte.
	 */
    size_t VByte::decode(const unsigned char *buffer, const unsigned char *maxPtr, uint64_t *value)
	{
		*value = 0;
		int i = 0;
		int shift = 0;

		while ( (&buffer[i]<=maxPtr) && !(buffer[i] & 0x80) )
		{
		   	if(shift>50) {
		   		cerr << "VByte.decode(uint64_t) Read too many bytes and still did not find a terminating byte" << endl;
		   	}

			*value |= (size_t)(buffer[i] & 127) << shift;
			i++;
			shift+=7;
		}

		*value |= (size_t)(buffer[i] & 127) << shift;
		i++;

		return i;
	}

	/**
	 * Decode value from the buffer using VByte.
	 */
    size_t VByte::decode(const unsigned char *buffer, const unsigned char *maxPtr, uint32_t *value)
	{
		*value = 0;
		int i = 0;
		int shift = 0;

		while ((&buffer[i]<=maxPtr) && !(buffer[i] & 0x80) )
		{
			if(shift>50) {
		   		cerr << "VByte.decode(uint32_t) Read too many bytes and still did not find a terminating byte" << endl;
		   	}

			*value |= (buffer[i] & 127) << shift;
			i++;
			shift+=7;
		}

		*value |= (buffer[i] & 127) << shift;
		i++;

		return i;
	}

	void VByte::encode(ostream &out, uint64_t value)
	{
		while( value > 127) {
			out.put((char)(value & 127));
			value>>=7;
		}
		out.put((char)(value|0x80));
	}

	uint64_t VByte::decode(istream &in)
	{
	    uint64_t out = 0;
	    int shift=0;
	    uint64_t readbyte = in.get(); if(!in.good()) throw std::runtime_error("Error reading input");

	    while( (readbyte & 0x80)==0) {
	    	if(shift>50) {
	    		throw std::runtime_error("VByte.istream() Read too many bytes and still did not find a terminating byte");
	    	}
		    out |= (readbyte & 127) << shift;
		    readbyte = in.get(); if(!in.good()) throw std::runtime_error("Error reading input");
		    shift+=7;
	    }
	    out |= (readbyte & 127) << shift;
	    return out;
	}
};
