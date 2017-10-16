/* SuffixArray.h
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

#ifndef SUFFIXARRAY_H_
#define SUFFIXARRAY_H_

/* 
   Adaptation of the original implementation...

   Copyright 1999, N. Jesper Larsson, all rights reserved.

   This file contains an implementation of the algorithm presented in "Faster
   Suffix Sorting" by N. Jesper Larsson (jesper@cs.lth.se) and Kunihiko
   Sadakane (sada@is.s.u-tokyo.ac.jp).

   This software may be used freely for any purpose. However, when distributed,
   the original source must be clearly stated, and, when the source code is
   distributed, the copyright notice must be retained and any alterations in
   the code must be clearly marked. No warranty is given regarding the quality
   of this software.*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

namespace csd{

#define KEY(p)          (V[*(p)+(h)])
#define SWAP(p, q)      (tmp=*(p), *(p)=*(q), *(q)=tmp)
#define MED3(a, b, c)   (KEY(a)<KEY(b) ?                        \
					(KEY(b)<KEY(c) ? (b) : KEY(a)<KEY(c) ? (c) : (a))       \
					: (KEY(b)>KEY(c) ? (b) : KEY(a)>KEY(c) ? (c) : (a)))

#ifndef CHECK
	/* Nonzero CHECK means check that sort is correct. Very slow for repetitive
		 files.*/
#define CHECK 0
#endif

#ifndef PRINT
	/* 0 for no printing. 1 to print suffix numbers in sorted order. 2 to print
		 first MAXPRINT characters of each suffix in sorted order (makes sense only
		 if the input is text and COMPACT is 0).*/
#define PRINT 1
#endif
#ifndef MAXPRINT
#define MAXPRINT 10
#endif

#ifndef COMPACT
	/* 0 to use alphabet 0...UCHAR_MAX without checking what appears. 1 to limit
		 the alphabet to the range l...k-1 that actually appears in the input. 2 to
		 transform the alphabet into 1...k-1 with no gaps and minimum k, preserving
		 the order.*/
#define COMPACT 1
#endif

#define MIN(a, b) ((a)<=(b) ? (a) : (b))

	class SuffixArray{
		public:
			SuffixArray();
            long* sort(unsigned char *text, unsigned long length);
			~SuffixArray();
			
		protected:
			long *I,                  /* group array, ultimately suffix array.*/
				 *V,                          /* inverse array, ultimately inverse of I.*/
				 r,                           /* number of symbols aggregated by transform.*/
				 h;                           /* length of already-sorted prefixes.*/	

			long scmp3(unsigned char *p, unsigned char *q, long *l, long maxl);	

			void update_group(long *pl, long *pm);
			void select_sort_split(long *p, long n);
			long choose_pivot(long *p, long n);
			void sort_split(long *p, long n);
			void bucketsort(long *x, long *p, long n, long k);
			long transform(long *x, long *p, long n, long k, long l, long q);
			void suffixsort(long *x, long *p, long n, long k, long l);
	};
};
#endif /*SUFFIXARRAY_H_*/

