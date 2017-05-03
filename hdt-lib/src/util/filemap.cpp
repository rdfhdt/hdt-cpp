/*
 * File: mmap.cpp
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
#include <stdexcept>
#include "filemap.h"

#include <inttypes.h>
#include <fcntl.h>	// open

#ifndef WIN32
#include <unistd.h>
#endif

#include <sys/stat.h>	// stat
#include <sys/types.h>

#include <iostream>
#include <string>
#include <sstream>

#ifndef WIN32
#include <sys/mman.h> // For mmap
#endif

using namespace std;

namespace hdt {

FileMap::FileMap(const char *fileName) : fd(0), ptr(NULL) {
#ifdef WIN32

#ifdef UNICODE
    int stringLength = (int)strlen(fileName)+1;
    int bufferLength = MultiByteToWideChar(CP_ACP, 0, fileName, stringLength, 0, 0);
    wchar_t *buf = new wchar_t[bufferLength];
    MultiByteToWideChar(CP_ACP,0, fileName, stringLength, buf, bufferLength);
    wstring fileNameString(buf);
    delete [] buf;
#else
    string fileNameString(fileName);
#endif

    // Open file
    fd = CreateFile( (WCHAR*)fileNameString.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
    if (fd==INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Error creating file for mapping.");
    }

    // Guess size
    DWORD lower, upper;
    lower = GetFileSize(fd,&upper);
    mappedSize = lower | ((uint64_t)upper<<32);

    // Create mapping
    h = CreateFileMapping (fd, NULL, PAGE_READONLY, upper, lower, NULL);
    if (h==NULL) {
        CloseHandle(fd);
        throw std::runtime_error("Error creating mapping on file");
    }

    // Get pointer
    ptr = (unsigned char*) MapViewOfFile (h, FILE_MAP_READ, 0, 0, 0);
    if (ptr==NULL) {
        CloseHandle(fd);
        CloseHandle(h);
        DWORD err = GetLastError();
        cerr << "Error getting pointer: " << err << endl;
        throw std::runtime_error("Error getting pointer of the mapped file");
    }
#else
    // Linux and OSX

	// Open file
	fd = open(fileName, O_RDONLY);
	if(fd<=0) {
		throw std::runtime_error("Error opening HDT file for mapping.");
	}

	// Guess size
	struct stat statbuf;
	if(stat(fileName,&statbuf)!=0) {
		throw std::runtime_error("Error trying to guess the file size");
	}
	mappedSize = statbuf.st_size;

	// Do mmap
	ptr = (unsigned char *) mmap(0, mappedSize, PROT_READ, MAP_PRIVATE, fd, 0);
	if(ptr==MAP_FAILED) {
		throw std::runtime_error("Error trying to mmap HDT file");
	}

	// Mark as needed so the OS keeps as much as possible in memory
    madvise((void*)ptr, mappedSize, MADV_WILLNEED);

#endif
}

FileMap::~FileMap() {
#ifdef WIN32
    UnmapViewOfFile((void*)ptr);
    CloseHandle(h);
    CloseHandle(fd);
#else
    if(ptr) {
		munmap(ptr, mappedSize);
	}
	if(fd>0) {
		close(fd);
	}
#endif

}

}
