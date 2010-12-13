/* MiniHashTable.h
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


#ifndef MINIHASHTABLE_H
#define MINIHASHTABLE_H
//Next defines are used in HashTable. Uncomment if HashTable is not present
#define INITIAL_SIZE	1048576
#define SEED	1159241

#define HASHFN  bitwisehash

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct minihashrec
{
	char *word;
	unsigned int id;
	struct minihashrec *next;
} MINIHASHREC;


/** MiniHashTable
 *
 *  @author Javier D. Fernandez
 */        
class MiniHashTable
{
public:

	MiniHashTable();
	int TSIZE;
    int globalId;
	bool useAsCount;
	void inithashtable();
    void inithashtable(int size);
    void inithashtable(int size,bool count); //use as count (assign the same first id)
	MINIHASHREC ** get();
	MINIHASHREC * get(unsigned pos);
	MINIHASHREC * hashsearch(char *w);
	MINIHASHREC * hashupdate(char *w);
	void delete_key(char *w);
	MINIHASHREC * hashinsert(char *w);
	MINIHASHREC * hashinsert(char *w, unsigned int id);
	void dump();

	unsigned int bitwisehash(char *word, int tsize, unsigned int seed);
	int scmp( char *s1, char *s2 );

	~MiniHashTable();
	
	int size();

private:
	MINIHASHREC **hash;
	int numNodes;
};


        
#endif /*MINIHASHTABLE_H_*/
