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
CSD_PFC::CSD_PFC()
{
	this->type = PFC;
	this->numstrings = 0;
	this->bytes = 0;
	this->blocksize = 0;
	this->nblocks = 0;
	this->text = NULL;
	this->blocks = NULL;
}

CSD_PFC::CSD_PFC(hdt::IteratorUCharString *it, uint32_t blocksize, hdt::ProgressListener *listener)
{
	this->type = PFC;
	this->numstrings = 0;
	this->bytes = 0;
	this->blocksize = blocksize;
	this->nblocks = 0;

	uint64_t reservedSize = 1024;
	text = (unsigned char*)malloc(reservedSize*sizeof(unsigned char));

	// Representing the vector of positions with log(bytes) bits
	blocks = new hdt::LogSequence2(32);

	unsigned char *previousStr, *currentStr = NULL;
	size_t previousLength = 0, reservedLength=10, currentLength = 0;

	previousStr = (unsigned char *) malloc(reservedLength);

	while (it->hasNext())
	{
		currentStr = it->next();
		currentLength = strlen( (char*) currentStr);
		//cout << "CSD_PFC insert: " << currentStr << " ("<<currentLength << ")" << endl;

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

			// Calculate the length of the long common prefix
			uint delta = longest_common_prefix(previousStr, currentStr, previousLength, currentLength);

			//cout << "Block: " << nblocks << " Pos: "<< length << endl;
			//cout << previousStr << endl << currentStr << endl << " Delta: " << delta << " Difference: " << currentStr + delta << endl << endl;

			// The prefix is differentially encoded
			bytes += VByte::encode(text+bytes, delta);

			// The suffix is copied to the sequence
			strncpy((char*)(text+bytes), (char*)currentStr+delta, currentLength-delta);
			bytes+=currentLength-delta;
			//cout << nblocks-1 << "," << length << " => " << currentStr << endl;
		}

		text[bytes] = '\0';
		bytes++;

		// New string processed
		numstrings++;

		// Save previous
		if(reservedLength<currentLength) {
			previousStr = (unsigned char *)realloc(previousStr, currentLength);
		}
		memcpy(previousStr, currentStr, currentLength);
		previousLength = currentLength;

		//NOTIFYCOND(listener, "Converting dictionary to PFC", numstrings, it->getNumberOfElements());
		if((numstrings%100000)==0) {
			listener->notifyProgress(numstrings, " Loading to PFC ");
		}
	}

	free(previousStr);

	// Storing the final byte position in the vector of positions
	blocks->push_back(bytes);

	// Trunc encoded sequence to save unused memory
	text = (unsigned char *) realloc(text, bytes*sizeof(unsigned char));

	blocks->reduceBits();
}

CSD_PFC::~CSD_PFC()
{
        if(text)
                free(text);
        if(blocks)
                delete blocks;
}

uint32_t CSD_PFC::locate(const unsigned char *s, uint32_t len)
{
	if(!text || !blocks)
		return 0;

	// Locating the candidate block for 's'
	uint block;
	bool cmp = locateBlock(s, &block);

//	dumpBlock(block);

	if (cmp) {
		// The URI is located at the first position of the block
		return (block*blocksize)+1;
	} else {
		// The block is sequentially scanned to find the URI
		uint idblock = locateInBlock(block, s, len);

		// If idblock = 0, the URI is not in the dictionary
		if (idblock != 0) {
			return (block*blocksize)+idblock+1;
		} else {
			return 0;
		}
	}
}

void CSD_PFC::dumpAll() {
	cout << "*****************" << endl;
	for(uint i=0;i<nblocks;i++){
		dumpBlock(i);
	}
	cout << "*****************" << endl;
}

void CSD_PFC::dumpBlock(uint block) {
	if(!text || !blocks || block>=nblocks){
		return;
	}
	cout << "Dump block: " << block << endl;
	size_t pos = blocks->get(block);

	size_t slen = strlen((char*)text+pos)+1;

	size_t bufLen = slen;
	unsigned char *string = (unsigned char*)malloc(bufLen);

	uint delta = 0;
	uint idInBlock = 0;

	// Reading the first string
	strncpy((char*)string, (char*)(text+pos), slen);
	string[slen] = '\0';
	pos+=slen;

	cout << block*blocksize+idInBlock << " (" << idInBlock << ") => " << string << endl;
	idInBlock++;

	// Scanning the block until a decission about the existence
	// of 's' can be made.
	while ( (idInBlock<blocksize) && (pos<bytes))
	{
		//cout << "POS: " << pos << "/" << bytes << " Next block: "<< blocks->get(block+1)<<endl;

		// Decode the prefix
		pos += VByte::decode(text+pos, &delta);

		// Guess suffix size
		slen = strlen((char*)text+pos)+1;

		// Ensure there is room in the buffer
		if((delta+slen+1)>bufLen) {
		    bufLen = delta+slen+1;
		    string = (unsigned char*)realloc(string, bufLen);
		}

		// Copy the suffix
		strncpy((char*)(string+delta), (char*)(text+pos), slen);

		cout << block*blocksize+idInBlock << " (" << idInBlock << ") => " << string << " Delta=" << delta << " Len="<< slen<< endl;

		pos+=slen;
		idInBlock++;
	}

	delete [] string;
}

