/* CSD.h
 * Copyright (C) 2011, Rodrigo Canovas & Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Abstract class for implementing Compressed String Dictionaries following:
 *
 *   ==========================================================================
 *     "Compressed String Dictionaries"
 *     Nieves R. Brisaboa, Rodrigo Canovas, Francisco Claude, 
 *     Miguel A. Martinez-Prieto and Gonzalo Navarro.
 *     10th Symposium on Experimental Algorithms (SEA'2011), p.136-147, 2011.
 *   ==========================================================================
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
 *   Rodrigo Canovas:  rcanovas@dcc.uchile.cl
 *   Miguel A. Martinez-Prieto:  migumar2@infor.uva.es
 */

#ifndef _COMPRESSEDSTRINGDICTIONARY_H
#define _COMPRESSEDSTRINGDICTIONARY_H

#include <stdint.h>
#include <Iterator.hpp>
#include <iostream>
#include <cassert>
#include <vector>
using namespace std;

namespace csd
{
static const uint32_t PFC = 2;
static const uint32_t HTFC = 3;
static const uint32_t FMINDEX = 4;
static const uint32_t REPAIRDAC = 5;
static const uint32_t HASHHUFF = 6;

class CSD
{		
  public:
	CSD();
    /** General destructor */
    virtual ~CSD() {};

    /** Returns the ID that identify s[1..length]. If it does not exist, 
	returns 0. 
	@s: the string to be located.
	@len: the length (in characters) of the string s.
    */
    virtual size_t locate(const unsigned char *s, size_t len)=0;

    /** Returns the string identified by id.
	@id: the identifier to be extracted.
    */
    virtual unsigned char * extract(size_t id)=0;

    /**
     * Free the string returned by extract()
     */
    virtual void freeString(const unsigned char *)=0;

    /** Returns the size of the structure in bytes. */
    virtual uint64_t getSize()=0;

    virtual hdt::IteratorUCharString *listAll()=0;

    /** Returns the number of strings in the dictionary. */
    size_t getLength();

    // Search for terms by prefix. It returns a vector of a given maximum size "maxResults"
    virtual void fillSuggestions(const char *prefix, vector<string> &out, int maxResults)=0;

    // Search for terms by prefix. It returns an iterator of all results in the dictionary
    virtual hdt::IteratorUCharString *getSuggestions(const char *prefix)=0;

    // Search for terms by prefix. It returns an iterator of all results in the dictionary, by ID
    virtual hdt::IteratorUInt *getIDSuggestions(const char *prefix)=0;

    /** Stores a CSD structure given a file pointer.
	@fp: pointer to the file saving a CSD structure.
    */
    virtual void save(ostream & fp)=0;

    virtual size_t load(unsigned char *ptr, unsigned char *ptrMax)=0;

    /** Loads a CSD structure from a file pointer.
	@fp: pointer to the file storing a CSD structure. */
    static CSD * load(istream & fp);

    static CSD * create(unsigned char type);
		
  protected:
    unsigned char type; 	//! Dictionary type.
    size_t tlength;	//! Original Tdict size.
    size_t numstrings;	//! Number of elements in the dictionary.
  };

}

#endif  
