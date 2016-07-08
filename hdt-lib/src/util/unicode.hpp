/*
 * unicode.hpp
 *
 *  Created on: 17/07/2012
 *      Author: mck
 */
#include <stdexcept>
#ifndef UNICODE_HPP_
#define UNICODE_HPP_

#include <iostream>

using namespace std;

/**
 * Append the value to the end of the string, as UTF8.
 */
void appendUnicodeUTF8(std::string &str, unsigned int c) {
	size_t size = 0;

	unsigned char output[7];

	/* check for illegal code positions:
	 * U+D800 to U+DFFF (UTF-16 surrogates)
	 * U+FFFE and U+FFFF
	 */
	if((c > 0xD7FF && c < 0xE000) || c == 0xFFFE || c == 0xFFFF) {
		cerr << "Ignoring character, Unicode Range ERROR: " << hex << c << dec << endl;
		//throw std::runtime_error("Unicode Range ERROR");
	}

	if      (c < 0x00000080)
		size = 1;
	else if(c < 0x00000800)
		size = 2;
	else if(c < 0x00010000)
		size = 3;
	else if(c < 0x00200000)
		size = 4;
	else if(c < 0x04000000)
		size = 5;
	else if(c < 0x80000000)
		size = 6;
	else
		throw std::runtime_error("Unicode ERROR");

	switch(size) {
	case 6:
		output[5] = 0x80 | (unsigned char)(c & 0x3F);
		c= c >> 6;
		/* set bit 2 (bits 7,6,5,4,3,2 less 7,6,5,4,3 set below) on last byte */
		c |= 0x4000000; /* 0x10000 = 0x04 << 24 */
		/* FALLTHROUGH */
	case 5:
		output[4] = 0x80 | (unsigned char)(c & 0x3F);
		c= c >> 6;
		/* set bit 3 (bits 7,6,5,4,3 less 7,6,5,4 set below) on last byte */
		c |= 0x200000; /* 0x10000 = 0x08 << 18 */
		/* FALLTHROUGH */
	case 4:
		output[3] = 0x80 | (unsigned char)(c & 0x3F);
		c= c >> 6;
		/* set bit 4 (bits 7,6,5,4 less 7,6,5 set below) on last byte */
		c |= 0x10000; /* 0x10000 = 0x10 << 12 */
		/* FALLTHROUGH */
	case 3:
		output[2] = 0x80 | (unsigned char)(c & 0x3F);
		c= c >> 6;
		/* set bit 5 (bits 7,6,5 less 7,6 set below) on last byte */
		c |= 0x800; /* 0x800 = 0x20 << 6 */
		/* FALLTHROUGH */
	case 2:
		output[1] = 0x80 | (unsigned char)(c & 0x3F);
		c= c >> 6;
		/* set bits 7,6 on last byte */
		c |= 0xc0;
		/* FALLTHROUGH */
	case 1:
		output[0] = (unsigned char)c;
	}

	output[ size ] = '\0';
	str.append((const char *)output);
}

#endif /* UNICODE_HPP_ */
