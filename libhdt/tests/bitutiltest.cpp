/*
 * bit375.cpp
 *
 *  Created on: 15/08/2012
 *      Author: mck
 */

#include <iostream>

#include <stdint.h>

#include "../src/util/bitutil.h"
#include "../src/util/StopWatch.hpp"

using namespace std;
using namespace hdt;

void showBinary(uint32_t val) {
	for(int i=31;i>=0;i--) {

		if( (val>>i)&1 ) {
			cout << "1";
		} else {
			cout << "0";
		}
		if(!((i)%8)) {
			cout << " ";
		}
	}
}

inline uint32_t wordSelectB(uint32_t v, uint32_t rank) {
		if(rank==0) {
			return 0;
		}
	//  cout << "Value: \t"; showBinary(v); cout << endl;

	  // Do a normal parallel bit count for a 64-bit integer,
	  // but store all intermediate steps.

	  uint32_t a =  v - ((v >> 1) & ~0U/3);					// a = (v & 0x5555...) + ((v >> 1) & 0x5555...);
	  uint32_t b = (a & ~0UL/5) + ((a >> 2) & ~0U/5);		// b = (a & 0x3333...) + ((a >> 2) & 0x3333...);
	  uint32_t c = (b + (b >> 4)) & ~0U/0x11;				// c = (b & 0x0f0f...) + ((b >> 4) & 0x0f0f...);
/*	  uint32_t d = (c * 0x1010101) >> 24;

	  cout << "a: \t"; showBinary(a); cout << endl;
	  cout << "b: \t"; showBinary(b); cout << endl;
	  cout << "c: \t"; showBinary(c); cout << endl;
	  cout << "d: \t"; showBinary(d); cout << endl;*/

	  // Now do select
	  uint32_t select  = 0;
	  uint32_t t = c & 0xFF;
	  if (rank > t) {
		  select += 16;
		  rank -= t;
	  }

	  t  = (c>>select) & 0xf;
	  if (rank > t) {
		  select += 8;
		  rank -= t;
	  }

	  t  = (b>>select) & 0x7;
	  if (rank > t) {
		  select += 4;
	  	  rank -= t;
	  }

	  t  = (a>>select) & 0x3;
	  if (rank > t) {
		  select += 2;
		  rank -= t;
	  }

	  t  = (v>>select) & 0x1;
	  if (rank > t){
		  select++;
	  }

	  return select+1;
}

inline uint32_t wordSelect1c(uint32_t v, uint32_t rank) {
	unsigned int tmp = v;
	unsigned int c=rank-1;

	cout << " Step " << c << " tmp=";
	showBinary(tmp);
	cout << endl;

	for (; tmp && c; c--)
	{
		tmp &= tmp - 1; // clear the least significant bit set
		cout << " Step " << c << " tmp=";
		showBinary(tmp);
		cout << endl;
	}

	cout << "\tValue " << hex << v << " Rank " << dec << rank << " result " << hex << tmp << dec << endl;


	if(tmp) {
		return __builtin_ffs(tmp);
	}
	return 0;
}

inline uint32_t wordSelect1d(uint32_t value, uint32_t rank) {
	uint32_t bitpos=0;
	while(rank && value) {
		rank-= value & 1;
		bitpos++;
		value>>=1;
	}
	return bitpos;
}

inline uint32_t wordSelect1f(uint32_t value, uint32_t rank) {
	uint32_t bitpos=0;
	while(rank>0 && value!=0) {
		if(value&1) {
			rank--;
		}
		bitpos++;
		value>>=1;
	}
	return bitpos;
}

int main(int argc, char **argv) {
#if 1
	uint32_t v=0x00FF00CCU;
	for(size_t i=0;i<100;i++,v++) {
		for(size_t r=0;r<=__builtin_popcount(v)+1;r++) {

			uint32_t a = wordSelect1d(v, r);
			uint32_t b = wordSelectB(v, r);

			if(a!=b) {
				cout << "RANK " << r << endl;
				//cout << "val=" << hex << i << dec << " r=" << r << " a=" << a << " b= " << b << endl
				cout << "POP: " << __builtin_popcount(v) << endl;
				cout << "Result: " << b << endl;
				cout << "Correct: " << a << endl << endl;
			}
		}
	}
#endif

#if 0
	const size_t num = 10000000;
	StopWatch st;
	size_t val=0;
	for(size_t r=0;r<64;r++) {
		for(size_t i=0;i<num;i++) {
			val+= wordSelect1f(i, r);
		}
	}

	cout << "1: " << val << " in "<< st << endl;

	val=0;
	st.reset();
	for(size_t r=0;r<64;r++) {
		for(size_t i=0;i<num;i++) {
			val+= wordSelectB(i, r);
		}
	}
	cout << "2: " << val << " in "<< st << endl;
#endif
}
