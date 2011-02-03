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

#include "MiniHashTable.h"

/** Mini Hash Table
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
MiniHashTable::MiniHashTable() {
	globalId = 1; //initialize globalId
}

/** Get
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
MINIHASHREC**
MiniHashTable::get() {
	return hash;
}

/** Get
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
//get rec at position i
MINIHASHREC*
MiniHashTable::get(unsigned pos) {
	return hash[pos];
}

/** Init Hash Table
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
/* Create hash table, initialise ptrs to NULL */
void MiniHashTable::inithashtable() {
	int i;
	TSIZE = INITIAL_SIZE;

	numNodes = 0;
	hash = (MINIHASHREC **) malloc(sizeof(MINIHASHREC *) * TSIZE);

	for (i = 0; i < TSIZE; i++)
		hash[i] = (MINIHASHREC *) NULL;
}

/** Init Hash Table
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
/* Create hash table with a given size, initialise ptrs to NULL */
void MiniHashTable::inithashtable(int size) {
	int i;
	TSIZE = size;

	numNodes = 0;
	hash = (MINIHASHREC **) malloc(sizeof(MINIHASHREC *) * TSIZE);

	for (i = 0; i < TSIZE; i++)
		hash[i] = (MINIHASHREC *) NULL;

	useAsCount = false;
}

/** Init Hash Table
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
/* Create hash table with a given size, initialise ptrs to NULL */
void MiniHashTable::inithashtable(int size, bool count) {
	int i;
	TSIZE = size;

	numNodes = 0;
	hash = (MINIHASHREC **) malloc(sizeof(MINIHASHREC *) * TSIZE);

	for (i = 0; i < TSIZE; i++)
		hash[i] = (MINIHASHREC *) NULL;

	useAsCount = count;
}

/** Detele Key
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
void MiniHashTable::delete_key(char *w) {
	MINIHASHREC *htmp, *hprv;
	unsigned int hval = HASHFN(w, TSIZE, SEED);

	for (hprv = NULL, htmp = hash[hval]
	; htmp != NULL && scmp(htmp->word, w) != 0; hprv = htmp, htmp = htmp->next) {
		;
	}

	if (hprv == NULL)
		hash[hval] = htmp->next;
	else
		hprv->next = htmp->next;

	free(htmp->word);
	numNodes--;
	//    free(htmp);
	htmp = NULL;
}

/** Hash Search
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
/* Search hash table for given string, return record if found, else NULL */
MINIHASHREC *
MiniHashTable::hashsearch(char *w) {
	MINIHASHREC *htmp, *hprv;
	unsigned int hval = HASHFN(w, TSIZE, SEED);

	for (hprv = NULL, htmp = hash[hval]
	; htmp != NULL && scmp(htmp->word, w) != 0; hprv = htmp, htmp = htmp->next) {
		;
	}

	if (hprv != NULL && htmp != NULL) /* move to front on access */
	{
		hprv->next = htmp->next;
		htmp->next = hash[hval];
		hash[hval] = htmp;
	}

	return (htmp);
}

/* Search hash table for given string, insert if not found
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
MINIHASHREC*
MiniHashTable::hashinsert(char *w) {
	MINIHASHREC *htmp, *hprv;

	unsigned int hval = HASHFN(w, TSIZE, SEED);

	for (hprv = NULL, htmp = hash[hval]
	; htmp != NULL && scmp(htmp->word, w) != 0; hprv = htmp, htmp = htmp->next) {
		;
	}

	if (htmp == NULL) {

		htmp = (MINIHASHREC *) malloc(sizeof(MINIHASHREC));

		htmp->word = (char *) malloc(strlen(w) + 1);
		strcpy(htmp->word, w); //copy the value


		htmp->id = globalId;
		if (useAsCount == false) {
			globalId++;
		}

		numNodes++;
		htmp->next = NULL;

		if (hprv == NULL)
			hash[hval] = htmp;
		else
			hprv->next = htmp;

		/* new records are not moved to front */
	} else {

		if (hprv != NULL) /* move to front on access */
		{
			hprv->next = htmp->next;
			htmp->next = hash[hval];
			hash[hval] = htmp;
		}

	}

	return htmp;
}

