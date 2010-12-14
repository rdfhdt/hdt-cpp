/* StatsGen.h
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * HDT Statistics generator.
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
 *
 */
#include "Basics.h"
#include "Literals.h"
#include "Header.h"
#include "Dictionary.h"
#include "Triples.h"
#include "RDFSyntaxMediator.h"

class StatsGen
{
public:
	/** Generic constructor */
	StatsGen();
	
	/** Parses the RDF dataset by following the features file */
	bool process(char* pathFile, char *output);
	
	/** Destructor */
	~StatsGen();
	
protected:
	istream *input;
	ifstream config;
	map<string,string> properties;
	
	Header *header;
	Dictionary *dictionary;
	Triples *triples;
	
	string *node;
	
	/** Graph parsing: SPO ('spo'), SOP ('sop'), PSO ('pso'), POS ('pos'), OSP ('osp'), OPS ('ops') */
	unsigned int parsing;
	/** ID mapping: MAPPING1 ('single'), MAPPING2 ('shared') */
	unsigned int mapping;
	/** Dictionary encoding: PLAIN ('plain'), COMPACT ('compact'), BITMAP ('bitmap'), K2TREE ('k2tree') */
	unsigned int t_encoding; 
	
	
	/** Parse the config file */
	bool parseConfig(char* pathFile);
	/** Parse a single triple from a new line read from the dataset */
	void parseTripleN3(string t);
};