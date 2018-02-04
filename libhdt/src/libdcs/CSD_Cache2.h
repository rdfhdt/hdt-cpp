/*
 * File: CSD_Cache2.hpp
 * Last modified: $Date: 2011-08-21 05:35:30 +0100 (dom, 21 ago 2011) $
 * Revision: $Revision: 180 $
 * Last modified by: $Author: mario.arias $
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
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#ifndef _CSDCACHE2_H
#define _CSDCACHE2_H

#include <iostream>
#include <cassert>
#include <string>
#include <string.h>
#include <set>

using namespace std;

#include <HDTListener.hpp>

#include "CSD.h"

namespace csd
{

class CSD_Cache2 : public CSD
{
private:
	CSD *child;
	vector<unsigned char *> array;

  public:		
    /** General constructor **/
	CSD_Cache2(CSD *child);

    /** General destructor. */
    ~CSD_Cache2();
    
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

    hdt::IteratorUCharString *listAll() { return child->listAll(); }

    /** Returns the size of the structure in bytes. */
    uint64_t getSize();

    /** Stores a CSD_PFC structure given a file pointer.
	@fp: pointer to the file saving a CSD_PFC structure.
    */
    void save(ostream & fp);

    size_t load(unsigned char *ptr, unsigned char *ptrMax) {
        return child->load(ptr, ptrMax);
    }

    /** Loads a CSD_PFC structure from a file pointer.
	@fp: pointer to the file storing a CSD_PFC structure. */
    static CSD * load(istream & fp);

    // Search for terms by prefix. It returns a vector of a given maximum size "maxResults"
    void fillSuggestions(const char *prefix, vector<string> &out, int maxResults) {
    	child->fillSuggestions(prefix, out, maxResults);
    }

    // Search for terms by prefix. It returns an iterator of all results in the dictionary
	hdt::IteratorUCharString *getSuggestions(const char *prefix){
		return child->getSuggestions(prefix);
	}

	// Search for terms by prefix. It returns an iterator of all results in the dictionary, by ID
	hdt::IteratorUInt *getIDSuggestions(const char *prefix){
		return child->getIDSuggestions(prefix);
	}

    CSD *getChild() {
    	return child;
    }
  };
};

#endif  
