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
#include <fstream>
#include <cassert>
#include <string.h>
#include <set>

using namespace std;

#include <Array.h>
#include <libcdsBasics.h>
using namespace cds_utils;

#include "CSD.h"
#include "VByte.h"

namespace csd
{
static const uint32_t FACTOR = 20; // Percentual fraction (of the original dict
				 // size) for memory allocation in building.

class CSD_PFC : public CSD
{		
  public:		
    /** General constructor **/
    CSD_PFC();

    CSD_PFC(IteratorUCharString *it, uint32_t blocksize);

    /** General destructor. */
    ~CSD_PFC();
    
    /** Returns the ID that identify s[1..length]. If it does not exist, 
	returns 0. 
	@s: the string to be located.
	@len: the length (in characters) of the string s.
    */
    uint32_t locate(const uchar *s, uint32_t len);

    /** Returns the string identified by id.
	@id: the identifier to be extracted.
    */
    uchar * extract(uint32_t id);

    /** Obtains the original Tdict from its CSD_PFC representation. Each string is
	separated by '\n' symbols.
	@dict: the plain uncompressed dictionary.
	@return: number of total symbols in the dictionary.
    */
    uint decompress(uchar **dict);

    void dumpAll();
    void dumpBlock(uint block);

    /** Returns the size of the structure in bytes. */
    uint32_t getSize();

    /** Stores a CSD_PFC structure given a file pointer.
	@fp: pointer to the file saving a CSD_PFC structure.
    */
    void save(ofstream & fp);

    /** Loads a CSD_PFC structure from a file pointer.
	@fp: pointer to the file storing a CSD_PFC structure. */
    static CSD * load(ifstream & fp);
	
		
  protected:
    uint32_t bytes;	//! Size of the Front-Coding encoded sequence (in bytes).
    uchar *text;	//! Front-Coding encoded sequence.

    uint32_t blocksize;	//! Number of strings stored in each block.
    uint32_t nblocks;	//! Number of total blocks in the dictionary.
    Array *blocks;	//! Start positions of dictionary blocks.

    /** Locates the block in where the string 's' can be stored. This method is
	based on a binary search comparing the first string in each block and
	the given string 's'.
	@s: the string to be located.
	@block: the candidate block.
	@return: a boolean value pointing if the string is located (this only
	 occurs when 's' is the first string in 'block').
    */
    bool locateBlock(const uchar *s, uint *block);

    /** Locates the offset for 's' in 'block' (returning its global ID) or 
	return 0 if it is  not exist 
	@block: block to be queried.
	@s: the required string.
	@len: the length (in characters) of the string s.
	@return: the ID for 's' or 0 if it is not exist.
    */
    uint locateInBlock(uint block, const uchar *s, uint len);

    /** Extracts the o-th string in the given 'block'.
	@block: block to be accesed.
	@o: internal offset for the required string in the block.
	@s: the extracted string.
    */
    void extractInBlock(uint block, uint o, uchar *s);


    /** Obtains the length of the long common prefix (lcp) of str1 and str2.
	@str1: first string in the comparison.
	@str2: second string in the comparison.
	@lstr1: length of the first string.
	@lstr2: length of the second string.
    */
    uint longest_common_prefix(const uchar* str1, const uchar* str2, uint lstr1, uint lstr2);
  };
};

#endif  
