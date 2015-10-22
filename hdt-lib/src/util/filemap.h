/*
 * File: filemap.h
 * Last modified: $Date: 2012-09-19 12:02:39 +0100 (mié, 19 sep 2012) $
 * Revision: $Revision: 278 $
 * Last modified by: $Author: mario.arias $
 *
 * Copyright (C) 2012, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
 * All rights reserved.
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
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#ifndef FILE_MAP_H_
#define FILE_MAP_H_

#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

namespace hdt {

class FileMap {
private:

#ifdef WIN32
    HANDLE fd,h;
#else
	int fd;
#endif
	size_t mappedSize;
	unsigned char *ptr;

public:
	FileMap(const char *fileName);
	virtual ~FileMap();

	unsigned char *getPtr() {
		return ptr;
	}

	size_t getMappedSize() {
		return mappedSize;
	}

};

}

#endif
