/* Triples.h
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * Abstract class to implement different forms of the Triples component
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
 
#ifndef TRIPLES_H
#define TRIPLES_H

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

#include "Basics.h"
#include "Dictionary.h"

using namespace std;

/** Triples
 *   It implements an abstract class representing the features of the 
 *   Triples component in HDT. See the format description for more details.
 *
 *  @author Miguel A. Martinez-Prieto
 */
class Triples
{
public:		
	/** Inserts a triple in the graph structure */
	void insert(unsigned int subject, unsigned int predicate, unsigned int object);
	/** Retrieves a triple in the graph structure by using the abstract 
	    repesentation x,y,z*/
	TripleID retrieveTriple(unsigned int x, unsigned int y, unsigned int z);

	/** Implements a console application to interact with HDT */
	virtual void console() = 0;
	/** Implemements an heuristic-based technique for triples clustering */
	void clustering();
	/** Draws the graph (in gnuplot) for all predicates
	    @npredicates: number of predicates in the graph
	    @predicates: predicate strings
	    @max: max value to square the matrix
	    @path: path for gnuplot files storage
	*/
	void gnuplot(unsigned int npredicates, vector <string> predicates, unsigned int max, string path);
	/** Sorts the graph */
	void graphSort();
	/** Returns the graph size in triples */
	unsigned int size();
	/** Transforms HDT in format n3 */
	virtual bool transformToN3() = 0;
	/** Write the triples structure; returns the number of triples effectively writen */
	virtual unsigned int write(string path) = 0;
	
	/** Serialize HDT to a given format*/
	virtual bool serialize(char *output, char *format) = 0;

	/** Calculate degrees */
	void calculateDegrees(string path);

protected:
	Dictionary *dictionary;

	vector<TripleID> graph;

	unsigned int parsing;

	unsigned int ntriples;
	string path;

	/** Writes the GNUPLOT header-file with the parameter-based configuration:
	    @firstP: id of the current serie for sizeP=1
	    @sizeP: number of series
	    @predicate: predicate string
	    @xmin: min value in X scale
	    @xmax: max value in X scale
	    @ymin: min value in Y scale
	    @ymax: max value in Y scale
	    @filename: preffix names for files
	*/
	void gnuplotHeader(unsigned int firstP, unsigned int sizeP, string predicate, 
			   unsigned int xmin, unsigned int xmax, unsigned int ymin, unsigned int ymax, string filename);

	bool static sortXYZ(const struct TripleID c1, const struct TripleID c2)
	{
		if ((c1.x) < (c2.x)) return true;
		else
		{
			if ((c1.x) > (c2.x)) return false;
			else
			{
				if ((c1.y) < (c2.y)) return true;
				else
				{
					if ((c1.y) > (c2.y)) return false;
					else
					{
						if ((c1.z) < (c2.z)) return true;
						else return false;
					}
				}
			}
		}

		return true;
	}

	void calculateDegree(string path);
	void SPOtoOPS();
};

#include "PlainTriples.h"
#include "CompactTriples.h"
#include "BitmapTriples.h"
//#include "K2TreeTriples.h"

#endif  /* _DICTIONARY_H */
