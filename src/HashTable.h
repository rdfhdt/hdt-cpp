/* HashTable.h
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * Hash Table implementation with Linear Probbing
 * Based on J. Zobel implementation, April 2001.
 * ---------------------------------------------------------------------
 * Permission to use this code is freely granted, provided that this
 * statement is retained.
 * ---------------------------------------------------------------------
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

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "MiniHashTable.h"

#define INITIAL_SIZE	1048576
#define SEED	1159241

#define HASHFN  bitwisehash

typedef struct hashrec
{
	char *word; //or suffix
	int count;
	bool shared;
	unsigned int id;
	unsigned int new_id;  
	MINIHASHREC* prefix;
	struct hashrec *next;        
} HASHREC;

/** HashTable
 *
 *  @author Javier D. Fernandez
 */
class HashTable
{
public:
	HashTable();
    	unsigned int TSIZE;
    	bool countOriginalOrder;
	void inithashtable(bool countOrder);
    	void inithashtable(unsigned int size, bool countOrder);
	HASHREC ** get();
	HASHREC * get(unsigned pos);
	HASHREC * hashsearch(char *w);
	HASHREC * hashsearch(HASHREC* rec); //search an equivalent element
	void delete_key(char *w);
	HASHREC * hashinsert(char *w, unsigned int &globalId, MINIHASHREC* prefix); //insert not shared, prefix or not
	HASHREC * hashinsert(HASHREC* shared, unsigned int &globalId); //insert shared
	void dump();

	unsigned int bitwisehash(char *word, unsigned int tsize, unsigned int seed);
	int scmp( HASHREC* rec, char *s2 );
	
	~HashTable();
	
	unsigned int size();

private:
	HASHREC **hash;
	int numNodes;
};


        
#endif /*HASHTABLE_H_*/
