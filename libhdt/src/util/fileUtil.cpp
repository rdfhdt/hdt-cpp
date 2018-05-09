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
#include <stdexcept>
#include <iostream>
#include <sys/stat.h>
#include <string.h>
#include "fileUtil.hpp"

#ifdef HAVE_LIBZ
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

#ifdef HAVE_LIBZ
#ifdef WIN32
	const size_t CHUNK=64*1024;
#else
	const size_t CHUNK=1024*1024;
#endif

	ifstream inFile(input, ios::binary);
	if(!inFile.good()) {
		throw std::runtime_error("Could not open input file for decompression");
	}

	ofstream outFile(output, ios::binary);
	if(!outFile.good()) {
		throw std::runtime_error("Could not open output file to save decompressed data");
	}

	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_out = CHUNK;
	strm.next_in = Z_NULL;
	strm.next_out = Z_NULL;

	int ret = inflateInit2(&strm, 16+MAX_WBITS);
	if (ret != Z_OK) {
		inFile.close();
		outFile.close();
		throw std::runtime_error("Error initializing libz stream");
	}

	unsigned char *inBuffer = new unsigned char[CHUNK];
	unsigned char *outBuffer = new unsigned char[CHUNK];

	uint64_t fileSize = fileUtil::getSize(inFile);
	uint64_t alreadyRead = 0;

	// Decompress until deflate stream ends or end of file
	do {
		// Read one block of input data
		if(!inFile.good()) {
			inFile.close();	outFile.close();
			delete[] inBuffer; delete[] outBuffer;
			(void)inflateEnd(&strm);
			throw std::runtime_error("Cannot read file and GZIP data hasn't finished.");
		}
		inFile.read((char*)inBuffer, CHUNK);
		strm.avail_in = inFile.gcount();
		strm.next_in = inBuffer;
		alreadyRead += strm.avail_in;
		if (strm.avail_in == 0) {
			break;
		}

		// Call deflate until it consumes the whole input buffer.
		do {
			strm.avail_out = CHUNK;
			strm.next_out = outBuffer;
			ret = inflate(&strm, Z_NO_FLUSH);

			switch (ret) {
			case Z_NEED_DICT:
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inFile.close(); outFile.close();
				delete[] inBuffer; delete[] outBuffer;

				cerr << "zlib error code: " << ret << " => "<< strm.msg << endl;
				(void)inflateEnd(&strm);
				throw std::runtime_error("Error decompressing gzip file");
			}

			unsigned int have = CHUNK - strm.avail_out;
			outFile.write((char*)outBuffer, have);
			NOTIFY(listener, "Decompressing GZIP", alreadyRead, fileSize);

		} while (strm.avail_in != 0);
	} while (ret!=Z_STREAM_END);

	inFile.close(); outFile.close();
	delete[] inBuffer; delete[] outBuffer;

	(void)inflateEnd(&strm);

	if(ret!=Z_STREAM_END) {
		throw std::runtime_error("The file is damaged or truncated. Could not decompress the expected bytes. ");
	}
#else
	throw std::runtime_error("Compiled without GZIP Support, please decompress the file and try again.");
#endif
}

/*  returns 1 iff str ends with suffix  */
int fileUtil::str_ends_with(const char * str, const char * suffix) {

  if( str == NULL || suffix == NULL )
    return 0;

  size_t str_len = strlen(str);
  size_t suffix_len = strlen(suffix);

  if(suffix_len > str_len)
    return 0;

  return 0 == strncmp( str + str_len - suffix_len, suffix, suffix_len );
}

DecompressStream::DecompressStream(const char *fileName) : in(NULL), filePipe(NULL), fileStream(NULL) {

#ifdef HAVE_LIBZ
	gzStream = NULL;
#endif
	string fn = fileName;
	string suffix = fn.substr(fn.find_last_of(".") + 1);
	string pipeCommand;

#ifdef WIN32
	if( suffix == "gz") {
		#ifdef HAVE_LIBZ
			in = gzStream = new igzstream(fileName);
		#else
			throw std::runtime_error("Support for GZIP was not compiled in this version. Please Decompress the file before importing it.");
		#endif
	}
#else
	if( suffix == "gz") {
		pipeCommand = "gunzip -c ";
	} else if(suffix=="snz") {
		pipeCommand = "snzip -d -c ";
	} else if(suffix=="bz2") {
		pipeCommand = "bunzip2 -c ";
	} else if(suffix=="snz") {
		pipeCommand = "snzip -d -c ";
	}
#endif

#ifndef WIN32
	if(pipeCommand.length()>0) {
		pipeCommand.append(fileName);
		if ((filePipe=popen(pipeCommand.c_str(),"r")) == NULL) {
			cerr << "Error creating pipe for command " << pipeCommand << endl;
			throw std::runtime_error("popen() failed to create pipe");
		}

		in = new boost::fdistream(fileno(filePipe));
	} else if(in==NULL){
		in = fileStream = new ifstream(fileName, ios::binary);
	}

	if (!in->good())
	{
		cerr << "Error opening file " << fileName << " for parsing " << endl;
		throw std::runtime_error("Error opening file for parsing");
	}
#endif
}

void DecompressStream::close() {
	if(fileStream) fileStream->close();
#ifndef WIN32
	if(filePipe) pclose(filePipe);
#endif
#ifdef HAVE_LIBZ
	if(gzStream) gzStream->close();
#endif
	delete in;
	in=NULL;
}
