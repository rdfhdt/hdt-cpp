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


#include "VByte.h"

namespace csd
{

	unsigned int VByte::encode(unsigned int c, unsigned char *r)
	{		
		unsigned int i= 0;

		while (c>127)
		{
			r[i] = (unsigned char)(c&127);
			i++;
			c>>=7;
		}
		
		r[i] = (unsigned char)(c|0x80);
		i++;
		
		return i;
	}
	
	unsigned int VByte::decode(unsigned int *c, unsigned char *r)
	{
		*c = 0;
		int i = 0;
		int shift = 0;
		
		while ( !(r[i] & 0x80) )
		{
			*c |= (r[i] & 127) << shift;
			i++;
			shift+=7;
		}
		
		*c |= (r[i] & 127) << shift;
		i++;
		
		return i;
	}
};


