/* CSD_PCF.h
 * Copyright (C) 2011, Rodrigo Canovas & Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * This class implements a VByte-oriented Front Coding technique for
 * compression of string dictionaries.
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
 * Contacting the author:
 *   Rodrigo Canovas:  rcanovas@dcc.uchile.cl
 *   Miguel A. Martinez-Prieto:  migumar2@infor.uva.es
 */

#ifndef _CSDPFC_H
#define _CSDPFC_H

#include <iostream>
#include <cassert>
#include <string.h>
#include <set>

using namespace std;

#include <HDTListener.hpp>
#include <Iterator.hpp>

#include "CSD.h"
#include "VByte.h"
#include "../sequence/LogSequence2.hpp"

namespace csd {

class PFCIterator;

class CSD_PFC: public CSD {
public:
	/** General constructor **/
	CSD_PFC();

	CSD_PFC(hdt::IteratorUCharString *it, uint32_t blocksize,
			hdt::ProgressListener *listener = NULL);

	/** General destructor. */
	~CSD_PFC();

	/** Returns the ID that identify s[1..length]. If it does not exist,
	 returns 0.
	 @s: the string to be located.
	 @len: the length (in characters) of the string s.
	 */
	size_t locate(const unsigned char *s, size_t len);

	/** Returns the string identified by id.
	 @id: the identifier to be extracted.
	 */
	unsigned char * extract(size_t id);

	void freeString(const unsigned char *str);

	/** Obtains the original Tdict from its CSD_PFC representation. Each string is
	 separated by '\n' symbols.
	 @dict: the plain uncompressed dictionary.
	 @return: number of total symbols in the dictionary.
	 */
	size_t decompress(unsigned char **dict);

	/** Returns the size of the structure in bytes. */
	uint64_t getSize();

	/** Stores a CSD_PFC structure given a file pointer.
	 @fp: pointer to the file saving a CSD_PFC structure.
	 */
	void save(ostream & fp);

	size_t load(unsigned char *ptr, unsigned char *ptrMax);

	/** Loads a CSD_PFC structure from a file pointer.
	 @fp: pointer to the file storing a CSD_PFC structure. */
	static CSD * load(istream & fp);

	// Search for terms by prefix. It returns a vector of a given maximum size "maxResults"
	void fillSuggestions(const char *base, vector<string> &out, int maxResults);

	// Search for terms by prefix. It returns an iterator of all results in the dictionary
	hdt::IteratorUCharString *getSuggestions(const char *base);

	// Search for terms by prefix. It returns an iterator of all results in the dictionary, by ID
	hdt::IteratorUInt *getIDSuggestions(const char *prefix);

	hdt::IteratorUCharString *listAll();
protected:
	uint64_t bytes;	//! Size of the Front-Coding encoded sequence (in bytes).
	unsigned char *text;	//! Front-Coding encoded sequence.

	bool isMapped;

	uint32_t blocksize;	//! Number of strings stored in each block.
	hdt::LogSequence2 *blocks;//! Start positions of each block in the encoded sequence.
	uint32_t nblocks;   //! Number of blocks

	/** Locates the block in where the string 's' can be stored. This method is
	 based on a binary search comparing the first string in each block and
	 the given string 's'.
	 @s: the string to be located.
	 @block: the candidate block.
	 @return: a boolean value pointing if the string is located (this only
	 occurs when 's' is the first string in 'block').
	 */
	bool locateBlock(const unsigned char *s, size_t *block);

	/** Locates the offset for 's' in 'block' (returning its global ID) or
	 return 0 if it is  not exist
	 @block: block to be queried.
	 @s: the required string.
	 @len: the length (in characters) of the string s.
	 @return: the ID for 's' or 0 if it is not exist.
	 */
	size_t locateInBlock(size_t block, const unsigned char *s,
			size_t len);

	/** Extracts the o-th string in the given 'block'.
	 @block: block to be accesed.
	 @o: internal offset for the required string in the block.
	 @return: the extracted string.
	 */
	unsigned char *extractInBlock(size_t block, size_t o);

