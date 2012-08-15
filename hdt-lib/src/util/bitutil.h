/*
 * bitutil.h
 *
 *  Created on: 14/08/2012
 *      Author: mck
 */

#ifndef BITUTIL_H_
#define BITUTIL_H_

#include <stdio.h>
#include <stdint.h>

namespace hdt {

extern const unsigned char popcount_tab[256];

/** bits needed to represent a number between 0 and n */
inline uint32_t bits(uint32_t n) {
	uint32_t b = 0;
	while (n) { b++; n >>= 1; }
	return b;
}

inline uint32_t wordSelect1(uint32_t value, uint32_t rank) {
	uint32_t bitpos=0;
	while(rank && value) {
		rank -= value & 1;
		bitpos++;
		value>>=1;
	}
	return bitpos;
}

/** reads bit p from e */
#define bitget(e,p) ((((e)[(p)>>5] >> ((p)&0x1F))) & 1)

/** sets bit p in e */
inline void bitset(uint32_t * e, size_t p) {
	e[p/32] |= (1<<(p%32));
}

/** cleans bit p in e */
inline void bitclean(uint32_t * e, size_t p) {
	e[p/32] &= ~(1<<(p%32));
}

/** Counts the number of 1s in x */
inline uint32_t popcount64(const uint64_t x) {
#ifdef __SSE4_2__
	return __builtin_popcountll(x);
#else
	return popcount_tab[(x) & 0xff]  + popcount_tab[(x >>  8) & 0xff]
	     + popcount_tab[(x >> 16) & 0xff]  + popcount_tab[(x >> 24) & 0xff]
         + popcount_tab[(x >> 32) & 0xff]  + popcount_tab[(x >> 40) & 0xff]
         + popcount_tab[(x >> 48) & 0xff]  + popcount_tab[(x >> 56) & 0xff];
#endif
}

/** Counts the number of 1s in x */
inline uint32_t popcount32(const uint32_t x) {
#ifdef __SSE4_2__
	return __builtin_popcount(x);
#else
	return popcount_tab[(x >>  0) & 0xff]  + popcount_tab[(x >>  8) & 0xff]
	                                                          + popcount_tab[(x >> 16) & 0xff] + popcount_tab[(x >> 24) & 0xff];
#endif
}

/** Counts the number of 1s in the lower 16 bits of x */
inline uint32_t popcount16(const uint32_t x) {
#ifdef __SSE4_2__
	return __builtin_popcount(x & 0xffff);
#else
	return popcount_tab[x & 0xff]  + popcount_tab[(x >>  8) & 0xff];
#endif
}

/** Counts the number of 1s in the lower 8 bits of x */
inline uint32_t popcount8(const uint32_t x) {
#ifdef __SSE4_2__
	return __builtin_popcount(x & 0xff);
#else
	return popcount_tab[x & 0xff];
#endif
}

}

#endif /* BITUTIL_H_ */
