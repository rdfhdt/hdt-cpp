/* BitSequence375.h

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

#ifndef _STATIC_BITSEQUENCE_375_H
#define _STATIC_BITSEQUENCE_375_H

#include <libcdsBasics.h>
#include <BitString.h>
#include <BitSequence.h>

namespace cds_static
{
    class BitSequence375 : public BitSequence
    {
        private:
		uint *data;
		uint nn;	// # of ints
		uint n;		// # of bits
		uint pop;	// # bits set
		uint *sdata;	// superblock counters
		uint sSize;	// size of sdata vector
		uchar *bdata;	// block counters
		uint bSize; 	// size of bdata vector

            static uint binsearch (uint *data, uint size, uint val)
            { 
		uint i,j,m;
		i = 0; j = size;

		while (i+1 < j)
		{ 
			m = (i+j)/2;

	  		if (data[m] >= val) j = m;
			else i = m;
		}

		return i;
            }

            static uint binsearch0 (uint *data, uint size, uint val)
            { 
		uint i,j,m;
		uint zeros;
		i = 0; j = size;

		while (i+1 < j)
		{ 
			m = (i+j)/2;
			zeros = m*256-data[m];

	  		if (zeros >= val) j = m;
			else i = m;
		}

		return i;
            }

	    void buildIndex();

        public:
            BitSequence375(){};

            BitSequence375(const BitString & bs);
            BitSequence375(uint *bitarray, size_t n);
            ~BitSequence375();
            
            virtual bool access(const size_t i) const;
            virtual size_t rank1(const size_t i) const;
            virtual size_t rank0(const size_t i) const;
            virtual size_t selectPrev1(const size_t start) const;
            virtual size_t selectNext1(const size_t start) const;
            virtual size_t select0(size_t x) const;
            virtual size_t select1(size_t x) const;
            virtual size_t getSize() const;

            /*load-save functions*/
            virtual void save(ofstream & f) const;
            static BitSequence375 * load(ifstream & f);
    };

}
#endif