unsigned char* CSD_PFC::extract(uint32_t id)
{
	if(!text || !blocks) {
		return NULL;
	}

	if ((id > 0) && (id <= numstrings))
	{
		// Calculating block and offset
		uint block = (id-1)/blocksize;
		uint offset = (id-1)%blocksize;

		unsigned char *s = extractInBlock(block, offset);

		return s;
	}
	else
	{
		return NULL;
	}
}

void CSD_PFC::freeString(const unsigned char *str) {
	free((void*)str);
}

uint64_t CSD_PFC::getSize()
{
	if(!text || !blocks) {
		return 0;
	}
	return bytes*sizeof(unsigned char)+blocks->size()+sizeof(CSD_PFC);
}

void CSD_PFC::save(ofstream &out)
{
	CRC8 crch;
	CRC32 crcd;
	unsigned char buf[27]; // 9 bytes per VByte (max) * 3 values.

	if(!text || !blocks) {
		return;
	}

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
	blocks->save(out);

	// Write packed data
	crcd.writeData(out, text, bytes);
	crcd.writeCRC(out);
}

CSD* CSD_PFC::load(ifstream & fp)
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
	dicc->text = (unsigned char *)malloc(dicc->bytes);
	const unsigned int blocksize = 8192;
	unsigned int counter=0;
	unsigned char *ptr = (unsigned char *)dicc->text;
	while(counter<dicc->bytes && fp.good()) {
		crcd.readData(fp, ptr, dicc->bytes-counter > blocksize ? blocksize : dicc->bytes-counter);

		ptr += fp.gcount();
		counter += fp.gcount();
	}
	if(counter!=dicc->bytes) {
		throw "Could not read all the data section of the Plain Front Coding.";
	}

	crc32_t filecrcd = crc32_read(fp);
	if(filecrcd!=crcd.getValue()) {
		throw "Checksum error in the data section of the Plain Front Coding.";
	}

	return dicc;
}

