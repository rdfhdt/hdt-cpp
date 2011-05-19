/*
 * TripleOrderConvert.cpp
 *
 *  Created on: 11/05/2011
 *      Author: mck
 */

#include "TripleOrderConvert.hpp"

namespace hdt {

char TripleComponentOrderStr[7][4] = { "Unk", "SPO", "SOP", "PSO", "POS", "OSP", "OPS" };

TripleComponentOrder parseOrder(const char *str){
	for(unsigned int i=0;i<7;i++) {
		if(strcmp(TripleComponentOrderStr[i], str)==0) {
			return (TripleComponentOrder) i;
		}
	}
	return Unknown;
}

/** Swap
 * @param a Description of the param.
 * @param b Description of the param.
 * @return void
 */
inline void swap(char &a, char &b) {
	unsigned int tmp;

	tmp = a;
	a = b;
	b = tmp;
}

/** Swap
 * @param a Description of the param.
 * @param b Description of the param.
 * @return void
 */
inline void swap(unsigned int &a, unsigned int &b) {
	unsigned int tmp;

	tmp = a;
	a = b;
	b = tmp;
}

/*

This table reflect the swaps needed to convert from
a SRC ComponentOrder to a DST ComponentOrder.

There are three swaps possible:
swap1 (x and y)
swap2 (y and z)
swap3 (x and z)

The order of swaps is important, there are five different:
 xy   12
 xz   13
 yz   23
 xyz1 12 13
 xyz2 12 23

 SRC
 v
      SOP  PSO  POS  OSP  OPS   <DEST
 SPO       yz   xy   xyz2 xyz1 xz
 SOP  yz        xyz1 xz   xy   xyz2
 PSO  xy   xyz2      yz   xz   xyz1
 POS  xyz1 xz   yz        xyz2
 OSP  xyz2 xy   xz   xyz1      yz
 OPS  xz   xyz1 xyz2 xy   yz

 */

// swap 1-2
//xy xyz1 xyz2
bool swap1tab[6][6] = {
		{ 0, 0, 1, 1, 1, 0 },
		{ 0, 0, 1, 0, 1, 1 },
		{ 1, 1, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 1, 0 },
		{ 1, 1, 0, 1, 0, 0 },
		{ 0, 1, 1, 1, 0, 0 }
};

// swap 1-3
// xz xyz1
bool swap2tab[6][6] = {
		{ 0, 0, 0, 0, 1, 1 },
		{ 0, 0, 1, 1, 0, 0 },
		{ 0, 0, 0, 0, 1, 1 },
		{ 1, 1, 0, 0, 0, 0 },
		{ 0, 0, 1, 1, 0, 0 },
		{ 1, 1, 0, 0, 0, 0 }
};

// swap 2-3
// yz xyz2
bool swap3tab[6][6] = {
		{ 0, 1, 0, 1, 0, 0 },
		{ 1, 0, 0, 0, 0, 1 },
		{ 0, 1, 0, 1, 0, 0 },
		{ 0, 0, 1, 0, 1, 0 },
		{ 1, 0, 0, 0, 0, 1 },
		{ 0, 0, 1, 0, 1, 0 }
};

/**
 * Convert parsing
 * @param triple Triple to convert
 * @param from Source TripleComponentOrder
 * @param to Destination TripleComponentOrder
 */
void swapComponentOrder(UnorderedTriple *triple, TripleComponentOrder from, TripleComponentOrder to) {
	if (from == to)
		return;

	bool swap1 = swap1tab[from-1][to-1];
	bool swap2 = swap2tab[from-1][to-1];
	bool swap3 = swap3tab[from-1][to-1];

	if (swap1) {
		swap(triple->x, triple->y);
	}

	if (swap2) {
		swap(triple->x, triple->z);
	}

	if (swap3) {
		swap(triple->y, triple->z);
	}

	// Debug Conversion
#if 0
	char str[4];
	strcpy(str, TripleComponentOrderStr[from]);

	cout << "Convert from " << TripleComponentOrderStr[from] << " to " << TripleComponentOrderStr[to] << endl;

	if(swap1) {
		swap(str[0], str[1]);
		cout << "\tSwap x y" << endl;
	}

	if(swap2) {
		swap(str[0], str[2]);
		cout << "\tSwap x z" << endl;
	}

	if(swap3) {
		swap(str[1], str[2]);
		cout << "\tSwap y z" << endl;
	}
	cout << "Result: "<< str << endl;
#endif
}

}