/** Search hash table for given string, insert with the given id if not found
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
MINIHASHREC*
MiniHashTable::hashinsert(char *w, unsigned int id) {
	MINIHASHREC *htmp, *hprv;

	unsigned int hval = HASHFN(w, TSIZE, SEED);

	for (hprv = NULL, htmp = hash[hval]
	; htmp != NULL && scmp(htmp->word, w) != 0; hprv = htmp, htmp = htmp->next) {
		;
	}

	if (htmp == NULL) {

		htmp = (MINIHASHREC *) malloc(sizeof(MINIHASHREC));

		htmp->word = (char *) malloc(strlen(w) + 1);
		strcpy(htmp->word, w); //copy the value


		htmp->id = id;

		numNodes++;
		htmp->next = NULL;

		if (hprv == NULL)
			hash[hval] = htmp;
		else
			hprv->next = htmp;

		/* new records are not moved to front */
	} else {

		if (hprv != NULL) /* move to front on access */
		{
			hprv->next = htmp->next;
			htmp->next = hash[hval];
			hash[hval] = htmp;
		}

	}

	return htmp;
}

/* Search hash table for given string, insert if not found, increase id if found.
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
MINIHASHREC*
MiniHashTable::hashupdate(char *w) {
	MINIHASHREC *htmp, *hprv;

	unsigned int hval = HASHFN(w, TSIZE, SEED);

	for (hprv = NULL, htmp = hash[hval]
	; htmp != NULL && scmp(htmp->word, w) != 0; hprv = htmp, htmp = htmp->next) {
		;
	}

	if (htmp == NULL) {

		htmp = (MINIHASHREC *) malloc(sizeof(MINIHASHREC));

		htmp->word = (char *) malloc(strlen(w) + 1);
		strcpy(htmp->word, w); //copy the value


		htmp->id = globalId;
		if (useAsCount == false) {
			globalId++;
		}

		numNodes++;
		htmp->next = NULL;

		if (hprv == NULL)
			hash[hval] = htmp;
		else
			hprv->next = htmp;

		/* new records are not moved to front */
	} else {
		htmp->id = htmp->id + 1;

		if (hprv != NULL) /* move to front on access */
		{
			hprv->next = htmp->next;
			htmp->next = hash[hval];
			hash[hval] = htmp;
		}

	}

	return htmp;
}

/** Dump
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
void MiniHashTable::dump() {
	int i;
	MINIHASHREC *ptr;

	for (i = 0; i < TSIZE; i++) {
		for (ptr = hash[i]; ptr != NULL; ptr = ptr->next) {
			// ptr stores current element in the iteration

		}
	}
}

/* Bitwise hash function.  Note that tsize does not have to be prime.
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
unsigned int MiniHashTable::bitwisehash(char *word, int tsize,
		unsigned int seed) {
	char c;
	unsigned int h;

	h = seed;
	for (; (c = *word) != '\0'; word++) {
		h ^= ((h << 5) + c + (h >> 2));
	}
	return ((unsigned int) ((h & 0x7fffffff) % tsize));
}

/** SCMP
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
int MiniHashTable::scmp(char *s1, char *s2) {
	while (*s1 != '\0' && *s1 == *s2) {
		s1++;
		s2++;
	}
	return (*s1 - *s2);
}

/** Destructor for MiniHashTable */
MiniHashTable::~MiniHashTable() {
	for (int i = 0; i < TSIZE; i++) {
		MINIHASHREC* ptr;
		while (hash[i] != NULL) {
			ptr = hash[i]->next;

			free(hash[i]->word);

			//if (hash[i]->from != NULL) free(hash[i]->from);
			free( hash[i]);
			hash[i] = ptr;
		}
	}

	free( hash);
}

/** Size
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
int MiniHashTable::size() {
	return numNodes;
}