bool CSD_PFC::locateBlock(const unsigned char *s, uint *block)
{
	if(nblocks==0) {
		return false;
	}
	long long int left = 0, right = nblocks-1, center;
	int cmp;

	//cout << endl << "locateBlock: " << s << endl;

	while (left <= right)
	{
		center = (left+right)/2;
		//cout << "Binary: " << left << "," << center << "," << right << endl;

		// Comparing s and the first string in the c-th block
		//cout << "Compare: " << (char*)(text+blocks->get(center)) << " with " << (char*)s << endl;
		cmp = strcmp((char*)(text+blocks->get(center)), (char*)s);

		char *compared = (char*)(text+blocks->get(center));

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

	//cout << "Found block: " << *block << endl;
        if(*block == (unsigned int)-1) {
            *block = 0;
        }

	return false;
}

uint CSD_PFC::locateInBlock(uint block, const unsigned char *s, uint len)
{
	if(block>=nblocks){
		return 0;
	}

	uint pos = blocks->get(block);
	uint bufLen = 100;
	unsigned char *string = (unsigned char*)malloc(bufLen);

	uint slen = strlen((char*)text+pos)+1;

	uint delta = 0;
	uint idInBlock = 0;
	uint pshared = 0, cshared = 0;

	if(slen>=bufLen) {
	    bufLen = slen;
	    string = (unsigned char*)realloc(string, bufLen);
	}

	// Reading the first string
	strncpy((char*)string, (char*)(text+pos), slen);
	pos+=slen;
	idInBlock++;

	//cout << "First string: " << string << endl;

	// Scanning the block until a decission about the existence
	// of 's' can be made.
	while ( (idInBlock<blocksize) && (pos<bytes))
	{

		// Decode the prefix
		pos += VByte::decode(text+pos, &delta);

		// Guess suffix size
		slen = strlen((char*)text+pos)+1;

		// Make sure there is room in the buffer
		if((delta+slen+1)>bufLen) {
		    bufLen = delta+slen+1;
		    string = (unsigned char*)realloc(string, bufLen);
		}

		// Copy the suffix
		strncpy((char*)(string+delta), (char*)(text+pos), slen);
		//cout << "New string: " << string << endl;

		if (delta >= cshared)
		{
			// Current delta value means that this string
			// has a larger long common prefix than the
			// previous one
			pshared = cshared;
			cshared += longest_common_prefix(string+cshared, s+cshared, delta+slen-cshared, len-cshared);

			// This is the required string
			if ((cshared == len) && ((delta+slen-1) == len)) {
				break;
			}
		}
		else
		{
			idInBlock = 0;
			break;
		}

		pos+=slen;
		idInBlock++;
	}

	if(pos==bytes || idInBlock == blocksize) {
		idInBlock=0;
	}

	delete [] string;
	return idInBlock;
}

unsigned char *CSD_PFC::extractInBlock(uint block, uint o)
{
	uint pos = blocks->get(block);
	uint len = strlen((char*)text+pos)+1;

	uint delta = 0;

	uint bufLen = len+1;
	unsigned char *s = (unsigned char*)malloc(bufLen);

	// Reading the first string
	strncpy((char*)s, (char*)(text+pos), len);

	for (uint j=0; j<o; j++)
	{
		pos += len;

		// Decode the prefix
		pos += VByte::decode(text+pos, &delta);

		// Guess suffix size
		len = strlen((char*)text+pos)+1;

		// Ensure there is room in the buffer
		if((delta+len+1)>=bufLen) {
		    bufLen = delta+len+1;
		    s = (unsigned char*)realloc(s, bufLen);
		}

		// Copy the suffix
		strncpy((char*)(s+delta), (char*)(text+pos), len);
	}

	return s;
}

uint CSD_PFC::longest_common_prefix(const unsigned char* str1, const unsigned char* str2, uint lstr1, uint lstr2)
{
	uint delta = 0;
	uint length = lstr1;
	if (length > lstr2) length = lstr2;

	for (uint i=0; i<length; i++)
	{
		if (str1[i] == str2[i]) delta++;
		else break;
	}

	return delta;
}

}

void csd::CSD_PFC::fillSuggestions(const char *base, vector<std::string> &out, int maxResults)
{
    uint block;
    locateBlock((unsigned char *)base, &block);

    if(!text || !blocks || block>=nblocks){
            return;
    }

    uint bufLen = 100;
    unsigned char *string = (unsigned char*)malloc(bufLen);
    uint baselen = strlen(base);

    bool terminate = false;

    while(block<nblocks && !terminate) {
	uint pos = blocks->get(block);
        uint slen = strlen((char*)text+pos)+1;
        uint delta = 0;
        uint idInBlock = 0;

	// Ensure size in the buffer
	if(bufLen<slen) {
	    string = (unsigned char *)realloc(string, bufLen);
	    bufLen = slen;
	}

	// Read the first string
        strncpy((char*)string, (char*)(text+pos), slen);
        string[slen] = '\0';
        pos+=slen;

        int cmp = strncmp(base, (char *)string, baselen);
        if(cmp==0) {
            out.push_back((char *)string);
            if(out.size()>=maxResults) {
                terminate=true;
            }
        } else if(cmp<0) {
            terminate=true;
        }

        idInBlock++;

        // Scanning the block until a decission about the existence
        // of 's' can be made.
        while ( (idInBlock<blocksize) && (pos<bytes) && !terminate)
        {
	    //cout << "POS: " << pos << "/" << bytes << " Next block: "<< blocks->get(block+1)<<endl;

	    // Decode the prefix
            pos += VByte::decode(text+pos, &delta);

	    // Guess suffix size
	    slen = strlen((char*)text+pos)+1;

	    // Ensure there is size in the buffer.
	    if((delta+slen+1)>=bufLen) {
		bufLen = delta+slen+1;
		string = (unsigned char*)realloc(string, bufLen);
	    }

	    // Copy the suffix
            strncpy((char*)(string+delta), (char*)(text+pos), slen);

            int cmp = strncmp(base, (char *)string, baselen);
            if(cmp==0) {
                out.push_back((char *)string);
                if(out.size()>=maxResults) {
                    terminate=true;
                }
            } else if(cmp<0) {
                terminate=true;
            }

            //cout << block*blocksize+idInBlock << " (" << idInBlock << ") => " << string << " Delta=" << delta << " Len="<< slen<< endl;

            pos+=slen;
            idInBlock++;
        }
        block++;
    }

    delete [] string;
}