	/** Obtains the length of the long common prefix (lcp) of str1 and str2.
	 @str1: first string in the comparison.
	 @str2: second string in the comparison.
	 @lstr1: length of the first string.
	 @lstr2: length of the second string.
	 */
	inline size_t longest_common_prefix(const unsigned char* str1,
			const unsigned char* str2, size_t lstr1, size_t lstr2);

	friend class PFCIterator;
	friend class PFCSuggestionIterator;
	friend class PFCSuggestionIDIterator;
};

class PFCIterator: public hdt::IteratorUCharString {
private:
	CSD_PFC *pfc;
	size_t max;
	size_t count;
public:
	PFCIterator(CSD_PFC *pfc) :
			pfc(pfc), count(1) {
		max = pfc->getLength();
	}

	virtual ~PFCIterator() {
	}

	bool hasNext() {
		return count <= max;
	}

	unsigned char *next() {
		return pfc->extract(count++);
	}

	size_t getNumberOfElements() {
		return max;
	}

	virtual void freeStr(unsigned char *ptr) {
		pfc->freeString(ptr);
	}
};

class PFCSuggestionIterator: public hdt::IteratorUCharString {
private:
	CSD_PFC *pfc;
	size_t max;
	size_t count;
	const char *prefix;
	bool hasnext;
	string tmpStr;
	size_t block;
	bool terminate;
	size_t idInBlock;
	size_t delta;
	size_t pos;
	size_t slen;
	size_t prefixlen;
public:

	PFCSuggestionIterator(CSD_PFC *pfc, const char *prefix) :
			pfc(pfc), count(1), prefix(prefix) {
		max = pfc->getLength();
		prefixlen = strlen(prefix);
		hasnext = false;
		terminate = false;
		block = 0;
		idInBlock = 0;
		delta = 0;
		pos=0;
		slen=0;
		tmpStr = "";
		pfc->locateBlock((unsigned char *) prefix, &block); //locate block where the substring might be located

		if (!pfc->text || !pfc->blocks || block >= pfc->nblocks) {
			return;
		}

		// locate the first occurrence
		// locate the block where the prefix should be located
		locateBlock();
		if (!hasnext) { //try to locate it inside a block
			locateInternal();
		}
	}
	void locateBlock() {
		hasnext=false;
		if (block >= pfc->nblocks){
			terminate=true;
		}
		if (!terminate){
			pos = pfc->blocks->get(block);

			delta = 0;
			idInBlock = 0;

			// Read the first string
			tmpStr.clear();
			tmpStr.append((char*) (pfc->text + pos));

			slen = tmpStr.length() + 1;
			pos += slen;

			int cmp = strncmp(prefix, tmpStr.c_str(), prefixlen);
			if (cmp == 0) { //substring found!
				hasnext = true;
			} else if (cmp < 0) {
				terminate = true;
				hasnext = false;
			}

			idInBlock++;
		}
	}
	void locateInternal() {
		hasnext=false;
		// Scanning the block until a decission about the existence of 's' can be made.

		while (((idInBlock < pfc->blocksize) && (pos < pfc->bytes) && !terminate) && !hasNext()) {
			// Decode the prefix
            if(sizeof(delta) == 8)
                pos += VByte::decode(pfc->text + pos, pfc->text + pfc->bytes, (uint64_t*) &delta);
            else
                pos += VByte::decode(pfc->text + pos, pfc->text + pfc->bytes, (uint32_t*) &delta);

			// Guess suffix size
			slen = strlen((char*) pfc->text + pos) + 1;

			tmpStr.resize(delta);
			tmpStr.append((char*) pfc->text + pos);

			int cmp = strncmp(prefix, tmpStr.c_str(), prefixlen);
			if (cmp == 0) {
				hasnext = true;

			} else if (cmp < 0) {
				terminate = true;
				hasnext = false;
			}

			pos += slen;
			idInBlock++;
		}
	}

	virtual ~PFCSuggestionIterator() {
	}

	bool hasNext() {
		if (terminate)
			return false;
		else
			return hasnext;
	}

