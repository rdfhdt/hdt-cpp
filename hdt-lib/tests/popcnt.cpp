#include <stdint.h>
#include <iostream>

// Compile with -msse4.2
//#include <smmintrin.h>
#include "../src/util/StopWatch.hpp"

using namespace std;

static const unsigned char __poptable[256] =
{
#   define B2(n) n,     n+1,     n+1,     n+2
#   define B4(n) B2(n), B2(n+1), B2(n+1), B2(n+2)
#   define B6(n) B4(n), B4(n+1), B4(n+1), B4(n+2)
    B6(0), B6(1), B6(1), B6(2)
};

/** Counts the number of 1s in x */
inline unsigned int popcountTable1(const unsigned int x) {
    return __poptable[(x >>  0) & 0xff]  + __poptable[(x >>  8) & 0xff]
        + __poptable[(x >> 16) & 0xff] + __poptable[(x >> 24) & 0xff];
}

// Slower than 1!!
inline unsigned int popcountTable2(const unsigned int x) {
	unsigned char * p = (unsigned char *) &x;
	return __poptable[p[0]] + __poptable[p[1]] + __poptable[p[2]] +	__poptable[p[3]];
}

inline unsigned int popCountParallel(unsigned int v) {
	unsigned int c; 	// store the total here

	v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
	c = (((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24; // count

	return c;
}

inline unsigned int popCountParallel64(unsigned int v) {
	unsigned int c;
	c =  ((v & 0xfff) * 0x1001001001001ULL & 0x84210842108421ULL) % 0x1f;
	c += (((v & 0xfff000) >> 12) * 0x1001001001001ULL & 0x84210842108421ULL) %
			0x1f;
	c += ((v >> 24) * 0x1001001001001ULL & 0x84210842108421ULL) % 0x1f;

	return c;
}

inline unsigned int popCountLoopSet(unsigned int v) {
	unsigned int c = 0;
	// strip one set bit per iteration
	while (v != 0)
	{
		v &= v-1;
		c++;
	}
	return c;
}

/****** The following code comes with the 3-clause BSD license  ******/
/* It is CPU specific because it uses SSE2 instructions. */
/* It should work under Visual Studio; what's the right SSE2 macro check
       for that compiler?  */

/* Imran notes:
    Note that you do need to compile the SSE popcounts with
    -fno-strict-aliasing, or they'll return incorrect results (the
    epilogues play a little fast and loose with pointer
    casting). Compiling for 64-bit will probably help the performance
    too, since the inner loops are unrolled and were optimized for the
    extra 8 registers. */
/* I found that -fno-strict-aliasing does not affect this benchmark */


/* Source: Supplemental Information to "Anatomy of High-Performance 2D
   Similarity Calculations", Haque IS, Pande VS, and Walters WP,
   Journal of Chemical Information and Modeling 2011 */

/*  Written by Imran S. Haque (ihaque@cs.stanford.edu)

 Copyright (c) 2011 Stanford University.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
     * Neither the name of Stanford University nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#if defined(__GNUC__) && defined(__SSE2__)
#include <emmintrin.h>

static inline __m128i _mm_srli1_epi8(__m128i v) {
    // Avg returns (a_i + b_i + 1) >> 1; so make b_i = -1 to compute (a >> 1)
    const __m128i neg1 = _mm_set1_epi8(0xFF);
    return _mm_avg_epu8(v,neg1);
}
static inline __m128i _mm_srli2_epi8(__m128i v) {
    // Faster than mask-and-shift approach used in srli4
    return _mm_srli1_epi8(_mm_srli1_epi8(v));
}
static inline __m128i _mm_srli4_epi8(__m128i v) {
    // Using ANDNOT lets us share the magic number with what's used for the popcount itself
    const __m128i mask = _mm_set1_epi32(0x0F0F0F0F);
    return _mm_srli_epi16(_mm_andnot_si128(mask,v),4);
}

static inline __m128i popcount_sse2_8_helper_1(unsigned* buf, int N) {
    __m128i* vbuf = (__m128i*)buf;
    __m128i total,count0,count1,count2,count3;
    int i;
    __m128i B0 = _mm_set1_epi32(0x55555555);
    __m128i B1 = _mm_set1_epi32(0x33333333);
    __m128i B2 = _mm_set1_epi32(0x0F0F0F0F);
    total = _mm_setzero_si128();
    for (i = 0; i < N; i+=4) {
      __m128i v0 = _mm_load_si128(vbuf+i+0);
      __m128i v1 = _mm_load_si128(vbuf+i+1);
      __m128i v2 = _mm_load_si128(vbuf+i+2);
      __m128i v3 = _mm_load_si128(vbuf+i+3);

      count0 = _mm_sub_epi8(v0, _mm_and_si128(B0, _mm_srli1_epi8(v0)));
      count1 = _mm_sub_epi8(v1, _mm_and_si128(B0, _mm_srli1_epi8(v1)));
      count2 = _mm_sub_epi8(v2, _mm_and_si128(B0, _mm_srli1_epi8(v2)));
      count3 = _mm_sub_epi8(v3, _mm_and_si128(B0, _mm_srli1_epi8(v3)));

      count0 = _mm_add_epi8(_mm_and_si128(B1,count0),
			    _mm_and_si128(B1,_mm_srli2_epi8(count0)));
      count1 = _mm_add_epi8(_mm_and_si128(B1,count1),
			    _mm_and_si128(B1,_mm_srli2_epi8(count1)));
      count2 = _mm_add_epi8(_mm_and_si128(B1,count2),
			    _mm_and_si128(B1,_mm_srli2_epi8(count2)));
      count3 = _mm_add_epi8(_mm_and_si128(B1,count3),
			    _mm_and_si128(B1,_mm_srli2_epi8(count3)));

      count0 = _mm_and_si128(B2,_mm_add_epi8(count0,
					     _mm_srli4_epi8(count0)));
      count1 = _mm_and_si128(B2,_mm_add_epi8(count1,
					     _mm_srli4_epi8(count1)));
      count2 = _mm_and_si128(B2,_mm_add_epi8(count2,
					     _mm_srli4_epi8(count2)));
      count3 = _mm_and_si128(B2,_mm_add_epi8(count3,
					     _mm_srli4_epi8(count3)));

      total = _mm_add_epi8(total,_mm_add_epi8(_mm_add_epi8(count0,count1),
					      _mm_add_epi8(count2,count3)));
    }

    // Reduce 16*8b->{-,-,-,16b,-,-,-,16b}
    const __m128i ZERO = _mm_setzero_si128();
    return _mm_sad_epu8(total,ZERO);
}


static inline int popcount_sse2_8bit(unsigned* buf,int n) {
  int N = n/4;
  __m128i count32 = _mm_setzero_si128();
  // 2^5 loop iters might overflow 8-bit counter, so
  // cap it at 2^4 iters per chunk
  const int inner_maxits = 16;
  while (N > inner_maxits) {
    count32 = _mm_add_epi32(count32,popcount_sse2_8_helper_1(buf,inner_maxits));
    buf += inner_maxits*4;
    N -= inner_maxits;
  }
  if (N > 0) count32 = _mm_add_epi32(count32,popcount_sse2_8_helper_1(buf,N));

  // Layout coming from PSADBW accumulation is 2*{0,32}: 0 S1 0 S0
  int count;
  _mm_store_ss((float*)&count,(__m128)(_mm_add_epi32(count32,_mm_shuffle_epi32(count32,_MM_SHUFFLE(2,2,2,2)))));
  return count;
}


static inline __m128i reduce_16_to_32(__m128i v) {
  // Reduce 8*16->4*32
  const __m128i ZERO = _mm_setzero_si128();
  __m128i temp1 = _mm_unpacklo_epi16(v,ZERO);
  __m128i temp2 = _mm_unpackhi_epi16(v,ZERO);
  return _mm_add_epi32(temp1,temp2);
}

static inline __m128i popcount_sse2_16_helper_1(unsigned* buf, int N) {
  __m128i* vbuf = (__m128i*)buf;
  __m128i total,count0,count1,count2,count3;
  int i;
  __m128i B0 = _mm_set1_epi32(0x55555555);
  __m128i B1 = _mm_set1_epi32(0x33333333);
  __m128i B2 = _mm_set1_epi32(0x0F0F0F0F);
  __m128i B3 = _mm_set1_epi32(0x00FF00FF);
  total = _mm_setzero_si128();
  for (i = 0; i < N; i+=4) {
    __m128i v0 = _mm_load_si128(vbuf+i+0);
    __m128i v1 = _mm_load_si128(vbuf+i+1);
    __m128i v2 = _mm_load_si128(vbuf+i+2);
    __m128i v3 = _mm_load_si128(vbuf+i+3);

    count0 = _mm_sub_epi16(v0, _mm_and_si128(B0, _mm_srli_epi16(v0,1)));
    count1 = _mm_sub_epi16(v1, _mm_and_si128(B0, _mm_srli_epi16(v1,1)));
    count2 = _mm_sub_epi16(v2, _mm_and_si128(B0, _mm_srli_epi16(v2,1)));
    count3 = _mm_sub_epi16(v3, _mm_and_si128(B0, _mm_srli_epi16(v3,1)));

    count0 = _mm_add_epi16(_mm_and_si128(B1,count0),
			   _mm_and_si128(B1,_mm_srli_epi16(count0,2)));
    count1 = _mm_add_epi16(_mm_and_si128(B1,count1),
			   _mm_and_si128(B1,_mm_srli_epi16(count1,2)));
    count2 = _mm_add_epi16(_mm_and_si128(B1,count2),
			   _mm_and_si128(B1,_mm_srli_epi16(count2,2)));
    count3 = _mm_add_epi16(_mm_and_si128(B1,count3),
			   _mm_and_si128(B1,_mm_srli_epi16(count3,2)));

    count0 = _mm_and_si128(B2,_mm_add_epi16(count0,
					    _mm_srli_epi16(count0,4)));
    count1 = _mm_and_si128(B2,_mm_add_epi16(count1,
					    _mm_srli_epi16(count1,4)));
    count2 = _mm_and_si128(B2,_mm_add_epi16(count2,
					    _mm_srli_epi16(count2,4)));
    count3 = _mm_and_si128(B2,_mm_add_epi16(count3,
					    _mm_srli_epi16(count3,4)));

    count0 = _mm_and_si128(B3,_mm_add_epi16(count0,
					    _mm_srli_epi16(count0,8)));
    count1 = _mm_and_si128(B3,_mm_add_epi16(count1,
					    _mm_srli_epi16(count1,8)));
    count2 = _mm_and_si128(B3,_mm_add_epi16(count2,
					    _mm_srli_epi16(count2,8)));
    count3 = _mm_and_si128(B3,_mm_add_epi16(count3,
					    _mm_srli_epi16(count3,8)));

    total = _mm_add_epi16(total,_mm_add_epi16(_mm_add_epi16(count0,count1),
					      _mm_add_epi16(count2,count3)));

  }

  return reduce_16_to_32(total);
}


static inline int popcount_sse2_16bit(unsigned* buf,int n) {
    int N = n/4, base = 0;
    uint32_t tmp[4];
    __m128i count32 = _mm_setzero_si128();
    // 2^12 loop iters might overflow 16-bit counter, so
    // cap it at 2^11 iters per chunk
    const int maxiters = 2048;
    while (N > maxiters) {
        count32 = _mm_add_epi32(count32,popcount_sse2_16_helper_1(buf+base,maxiters));
        base += maxiters*4;
        N -= maxiters;
    }
    if (N > 0) count32 = _mm_add_epi32(count32,popcount_sse2_16_helper_1(buf+base, N));
    _mm_store_si128((__m128i*)tmp,count32);
    return tmp[0]+tmp[1]+tmp[2]+tmp[3];
}

int popcount_sse2_32bit(unsigned* buf, int n) {
  __m128i* vbuf = (__m128i*)buf;
  int N = n/4;
  __m128i total, count0, count1, count2, count3;
  int i;
  unsigned tmp[4];
  unsigned magic[] = {0x55555555, 0x55555555, 0x55555555, 0x55555555,
		      0x33333333, 0x33333333, 0x33333333, 0x33333333,
		      0x0F0F0F0F, 0x0F0F0F0F, 0x0F0F0F0F, 0x0F0F0F0F,
		      0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF,
		      0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF};
  __m128i B0 = _mm_load_si128((__m128i*)(magic));
  __m128i B1 = _mm_load_si128((__m128i*)(magic+4));
  __m128i B2 = _mm_load_si128((__m128i*)(magic+8));
  __m128i B3 = _mm_load_si128((__m128i*)(magic+12));
  __m128i B4 = _mm_load_si128((__m128i*)(magic+16));
  total = _mm_set1_epi32(0);
  total = _mm_xor_si128(total, total);
  for (i = 0; i < N; i+=4) {
    __m128i v0 = _mm_load_si128(vbuf+i+0);
    __m128i v1 = _mm_load_si128(vbuf+i+1);
    __m128i v2 = _mm_load_si128(vbuf+i+2);
    __m128i v3 = _mm_load_si128(vbuf+i+3);

    count0 = _mm_sub_epi32(v0, _mm_and_si128(B0, _mm_srli_epi32(v0,1)));
    count1 = _mm_sub_epi32(v1, _mm_and_si128(B0, _mm_srli_epi32(v1,1)));
    count2 = _mm_sub_epi32(v2, _mm_and_si128(B0, _mm_srli_epi32(v2,1)));
    count3 = _mm_sub_epi32(v3, _mm_and_si128(B0, _mm_srli_epi32(v3,1)));

    count0 = _mm_add_epi32( _mm_and_si128(B1,count0),
			    _mm_and_si128(B1,_mm_srli_epi32(count0,2)));
    count1 = _mm_add_epi32( _mm_and_si128(B1,count1),
			    _mm_and_si128(B1,_mm_srli_epi32(count1,2)));
    count2 = _mm_add_epi32( _mm_and_si128(B1,count2),
			    _mm_and_si128(B1,_mm_srli_epi32(count2,2)));
    count3 = _mm_add_epi32( _mm_and_si128(B1,count3),
			    _mm_and_si128(B1,_mm_srli_epi32(count3,2)));

    count0 = _mm_and_si128(B2,_mm_add_epi32(count0,
					    _mm_srli_epi32(count0,4)));
    count1 = _mm_and_si128(B2,_mm_add_epi32(count1,
					    _mm_srli_epi32(count1,4)));
    count2 = _mm_and_si128(B2,_mm_add_epi32(count2,
					    _mm_srli_epi32(count2,4)));
    count3 = _mm_and_si128(B2,_mm_add_epi32(count3,
					    _mm_srli_epi32(count3,4)));

    count0 = _mm_and_si128(B3,_mm_add_epi32(count0,
					    _mm_srli_epi32(count0,8)));
    count1 = _mm_and_si128(B3,_mm_add_epi32(count1,
					    _mm_srli_epi32(count1,8)));
    count2 = _mm_and_si128(B3,_mm_add_epi32(count2,
					    _mm_srli_epi32(count2,8)));
    count3 = _mm_and_si128(B3,_mm_add_epi32(count3,
					    _mm_srli_epi32(count3,8)));

    count0 = _mm_and_si128(B4,_mm_add_epi32(count0,
					    _mm_srli_epi32(count0,16)));
    count1 = _mm_and_si128(B4,_mm_add_epi32(count1,
					    _mm_srli_epi32(count1,16)));
    count2 = _mm_and_si128(B4,_mm_add_epi32(count2,
					    _mm_srli_epi32(count2,16)));
    count3 = _mm_and_si128(B4,_mm_add_epi32(count3,
					    _mm_srli_epi32(count3,16)));

    total = _mm_add_epi32(total,_mm_add_epi32(_mm_add_epi32(count0,count1),
					      _mm_add_epi32(count2,count3)));
  }
  _mm_store_si128((__m128i*)tmp,total);
  return tmp[0]+tmp[1]+tmp[2]+tmp[3];
}
#endif

#if defined(__GNUC__) && defined(__SSSE3__)
#include <tmmintrin.h>

__m128i popcount_ssse3_helper_1(unsigned* buf, int N) {
    __m128i* vbuf = (__m128i*)buf;
    __m128i total = _mm_setzero_si128();
    // LUT of count of set bits in each possible 4-bit nibble, from low-to-high:
    // 0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4
    const unsigned _LUT[] = {0x02010100, 0x03020201, 0x03020201, 0x04030302};
    const __m128i LUT = _mm_load_si128((__m128i*)_LUT);
    const __m128i mask = _mm_set1_epi32(0x0F0F0F0F);

    for (int i = 0; i < N; i+=4) {
        __m128i v0 = _mm_load_si128(vbuf+i+0);
        __m128i v1 = _mm_load_si128(vbuf+i+1);
        __m128i v2 = _mm_load_si128(vbuf+i+2);
        __m128i v3 = _mm_load_si128(vbuf+i+3);

        // Split each byte into low and high nibbles
        __m128i v0_lo = _mm_and_si128(mask,v0);
        __m128i v1_lo = _mm_and_si128(mask,v1);
        __m128i v2_lo = _mm_and_si128(mask,v2);
        __m128i v3_lo = _mm_and_si128(mask,v3);

        __m128i v0_hi = _mm_and_si128(mask,_mm_srli_epi16(v0,4));
        __m128i v1_hi = _mm_and_si128(mask,_mm_srli_epi16(v1,4));
        __m128i v2_hi = _mm_and_si128(mask,_mm_srli_epi16(v2,4));
        __m128i v3_hi = _mm_and_si128(mask,_mm_srli_epi16(v3,4));

        // Compute POPCNT of each byte in two halves using PSHUFB instruction for LUT
        __m128i count0 = _mm_add_epi8(_mm_shuffle_epi8(LUT,v0_lo),_mm_shuffle_epi8(LUT,v0_hi));
        __m128i count1 = _mm_add_epi8(_mm_shuffle_epi8(LUT,v1_lo),_mm_shuffle_epi8(LUT,v1_hi));
        __m128i count2 = _mm_add_epi8(_mm_shuffle_epi8(LUT,v2_lo),_mm_shuffle_epi8(LUT,v2_hi));
        __m128i count3 = _mm_add_epi8(_mm_shuffle_epi8(LUT,v3_lo),_mm_shuffle_epi8(LUT,v3_hi));

        total = _mm_add_epi8(total,_mm_add_epi8(_mm_add_epi8(count0,count1),
                                                _mm_add_epi8(count2,count3)));

    }
    // Reduce 16*8b->{-,-,-,16b,-,-,-,16b}
    const __m128i ZERO = _mm_setzero_si128();
    return _mm_sad_epu8(total,ZERO);
}


inline int popcount_ssse3(unsigned* buf,int n) {
    int N = n/4;
    __m128i count32 = _mm_setzero_si128();
    // 2^5 loop iters might overflow 8-bit counter, so
    // cap it at 2^4 iters per chunk
    const int inner_maxits = 16;
    while (N > inner_maxits) {
        count32 = _mm_add_epi32(count32,popcount_ssse3_helper_1(buf,inner_maxits));
        buf += inner_maxits*4;
        N -= inner_maxits;
    }
    if (N > 0) count32 = _mm_add_epi32(count32,popcount_ssse3_helper_1(buf,N));

    // Layout coming from PSADBW accumulation is 2*{0,32}: 0 S1 0 S0
    int count;
    _mm_store_ss((float*)&count,(__m128)(_mm_add_epi32(count32,_mm_shuffle_epi32(count32,_MM_SHUFFLE(2,2,2,2)))));
    return count;
}

#endif


int main(void)
{

	StopWatch st;
	size_t total=0;
	for(unsigned int i=0;i<1000000001;i++){
		//unsigned int pop = __builtin_popcount(i);  // Fastest!!
		//unsigned int pop = _mm_popcnt_u32(i);
		//unsigned int pop = popcountTable1(i);
		//unsigned int pop = popcountTable2(i);
		//unsigned int pop = popCountParallel(i);
		//unsigned int pop = popCountParallel64(i);
		unsigned int pop = popCountLoopSet(i);
		//unsigned int pop = popcount_sse2_32bit(&i, sizeof(unsigned int));
		//unsigned int pop = popcount_ssse3(&i, sizeof(unsigned int));
		total+=pop;
	}


    cout << "Total: " << total << " in " << st << endl;
    return 0;
}
