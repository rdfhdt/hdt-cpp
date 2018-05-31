/*
 * File: CSD_Cache2.cpp
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
#include "CSD_Cache2.h"

namespace csd
{
CSD_Cache2::CSD_Cache2(CSD *child) : child(child)
{
	assert(child);
	numstrings = child->getLength();

	array.resize(child->getLength(), NULL);
}


CSD_Cache2::~CSD_Cache2()
{
	for(std::vector<unsigned char *>::iterator it = array.begin(); it != array.end(); ++it) {
		unsigned char *value = *it;
		if(value!=NULL) {
			child->freeString(*it);
		}
	}

	delete child;
}

size_t CSD_Cache2::locate(const unsigned char *s, size_t len)
{
	// FIXME: Not implemented
	return child->locate(s, len);
}


unsigned char* CSD_Cache2::extract(size_t id)
{
	if(id<1 || id>array.size()) {
		return NULL;
	}

	if(array[id-1]!=NULL) {
		return array[id-1];
	}

	// Not found, fetch and add
	unsigned char *value = child->extract(id);

	array[id-1] = value;

	return value;
}

void CSD_Cache2::freeString(const unsigned char *str) {
	// Do nothing, all freed on destruction.
}

uint64_t CSD_Cache2::getSize()
{
	return child->getSize();
}

void CSD_Cache2::save(ostream &fp)
{
	child->save(fp);
}

CSD* CSD_Cache2::load(istream &fp)
{
	throw std::logic_error("Not imlemented");
}

}
