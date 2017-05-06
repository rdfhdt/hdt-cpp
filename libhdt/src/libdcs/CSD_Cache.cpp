/*
 * File: CSD_Cache.cpp
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

#include <stdlib.h>

#include <string.h>
#include "CSD_Cache.h"

namespace csd
{
CSD_Cache::CSD_Cache(CSD *child) : child(child)//, cacheint(65536), cachestr(1024)
{
	assert(child);
	numstrings = child->getLength();
}


CSD_Cache::~CSD_Cache()
{
	delete child;
}

size_t CSD_Cache::locate(const unsigned char *s, size_t len)
{
	// FIXME: Not working.
#if 0
	LRU_Str::const_iterator it = cachestr.find((char *)s);

	if (it != cachestr.end()) {
		// Key found: retrieve its associated value
		cout << "1retrieving: " << it.key() << " -> " << it.value() << endl;
		return it.value();
	} else {
		// Key not found: compute and insert the value
		cout << "1not found" << s << endl;
        size_t value = child->locate(s, len);
		cachestr[(char *)s] = value;
		return value;
	}
#endif
	return child->locate(s, len);
}


unsigned char* CSD_Cache::extract(size_t id)
{
	return child->extract(id);
/*
    try {
        const string &value = cacheint.get(id);
        size_t len = value.length();
        unsigned char *ptr = (unsigned char *)malloc((1+len)*sizeof(unsigned char));
        strncpy((char *)ptr, (const char *)value.c_str(), len);
        ptr[len]='\0';
        return ptr;
    }catch(std::range_error& e) {
        // Key not found: compute and insert the value
        char *value = (char *) child->extract(id);

        string str(value);
        cacheint.put(id,str);

        return (unsigned char *)value;
    }
*/
}

void CSD_Cache::freeString(const unsigned char *str) {
	// Do nothing.
}

uint64_t CSD_Cache::getSize()
{
	return child->getSize();
}

void CSD_Cache::save(ostream &fp)
{
	child->save(fp);
}

CSD* CSD_Cache::load(istream &fp)
{
	throw std::logic_error("Not imlemented");
}

}
