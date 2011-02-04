/* BitmapTriples.h
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * Bitmap-based encoding for Triples
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
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A Martinez-Prieto:  migumar2@infor.uva.es
 */

#ifndef BITMAPTRIPLES_H
#define BITMAPTRIPLES_H

#include <vector>
#include <iostream>
#include <algorithm>

#include <libcdsBasics.h>
#include <BitSequenceRG.h>
#include <BitString.h>

#include "Basics.h"
#include "Triples.h"
#include "HashTable.h"

using namespace cds_utils;
using namespace cds_static;
using namespace std;

static const int ZPLAIN = 1;

typedef struct minipair {
	unsigned int source;
	unsigned int target;
} MINIPAIR;

class BitmapTriples: public Triples {
public:
	BitmapTriples();
	BitmapTriples(Dictionary *dictionary, unsigned int ntriples,
			unsigned int parsing);
	BitmapTriples(Dictionary *dictionary, unsigned int ntriples,
			unsigned int parsing, string path);
	void console();
	bool transformToN3();
	unsigned int write(string path);
	bool serialize(char *output, char *format);
	bool loadGraphMemory();
	static unsigned int write(vector<TripleID> &graph, string path);
	void vocabStats();
	~BitmapTriples();

protected:
	BitSequence *bitmapY;
	BitSequence *bitmapZ;
	FILE *fileY, *fileZ;
	bool load;
	Query parseQ(string q);
	bool ask(string q);
	int construct(string q);
	int select(string q, vector<TripleString> **triples);
	static void buildBitString(BitString **bs, vector<uint> *v, uint elems);
	int getLength(vector<minipair> * triplesSubClass, int current,
			MiniHashTable *subclassLengths);

	bool static pairSort(const struct minipair c1, const struct minipair c2) {
		if ((c1.source) < (c2.source))
			return true;
		else {
			if ((c1.source) > (c2.source))
				return false;
			else {
				if ((c1.target) < (c2.target))
					return true;
				else {
					if ((c1.target) > (c2.target))
						return false;
					else
						return false;
				}
			}
		}
		return true;
	}

	int static FindTransition(const vector<minipair> vocabularies, const int id) {
		int pos = -1;//to indicate not found
		int l = 0, r = vocabularies.size() - 1, c = (l + r) / 2;

		while (l <= r) {
			if (vocabularies[c].source == id) {
				pos = c;
				break;
			}
			if (vocabularies[c].source < id)
				l = c + 1;
			else
				r = c - 1;
			c = (l + r) / 2;
		}
		return pos;
	}
};

#endif  /* _BITMAPTRIPLES_H */

