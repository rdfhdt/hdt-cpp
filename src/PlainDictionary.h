/* PlainDictionary.h
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * Plain implementation of the Dictionary component
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
 
#ifndef PLAINDICTIONARY_H
#define PLAINDICTIONARY_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include "Dictionary.h"
#include "HashTable.h"
#include "MiniHashTable.h"
#include "SpacelessParser.hpp"

using namespace std;

/** PlainDictionary
 *   It implements the abstract class Dictionary on a 'plain approach' based on
 *   hashtables.
 *
 *  @author Javier D. Fernandez
 */
class PlainDictionary : public Dictionary
{
public:	
	/** Generic constructor */
	PlainDictionary();
	/** Constructor with size options */
	PlainDictionary(unsigned int sizeHash);
	/** Constructor for loading */
	PlainDictionary(string path, unsigned int mapping, unsigned int separator, 
			unsigned int nsubjects, unsigned int npredicates, unsigned int nobjects, unsigned int ssubobj);
	
	void checkTriple(string *node);
	unsigned int isClusterized(unsigned int id, unsigned int mode);	
	unsigned int retrieveID(string key, unsigned int mode);
	string retrieveString(unsigned int id, unsigned int mode);
	void updateID(unsigned int id, unsigned int new_id, unsigned int mode);

	void beginClustering();
	void endClustering();
	void lexicographicSort(unsigned int mapping);
	void split();
	void write(string path, unsigned int separator);

	/** Destructor */
	~PlainDictionary();

protected:
	unsigned int sizeHash;

	vector <HASHREC*> predicates;
	vector <HASHREC*> subjects_shared;
	vector <HASHREC*> subjects_not_shared;
	vector <HASHREC*> objects_not_shared;

	vector <HASHREC*> cluster_subjects_shared;
	vector <HASHREC*> cluster_subjects_not_shared;
	vector <HASHREC*> cluster_objects_not_shared;

	HashTable hashSubject;
	HashTable hashPredicate;
	HashTable hashObject;

	MiniHashTable prefixes;

	void initialize();
	void map(unsigned int mapping);

	bool static cmpLexicographic(const struct hashrec *c1, const struct hashrec *c2)
	{
		string total_c1=c1->word;
		if (c1->prefix!=NULL) total_c1 = c1->prefix->word + total_c1;
		
		string total_c2=c2->word;
		if (c2->prefix!=NULL) total_c2 = c2->prefix->word + total_c2;
	
		if (strcmp ((char*)total_c1.c_str(), (char*)total_c2.c_str())<0) 
			return true;
		else 
			return false;
	}
};

#endif  /* _PLAINDICTIONARY_H */