	unsigned char *next() {
		char* currentSolution =strdup(tmpStr.c_str());
		// prepare for the following solution
		hasnext=false;
		while (!hasnext&&!terminate){
			locateInternal();
			if (!hasnext){
				block++;
				locateBlock();
			}
		}
		return (unsigned char*)currentSolution;
	}

	size_t getNumberOfElements() {
		return 0; //not possible to estimate accurately.
	}

	virtual void freeStr(unsigned char *ptr) {
		pfc->freeString(ptr);
	}
};



class PFCSuggestionIDIterator: public hdt::IteratorUInt {
private:
	CSD_PFC *pfc;
	size_t max;
	size_t count;
	const char *prefix;
	bool hasnext;
	string tmpStr;
	size_t block;
	bool terminate;
	size_t idInBlock;
	size_t delta;
	size_t pos;
	size_t slen;
	size_t prefixlen;
public:

	PFCSuggestionIDIterator(CSD_PFC *pfc, const char *prefix) :
			pfc(pfc), count(1), prefix(prefix) {
		max = pfc->getLength();
		prefixlen = strlen(prefix);
		hasnext = false;
		terminate = false;
		block = 0;
		idInBlock = 0;
		delta = 0;
		pos=0;
		slen=0;
		tmpStr = "";
		pfc->locateBlock((unsigned char *) prefix, &block); //locate block where the substring might be located

		if (!pfc->text || !pfc->blocks || block >= pfc->nblocks) {
			return;
		}

		// locate the first occurrence
		// locate the block where the prefix should be located
		locateBlock();
		if (!hasnext) { //try to locate it inside a block
			locateInternal();
		}
	}
	void locateBlock() {
		hasnext=false;
		if (block >= pfc->nblocks){
			terminate=true;
		}
		if (!terminate){
			pos = pfc->blocks->get(block);

			delta = 0;
			idInBlock = 0;

			// Read the first string
			tmpStr.clear();
			tmpStr.append((char*) (pfc->text + pos));

			slen = tmpStr.length() + 1;
			pos += slen;

			int cmp = strncmp(prefix, tmpStr.c_str(), prefixlen);
			if (cmp == 0) { //substring found!
				hasnext = true;
			} else if (cmp < 0) {
				terminate = true;
				hasnext = false;
			}

			idInBlock++;
		}
	}
	void locateInternal() {
		hasnext=false;
		// Scanning the block until a decission about the existence of 's' can be made.

		while (((idInBlock < pfc->blocksize) && (pos < pfc->bytes) && !terminate) && !hasNext()) {
			// Decode the prefix
            if(sizeof(delta) == 8)
                pos += VByte::decode(pfc->text + pos, pfc->text + pfc->bytes, (uint64_t*) &delta);
            else
                pos += VByte::decode(pfc->text + pos, pfc->text + pfc->bytes, (uint32_t*) &delta);

			// Guess suffix size
			slen = strlen((char*) pfc->text + pos) + 1;

			tmpStr.resize(delta);
			tmpStr.append((char*) pfc->text + pos);

			int cmp = strncmp(prefix, tmpStr.c_str(), prefixlen);
			if (cmp == 0) {
				hasnext = true;

			} else if (cmp < 0) {
				terminate = true;
				hasnext = false;
			}

			pos += slen;
			idInBlock++;
		}
	}

	virtual ~PFCSuggestionIDIterator() {
	}

	bool hasNext() {
		if (terminate)
			return false;
		else
			return hasnext;
	}

	size_t next() {
		size_t currentSolution = (block*pfc->blocksize)+idInBlock;
		// prepare for the following solution
		hasnext=false;
		while (!hasnext&&!terminate){
			locateInternal();
			if (!hasnext){
				block++;
				locateBlock();
			}
		}
		return currentSolution;
	}

	size_t getNumberOfElements() {
		return 0; //not possible to estimate accurately.
	}

	virtual void freeStr(unsigned char *ptr) {
		pfc->freeString(ptr);
	}
};

}

#endif
