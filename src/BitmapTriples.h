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


typedef struct minipair
{
	unsigned int source;
	unsigned int target;
} MINIPAIR;


/** BitmapTriples
 *   It implements the abstract class Triples with bitmap structures.
 *
 *  @author Miguel A. Martinez-Prieto
 */
class BitmapTriples : public Triples
{
public:	
	/** Generic constructor */
	BitmapTriples();
	/** Constructor for parsing*/
	BitmapTriples(Dictionary *dictionary, unsigned int ntriples, unsigned int parsing);
	/** Constructor for loading*/
	BitmapTriples(Dictionary *dictionary, unsigned int ntriples, unsigned int parsing, string path);

	void console();
	bool transformToN3();
	unsigned int write(string path);
	/** Serialize HDT to a given format*/
	bool serialize(char *output, char *format);
	
	/** Load the full graph to main memory */
	bool loadGraphMemory();
		
	static unsigned int write(vector<TripleID> &graph, string path);
	
	
	/** Show Vocabulary stats */
	void vocabStats();
	
	/** Destructor */
	~BitmapTriples();

protected:
	/** Bitmap representations of the graph structure */
	BitSequence *bitmapY;
	BitSequence *bitmapZ;

	FILE *fileY, *fileZ;
	//FILE *filebY, *filebZ;

	bool load;

	/** Simple query parser, returns a TripleID value with variables
 	    represented as negative numbers */
	Query parseQ(string q);

	/** ASK SPARQL-query implementation on Check&Find algorithm for HDT*/
	bool ask(string q);

	/** CONSTRUCT SPARQL-query implementation on Check&Find algorithm for HDT*/
	int construct(string q);

	/** SELECT SPARQL-query implementation on Check&Find algorithm for HDT*/
	int select(string q, vector<TripleString> **triples);

	/** Builds a bitstring vector 'v' with 'elems' elements. */
	static void buildBitString(BitString **bs, vector<uint> *v, uint elems);

	/** Util for vocab stats */	
	int getLength(vector<minipair> * triplesSubClass,int current, MiniHashTable *subclassLengths);

	/** sort minipair */
	bool static pairSort(const struct minipair c1, const struct minipair c2)
	{
		if ((c1.source) < (c2.source)) return true;
		else
		{
			if ((c1.source) > (c2.source)) return false;
			else
			{
				if ((c1.target) < (c2.target)) return true;
				else
				{
					if ((c1.target) > (c2.target)) return false;
						else return false;
					
				}
			}
		}

		return true;
	}
	// Returns the position of the element or -1 if not found
	int static
	FindTransition(const vector<minipair> vocabularies, const int id)
	{         
	  int pos=-1;//to indicate not found
	  int l = 0, r = vocabularies.size()-1, c = (l+r)/2;

	  while (l<=r)  
	  {     
	    if (vocabularies[c].source == id)
	    { 
	      pos = c; 
	      break;
	    }
	    if (vocabularies[c].source < id) 
	       l = c+1;
	    else r = c-1;
	       c = (l+r)/2;
	  }
	  return pos;
	}
	
};

#endif  /* _BITMAPTRIPLES_H */
	 
