/* SSA.h
 * Copyright (C) 2011, Rodrigo Canovas & Miguel A. Martinez-Prieto
 * all rights reserved.
 * 
 * Abstract class for implementing Compressed String Dictionaries following:
 * 
 *   ==========================================================================
 *    "Compressed String Dictionaries"
 *     Nieves R. Brisaboa, Rodrigo Canovas, Francisco Claude, 
 *     Miguel A. Martinez-Prieto and Gonzalo Navarro.
 *     10th Symposium on Experimental Algorithms (SEA'2011), p.136-147, 2011.
 *   ==========================================================================
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
 * Contacting the authors:
 * Rodrigo Canovas:  rcanovas@dcc.uchile.cl
 * Miguel A. Martinez-Prieto:  migumar2@infor.uva.es
 */

#ifndef SSA_WORDS_H
#define SSA_WORDS_H

#include <SequenceBuilder.h>
#include <Sequence.h>
#include <BitSequenceBuilder.h>
#include <BitSequence.h>

#include <Mapper.h>
#include <algorithm>

#include "SuffixArray.h"

using namespace std;
using namespace cds_static;

namespace csd{
	class SSA{
		public:
			SSA(uchar * seq, uint n, bool free_text=false, bool use_sampling=false);
			SSA();
			~SSA();

			bool set_static_sequence_builder(SequenceBuilder * ssb);
			bool set_static_bitsequence_builder(BitSequenceBuilder * sbb);
			bool set_samplesuff(uint sample);

			bool build_index();

			uint size();
			void print_stats();
			uint length();

			uint LF(uint i);
			uint locate_id(uchar * pattern, uint m);
			uint locate(uchar * pattern, uint m, uint32_t **occs);
            uint locate(uchar * pattern, uint m, uint offset, uint limit, uint32_t **occs, uint* num_occ);

			uchar * extract_id(uint id, uint max_len);
            static SSA * load(istream &fp);
            void save(ostream & fp);

		protected:
			uint n;
			Sequence * bwt;

			BitSequence * sampled;
			uint samplesuff;
			uint * suff_sample;  
			
			uint * occ;
			uint maxV;
			bool built;
			bool free_text;
			bool use_sampling;
			bool *alphabet;	

			/*use only for construction*/
			uchar * _seq;
			uint * _bwt;   
			unsigned long * _sa;   
			SequenceBuilder * _ssb;
			BitSequenceBuilder * _sbb;
			/*******************************/

			void build_bwt();
			void build_sa();
			int cmp(uint i, uint j);
	};

};
#endif
