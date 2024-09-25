/*
 * File: StopWatch.hpp
 * Last modified: $Date$
 * Revision: $Revision: 180 $
 *
 * Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 * Copyright (C) 2012, Mario Arias
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
 * Contacting the author:
 *   Mario Arias:               mario.arias@gmail.com
 *
 */

#ifndef FILEUTIL_HPP_
#define FILEUTIL_HPP_

#include <stdint.h>
#include <iostream>
#include <fstream>

#include "../third/gzstream.h"
#include "../third/fdstream.hpp"
#include "../third/memstream.hpp"

#include <HDTListener.hpp>

using namespace std;

class fileUtil {
public:
	static uint64_t getSize(istream &in);
	static uint64_t getSize(const char *file);

	static void decompress(const char *input, const char * output, hdt::ProgressListener *listener=NULL);
	static int str_ends_with(const char *str, const char *suffix);
};

class DecompressStream {
private:
	istream *in;
	FILE *filePipe;
	ifstream *fileStream;
#ifdef HAVE_LIBZ
	igzstream *gzStream;
#endif

public:
	DecompressStream(const char *fileName);
	DecompressStream(const char *buffer, size_t len);
	~DecompressStream();
	std::istream *getStream() {
		return in;
	}
	void close();
};

#endif /* FILEUTIL_HPP_ */
