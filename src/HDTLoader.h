/* HDTLoader.h
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * HDT Loader for efficient and effective RDF-based applications.
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
 
#ifndef HDTLOADER_H
#define HDTLOADER_H

#include "Basics.h"
#include "Header.h"
#include "Dictionary.h"
#include "Triples.h"

using namespace std;

/** HDT Loader
 *
 *   It parses a dataset from a known RDF syntax to HDT.  
 *   All parsing options are given in a properties file used as input.
 *
 *  @author Miguel A. Martinez-Prieto
 */
class HDTLoader
{
public:
	/** Generic constructor */
	HDTLoader();
	
	/** Implements a console application to interact with HDT through
	    the Triples component */
	void console();
	
	/** Implements a serialization from HDT to another RDF format */
	void serialize(char* location, char* format);

	/** Loads the HDT representation from the location */
	bool load(char* location);

	/** Destructor */
	~HDTLoader();
			
protected:
	Header *header;
	Dictionary *dictionary;
	Triples *triples;
};

#endif  /* _HDTLOADER_H */
