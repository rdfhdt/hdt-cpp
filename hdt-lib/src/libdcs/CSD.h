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

#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
using namespace std;

#include <libcdsBasics.h>
using namespace cds_utils;

namespace csd
{
static const uint32_t PFC = 2;
static const uint32_t HTFC = 3;
static const uint32_t FMINDEX = 4;
static const uint32_t REPAIRDAC = 5;
static const uint32_t HASHHUFF = 6;


class IteratorUCharString {
public:
	virtual bool hasNext() {
		return false;
	}

	virtual unsigned char *next() {
		return 0;
	}
};

class VectorIteratorUCharString : public IteratorUCharString {
private:
	std::vector<std::string> &vector;
	unsigned int pos;
public:
	VectorIteratorUCharString(std::vector<std::string> &vector) : vector(vector), pos(0){

	}

	virtual bool hasNext() {
		return pos<vector.size();
	}

	virtual unsigned char *next() {
		return (unsigned char *)vector[pos++].c_str();
	}
};


class CSD
{		
  public:			
    /** General destructor */
    virtual ~CSD() {};

    /** Returns the ID that identify s[1..length]. If it does not exist, 
	returns 0. 
	@s: the string to be located.
	@len: the length (in characters) of the string s.
    */
    virtual uint32_t locate(const uchar *s, uint32_t len)=0;

    /** Returns the string identified by id.
	@id: the identifier to be extracted.
    */
    virtual uchar * extract(uint32_t id)=0;

    /** Obtains the original Tdict from its CSD representation. Each string is
	separated by '\n' symbols.
	@dict: the plain uncompressed dictionary.
	@return: number of total symbols in the dictionary.
    */
    virtual uint decompress(uchar **dict)=0;

    /** Returns the size of the structure in bytes. */
    virtual uint32_t getSize()=0;

    virtual void dumpAll()=0;

    /** Returns the number of strings in the dictionary. */
    uint32_t getLength();

    /** Stores a CSD structure given a file pointer.
	@fp: pointer to the file saving a CSD structure.
    */
    virtual void save(ofstream & fp)=0;

    /** Loads a CSD structure from a file pointer.
	@fp: pointer to the file storing a CSD structure. */
    static CSD * load(ifstream & fp);
		
  protected:
    uint32_t type; 	//! Dictionary type.
    uint32_t tlength;	//! Original Tdict size.
    uint32_t length;	//! Number of elements in the dictionary.
    uint32_t maxlength; 	//! Length of the largest string in the dictionary.
  };
};

#include "CSD_PFC.h"
//#include "CSD_RePairDAC.h"
//#include "CSD_HTFC.h"
//#include "CSD_FMIndex.h"
//#include "CSD_HashHuff.h"

#endif  
