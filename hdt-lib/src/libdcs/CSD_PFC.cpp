/* CSD_PFC.h
 * Copyright (C) 2011, Rodrigo Canovas & Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * This class implements a VByte-oriented Front Coding technique for 
 * compression of string dictionaries.
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
 *   Rodrigo Canovas:  rcanovas@dcc.uchile.cl
 *   Miguel A. Martinez-Prieto:  migumar2@infor.uva.es
 */

#include <stdlib.h>

#include "../util/crc8.h"
#include "../util/crc32.h"

#include "CSD_PFC.h"

namespace csd
{
CSD_PFC::CSD_PFC() : isMapped(false)
{
	this->type = PFC;
	this->numstrings = 0;
	this->bytes = 0;
	this->blocksize = 0;
	this->nblocks = 0;
	this->text = NULL;
	this->blocks = NULL;
}

CSD_PFC::CSD_PFC(hdt::IteratorUCharString *it, uint32_t blocksize, hdt::ProgressListener *listener) : isMapped(false)
{
    this->type = PFC;
    this->numstrings = 0;
    this->bytes = 0;
    this->blocksize = blocksize;
    this->nblocks = 0;

    uint64_t reservedSize = 1024;
    text = (unsigned char*)malloc(reservedSize*sizeof(unsigned char));

    // Pointers to the first string of each block.
    blocks = new hdt::LogSequence2(32);

    unsigned char *currentStr = NULL;
    size_t currentLength = 0;
    string previousStr;

    while (it->hasNext())
    {
        currentStr = it->next();
        currentLength = strlen( (char*) currentStr);

        // Realloc size of the buffer if necessary.
        // +1 for string terminator +9 for VByte encoding (worst case)
        if ((bytes+currentLength+10) > reservedSize)
        {
            reservedSize = (bytes+currentLength+10)*2;

            text = (unsigned char*)realloc(text, reservedSize*sizeof(unsigned char));
        }

        if ((numstrings % blocksize) == 0)
        {
            // First string in the current block!
            blocks->push_back(bytes);
            nblocks++;

            // The string is explicitly copied to the encoded sequence.
            strncpy((char*)(text+bytes), (char*)currentStr, currentLength);
            bytes+=currentLength;
        } else {
            // Regular string

            // Calculate the length of the common prefix
            unsigned int delta = longest_common_prefix((unsigned char *)previousStr.c_str(), currentStr, previousStr.length(), currentLength);

            // The prefix is differentially encoded
            bytes += VByte::encode(text+bytes, delta);

            // The suffix is copied to the sequence
            strncpy((char*)(text+bytes), (char*)currentStr+delta, currentLength-delta);
            bytes+=currentLength-delta;
        }

        text[bytes] = '\0';
        bytes++;

        // New string processed
        numstrings++;

        // Save previous
        previousStr.assign((char*)currentStr);

        NOTIFYCOND(listener, "Converting dictionary to PFC", numstrings, it->getNumberOfElements());

        it->freeStr(currentStr);
    }

    // Storing the final byte position in the vector of positions
    blocks->push_back(bytes);

    // Trunc encoded sequence to save unused memory
    text = (unsigned char *) realloc(text, bytes*sizeof(unsigned char));

    blocks->reduceBits();
}

CSD_PFC::~CSD_PFC()
{
	if(!isMapped) {
		if(text)
			free(text);
	}

	if(blocks)
		delete blocks;
}

uint32_t CSD_PFC::locate(const unsigned char *s, uint32_t len)
{
	if(!text || !blocks)
		return 0;

	// Locating the candidate block for 's'
	unsigned int block;
	bool cmp = locateBlock(s, &block);

	//	dumpBlock(block);

	if (cmp) {
		// The URI is located at the first position of the block
		return (block*blocksize)+1;
	} else {
		// The block is sequentially scanned to find the URI
		unsigned int idblock = locateInBlock(block, s, len);

		// If idblock = 0, the URI is not in the dictionary
		if (idblock != 0) {
			return (block*blocksize)+idblock+1;
		} else {
			return 0;
		}
	}
}

unsigned char* CSD_PFC::extract(uint32_t id)
{
	if(!text || !blocks) {
		return NULL;
	}

	if ((id > 0) && (id <= numstrings))
	{
        // Calculate block and offset
		unsigned int block = (id-1)/blocksize;
		unsigned int offset = (id-1)%blocksize;

		unsigned char *s = extractInBlock(block, offset);

		return s;
	}
	else
	{
		return NULL;
	}
}

void CSD_PFC::freeString(const unsigned char *str) {
	delete [] str;
}

uint64_t CSD_PFC::getSize()
{
	if(!text || !blocks) {
		return 0;
	}
	return bytes*sizeof(unsigned char)+blocks->size()+sizeof(CSD_PFC);
}

void CSD_PFC::save(ostream &out)
{
	CRC8 crch;
	CRC32 crcd;
	unsigned char buf[27]; // 9 bytes per VByte (max) * 3 values.

	// Save type
	crch.writeData(out, (unsigned char *)&type, sizeof(type));

	// Save sizes
	uint8_t pos = 0;
	pos += VByte::encode(&buf[pos], numstrings);
	pos += VByte::encode(&buf[pos], bytes);
	pos += VByte::encode(&buf[pos], blocksize);

	crch.writeData(out, buf, pos);
	crch.writeCRC(out);

	// Write block pointers
	if(!blocks) {
		hdt::LogSequence2 log;
		log.save(out);
	} else {
		blocks->save(out);
	}

	// Write packed data
	if(text) {
		crcd.writeData(out, text, bytes);
	} else {
		assert(numstrings==0);
		assert(bytes==0);
	}
	crcd.writeCRC(out);
}

CSD* CSD_PFC::load(istream & fp)
{
	CRC8 crch;
	CRC32 crcd;
	unsigned char buf[27]; // 9 bytes per VByte (max) * 3 values.
	CSD_PFC *dicc = new CSD_PFC();

	// Load variables
	dicc->type = PFC;   // Type already read by CSD
	dicc->numstrings = (uint32_t) VByte::decode(fp);
	dicc->bytes = VByte::decode(fp);
	dicc->blocksize = (uint32_t) VByte::decode(fp);

	// Calculate variables CRC
	crch.update(&dicc->type, sizeof(dicc->type));

	uint8_t pos = 0;
	pos += VByte::encode(&buf[pos], dicc->numstrings);
	pos += VByte::encode(&buf[pos], dicc->bytes);
	pos += VByte::encode(&buf[pos], dicc->blocksize);
	crch.update(buf, pos);

	crc8_t filecrc = crc8_read(fp);
	if(crch.getValue()!=filecrc) {
		throw "Checksum error while reading Plain Front Coding Header.";
	}

	// Load blocks
	dicc->blocks = new hdt::LogSequence2();
	dicc->blocks->load(fp);
	dicc->nblocks = dicc->blocks->getNumberOfElements()-1;

	// Load strings
	if(dicc->bytes && dicc->numstrings) {
		dicc->text = (unsigned char *)malloc(dicc->bytes);
		const unsigned int blocksize = 8192;
		uint64_t counter=0;
		unsigned char *ptr = (unsigned char *)dicc->text;
		while(counter<dicc->bytes && fp.good()) {
			crcd.readData(fp, ptr, dicc->bytes-counter > blocksize ? blocksize : dicc->bytes-counter);

			ptr += fp.gcount();
			counter += fp.gcount();
		}
		if(counter!=dicc->bytes) {
			throw "Could not read all the data section of the Plain Front Coding.";
		}
	} else {
		// Make sure that all is zero.
		dicc->text = NULL;
		dicc->numstrings = 0;
		dicc->bytes = 0;
		dicc->nblocks = 0;
		delete dicc->blocks;
	}

	crc32_t filecrcd = crc32_read(fp);
	if(filecrcd!=crcd.getValue()) {
		throw "Checksum error in the data section of the Plain Front Coding.";
	}

	return dicc;
}

size_t CSD_PFC::load(unsigned char *ptr, unsigned char *ptrMax) {
	size_t count=0;

	// Type
	if(ptr[count++] != PFC)
		throw "Trying to read a CSD_PFC but type does not match";

	count += VByte::decode(&ptr[count], &numstrings);
	count += VByte::decode(&ptr[count], &bytes);
	count += VByte::decode(&ptr[count], &blocksize);

	// CRC
	CRC8 crch;
	crch.update(&ptr[0], count);
	if(crch.getValue()!=ptr[count++])
		throw "CRC Error while reading CSD_PFC Header.";

	// Blocks
    if(blocks) delete blocks;
	blocks = new hdt::LogSequence2();
	count += blocks->load(&ptr[count], ptrMax);

	nblocks = blocks->getNumberOfElements()-1;

	// Read packed data
    if(!isMapped) free(text);
	text = &ptr[count];
	count+=bytes;

	// Ignore data CRC.
	count+=4;

	isMapped=true;

	return count;
}

bool CSD_PFC::locateBlock(const unsigned char *s, unsigned int *block)
{
	if(nblocks==0) {
		return false;
	}

	long long int left = 0, right = nblocks-1, center;
	int cmp;

	while (left <= right)
	{
		center = (left+right)/2;

		// Comparing s and the first string in the c-th block
		cmp = strcmp((char*)(text+blocks->get(center)), (char*)s);

		if (cmp > 0)
		{
			// 's' is in any preceding block
			right = center-1;
		}
		else
		{
			if (cmp < 0)
			{
				// 's' is in any subsequent block
				left = center+1;
			}
			else
			{
				// 's' is the first one in the c-th block
				*block = center;
				return true;
			}
		}
	}

	// If (cmp < 0) -> c is the candidate block for 's'
	// If (cmp > 0) -> c-1 is the candidate block for 's'
	if (cmp < 0)
		*block = center;
	else
		*block = center-1;

	if(*block == (unsigned int)-1) {
		*block = 0;
	}

	return false;
}

unsigned int CSD_PFC::locateInBlock(unsigned int block, const unsigned char *str, unsigned int len)
{
	if(block>=nblocks){
		return 0;
	}

	unsigned int delta = 0;
	unsigned int idInBlock = 0;
	unsigned int commonPrefix = 0;

	unsigned int pos = blocks->get(block);

	// Read the first string
	std::string tmpStr((char*)text+pos);

	pos+=tmpStr.length()+1;
	idInBlock++;

	// Read the rest
	while ( (idInBlock<blocksize) && (pos<bytes))
	{
		// Decode the prefix
		pos += VByte::decode(text+pos, &delta);

		// Copy the suffix
		tmpStr.resize(delta);
		tmpStr.append((char*)(text+pos));

		if (delta >= commonPrefix)
		{
			// Compare tmpString with the searched one, only after commonPrefix characters.
			// (We already knew that commonPrefix was common anyway).
			commonPrefix += longest_common_prefix(
					(unsigned char*)tmpStr.c_str()+commonPrefix,
					str+commonPrefix,
					tmpStr.length()-commonPrefix,
					len-commonPrefix
				);

			// We found it!
			if ((commonPrefix == len) && (tmpStr.length() == len)) {
				return idInBlock;
			}
		} else {
			// The common prefix is even worse than before, not found.
			return 0;
		}

		pos += tmpStr.length()+1-delta;
		idInBlock++;
	}

	// We checked the whole block but did not find it.
	return 0;
}

unsigned char *CSD_PFC::extractInBlock(unsigned int block, unsigned int o)
{
	unsigned int pos = blocks->get(block);
	unsigned int delta = 0;

	// Read the first string
	string tmpStr((char*)(text+pos));
	pos += tmpStr.length()+1;

	for (unsigned int j=0; j<o; j++)
	{
		// Decode the prefix
		pos += VByte::decode(text+pos, &delta);

		// Copy the suffix
		tmpStr.resize(delta);
		tmpStr.append((char*)(text+pos));

		// Go forward the suffix size
        pos += tmpStr.length()-delta+1;
	}

	unsigned char *buf = new unsigned char[tmpStr.length()+1];
	strcpy((char*)buf, tmpStr.c_str());
	return buf;
}

unsigned int CSD_PFC::longest_common_prefix(const unsigned char* str1, const unsigned char* str2, unsigned int lstr1, unsigned int lstr2)
{
	unsigned int delta = 0;
    unsigned int length = lstr1 < lstr2 ? lstr1 : lstr2;

    while ( (delta<length) && (str1[delta] == str2[delta])) {
        delta++;
    }

	return delta;
}


hdt::IteratorUCharString *CSD_PFC::listAll() {
	return new PFCIterator(this);
}

void CSD_PFC::fillSuggestions(const char *base, vector<std::string> &out, int maxResults)
{
	unsigned int block;
	locateBlock((unsigned char *)base, &block);

	if(!text || !blocks || block>=nblocks){
		return;
	}

	string tmpStr;
	unsigned int baselen = strlen(base);
	bool terminate = false;

	while(block<nblocks && !terminate) {
		unsigned int pos = blocks->get(block);

		unsigned int delta = 0;
		unsigned int idInBlock = 0;

		// Read the first string
		tmpStr.clear();
		tmpStr.append((char*)(text+pos));

		unsigned int slen = tmpStr.length()+1;
		pos+=slen;

		int cmp = strncmp(base, tmpStr.c_str(), baselen);
		if(cmp==0) {
			out.push_back(tmpStr);
			if(out.size()>=maxResults) {
				terminate=true;
			}
		} else if(cmp<0) {
			terminate=true;
		}

		idInBlock++;

		// Scanning the block until a decission about the existence of 's' can be made.
		while ( (idInBlock<blocksize) && (pos<bytes) && !terminate)
		{
			// Decode the prefix
			pos += VByte::decode(text+pos, &delta);

			// Guess suffix size
			slen = strlen((char*)text+pos)+1;

			tmpStr.resize(delta);
			tmpStr.append((char*)text+pos);

			int cmp = strncmp(base, tmpStr.c_str(), baselen);
			if(cmp==0) {
				out.push_back(tmpStr);
				if(out.size()>=maxResults) {
					terminate=true;
				}
			} else if(cmp<0) {
				terminate=true;
			}

			pos+=slen;
			idInBlock++;
		}
		block++;
	}
}

}
