/* Dictionary.h
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * Abstract class to implement different forms of the Dictionary component
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
 
#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>

#include "Basics.h"

using namespace std;

/** Dictionary
 *   It implements an abstract class representing the features of the 
 *   Dictionary component in HDT. See the format description for more details.
 *
 *  @author Miguel A. Martinez-Prieto
 */
class Dictionary
{
public:		
	/** Checks all components in a triple. A text key (playing roles of 
	    subject, predicate or object) in inserted if it does not exist. */
	virtual void checkTriple(string *node) = 0;	
	/** Checks if it has been clusterized. */
	virtual unsigned int isClusterized(unsigned int id, unsigned int mode) = 0;
	/** Transforms the text key in its ID in the dictionary.
	    Mode means the vocabulary required. */
	virtual unsigned int retrieveID(string key, unsigned int mode) = 0;
	/** Transforms the ID in the string key associated.
	    Mode means the vocabulary required. */
	virtual string retrieveString(unsigned int id, unsigned int mode) = 0;
	/** Updates the id for a string key.
	    Mode means the vocabulary required. */
	virtual void updateID(unsigned int id, unsigned int new_id, unsigned int mode) = 0;


	/** Updates the dictionary before the clustering */
	virtual void beginClustering() = 0;
	/** Re-updates the dictionary after the clustering */
	virtual void endClustering() = 0;
	/** Lexicographic Sorting */
	virtual void lexicographicSort(unsigned int mapping) = 0;
	/** Splits vocabularies */
	virtual void split() = 0;
	/** Write the dictionary */
	virtual void write(string path, unsigned int separator) = 0;

	/** Returns the max ID used to label any node in the graph */
	unsigned int getMaxID();
	/** Returns the mapping policy */
	unsigned int getMapping();
	/** Returns the number of different subjects in the Dictionary */
	unsigned int getNsubjects();
	/** Returns the number of different predicates in the Dictionary */
	unsigned int getNpredicates();
	/** Returns the number of different objects in the Dictionary */
	unsigned int getNobjects();
	/** Returns the number of shared subjects/objects in the Dictionary */
	unsigned int getSsubobj();

protected:
	/** ID-mapping: MAPPING1 ('single'), MAPPING2 ('shared') */
	unsigned int mapping;
	/** Total subjects */
	unsigned int nsubjects;
	/** Total predicates */
	unsigned int npredicates;
	/** Total objects */
	unsigned int nobjects;
	/** Shared subjects+objects */
	unsigned int ssubobj;

	/** Initialize and configures structures for dictionary building */
	virtual void initialize() = 0;
	/** Map vocabularies */
	virtual void map(unsigned int mapping) = 0;
};

#include "PlainDictionary.h"

#endif  /* _DICTIONARY_H */
