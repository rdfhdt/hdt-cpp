/*
 * File: StopWatch.hpp
 * Last modified: $Date: 2011-08-21 05:35:30 +0100 (dom, 21 ago 2011) $
 * Revision: $Revision: 180 $
 * Last modified by: $Author: mario.arias $
 *
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

#include <iostream>
#include <sys/stat.h>
#include "fileUtil.hpp"

#ifdef USE_LIBZ
#include <zlib.h>
#endif

using namespace std;

uint64_t fileUtil::getSize(std::istream &in) {
	long long begin = in.tellg();
	in.seekg(0, std::ios::end);
	long long end = in.tellg();
	in.seekg(begin, std::ios::beg);

	return end-begin;
}

uint64_t fileUtil::getSize(const char *file) {
#ifdef WIN32
    ifstream in(file);
    in.seekg(0, std::ios::end);
    uint64_t size = in.tellg();
    in.close();
    return size;
#else
	struct stat fileStat;
	if(stat(file, &fileStat)==0) {
		return fileStat.st_size;
	}
	return 0;
#endif
}

void fileUtil::decompress(const char *input, const char * output, hdt::ProgressListener *listener) {

#ifdef USE_LIBZ
#ifdef WIN32
#define CHUNK 64*1024
#else
#define CHUNK 1024*1024
#endif

int ret;
    unsigned have;
    z_stream strm;
    unsigned char *inBuffer = new unsigned char[CHUNK];
    unsigned char *outBuffer = new unsigned char[CHUNK];

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_out = CHUNK;
    strm.next_in = Z_NULL;
    strm.next_out = Z_NULL;

    ret = inflateInit2(&strm, 16+MAX_WBITS);
    if (ret != Z_OK)
        throw "Error initializing libz stream";

    ifstream inFile(input, ios::binary);
    ofstream outFile(output, ios::binary);

    uint64_t fileSize = fileUtil::getSize(inFile);
    uint64_t alreadyRead = 0;

    /* decompress until deflate stream ends or end of file */

    do {

	if(!inFile.good()) {
            inFile.close();
            outFile.close();
            delete[] inBuffer;
            delete[] outBuffer;
	    (void)inflateEnd(&strm);
	    throw "Cannot read file and GZIP data hasn't finished.";
	}
        inFile.read((char*)inBuffer, CHUNK);
        strm.avail_in = inFile.gcount();
        strm.next_in = inBuffer;

        alreadyRead += strm.avail_in;

        if (strm.avail_in == 0) {
            break;
	}

        do {
            strm.avail_out = CHUNK;
            strm.next_out = outBuffer;
            ret = inflate(&strm, Z_NO_FLUSH);

            switch (ret) {
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
            inFile.close();
            outFile.close();
	    delete[] inBuffer;
	    delete[] outBuffer;

            cerr << "zlib error code: " << ret << " => "<< strm.msg << endl;
                (void)inflateEnd(&strm);
                throw "Z_MEM_ERROR";
            }

            have = CHUNK - strm.avail_out;

            outFile.write((char*)outBuffer, have);

            NOTIFY(listener, "Decompressing GZIP", alreadyRead, fileSize);

        } while (strm.avail_in != 0);
    } while (ret!=Z_STREAM_END);

    inFile.close();
    outFile.close();

    delete[] inBuffer;
    delete[] outBuffer;

    /* clean up and return */

    (void)inflateEnd(&strm);

    if(ret!=Z_STREAM_END) {
	    throw "The file is damaged or truncated. Could not decompress the expected bytes. ";
    }
#else
	    throw "Compiled without GZIP Support, please decompress the file and try again.";
#endif

}
