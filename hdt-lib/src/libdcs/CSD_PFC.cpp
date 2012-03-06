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

#include "CSD_PFC.h"

namespace csd
{
CSD_PFC::CSD_PFC()
{
	this->type = PFC;
	this->length = 0;
	this->maxlength = 0;
	this->bytes = 0;
	this->blocksize = 0;
	this->nblocks = 0;
	this->text = NULL;
	this->blocks = NULL;
}

CSD_PFC::CSD_PFC(IteratorUCharString *it, uint32_t blocksize, hdt::ProgressListener *listener)
{
	this->type = PFC;
	this->length = 0;
	this->maxlength = 0;
	this->bytes = 0;
	this->blocksize = blocksize;
	this->nblocks = 0;

	size_t reservedSize = 1024;
	text = (uchar*)malloc(reservedSize*sizeof(uchar));

	vector<uint> xblocks; // Temporal storage for start positions

	uchar *previousStr, *currentStr = NULL;
	uint previousLength = 0, currentLength = 0;

	while (it->hasNext())
	{
		currentStr = it->next();

		currentLength = strlen( (char*) currentStr);

		if (currentLength > maxlength) maxlength = currentLength;

		// Checking the current size of the encoded
		// sequence: realloc if necessary
		if ((bytes+currentLength+1) > reservedSize)
		{
			while(((size_t)bytes+currentLength+1) > reservedSize) {
				reservedSize <<= 1;
				if(reservedSize==0) {
					reservedSize = ((size_t)bytes+currentLength)*2;
				}
			}
			text = (uchar*)realloc(text, reservedSize*sizeof(uchar));
		}

		if ((length % blocksize) == 0)
		{
			// First string in the current block!
			//cout << "First of block: " << nblocks << " => " << currentStr << endl;
			// The current byte is the first one for the
			// current block,
			xblocks.push_back(bytes);
			nblocks++;

			// The string is explicitly copied to the
			// encoded sequence.
			strncpy((char*)(text+bytes), (char*)currentStr, currentLength);
			bytes+=currentLength;

			//cout << nblocks-1 << "," << length << " => " << currentStr << endl;
		}
		else
		{
			// Regular string

			// Calculating the length of the long common prefix
			uint delta = longest_common_prefix(previousStr, currentStr, previousLength, currentLength);

			//cout << "Block: " << nblocks << " Pos: "<< length << endl;
			//cout << previousStr << endl << currentStr << endl << " Delta: " << delta << " Difference: " << currentStr + delta << endl << endl;

			// The prefix is differentially encoded
			bytes += VByte::encode(delta, text+bytes);

			// The suffix is copied to the sequence
			strncpy((char*)(text+bytes), (char*)currentStr+delta, currentLength-delta);
			bytes+=currentLength-delta;
			//cout << nblocks-1 << "," << length << " => " << currentStr << endl;
		}

		text[bytes] = '\0';
		bytes++;

		// New string processed
		length++;
		previousStr = currentStr;
		previousLength = currentLength;

		NOTIFYCOND(listener, "Converting dictionary to PFC", length, it->getNumberOfElements());
	}

	// Storing the final byte position in the vector of positions
	xblocks.push_back(bytes);

	// Trunc encoded sequence to save unused memory
	text = (uchar *) realloc(text, bytes*sizeof(uchar));

	// Representing the vector of positions with log(bytes) bits
	blocks = new Array(xblocks, bits(bytes));
}

CSD_PFC::~CSD_PFC()
{
        if(text)
                free(text);
        if(blocks)
                delete blocks;
}

uint32_t CSD_PFC::locate(const uchar *s, uint32_t len)
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
	uint pos = blocks->getField(block);
	uchar *string = new uchar[maxlength+1];

	uint slen = strlen((char*)text+pos)+1;

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
		//cout << "POS: " << pos << "/" << bytes << " Next block: "<< blocks->getField(block+1)<<endl;

		// Decoding the prefix
		pos += VByte::decode(&delta, text+pos);

		// Copying the suffix
		slen = strlen((char*)text+pos)+1;
		strncpy((char*)(string+delta), (char*)(text+pos), slen);

		cout << block*blocksize+idInBlock << " (" << idInBlock << ") => " << string << " Delta=" << delta << " Len="<< slen<< endl;

		pos+=slen;
		idInBlock++;
	}

	delete [] string;
}

uchar* CSD_PFC::extract(uint32_t id)
{
	if(!text || !blocks) {
		return NULL;
	}

	if ((id > 0) && (id <= length))
	{
		// Allocating memory for the string
		uchar *s = new uchar[maxlength+1];

		// Calculating block and offset
		uint block = (id-1)/blocksize;
		uint offset = (id-1)%blocksize;

		extractInBlock(block, offset, s);

		return s;
	}
	else
	{
		return NULL;
	}
}

uint64_t CSD_PFC::getSize()
{
	if(!text || !blocks) {
		return 0;
	}
	return bytes*sizeof(uchar)+blocks->getSize()+sizeof(CSD_PFC);
}

void CSD_PFC::save(ofstream & fp)
{
	if(!text || !blocks) {
		return;
	}
	saveValue<uint32_t>(fp, type);
	saveValue<uint32_t>(fp, length);
	saveValue<uint32_t>(fp, maxlength);
	saveValue<uint32_t>(fp, bytes);
	saveValue<uchar>(fp, text, bytes);
	saveValue<uint32_t>(fp, blocksize);
	saveValue<uint32_t>(fp, nblocks);
	blocks->save(fp);
}

CSD* CSD_PFC::load(ifstream & fp)
{
	//	uint32_t type = loadValue<uint32_t>(fp);
	//	if(type != PFC) return NULL;

	CSD_PFC *dicc = new CSD_PFC();

	dicc->type = PFC;
	dicc->length = loadValue<uint32_t>(fp);
	dicc->maxlength = loadValue<uint32_t>(fp);
	dicc->bytes = loadValue<uint32_t>(fp);

#ifdef WIN32
	dicc->text = (unsigned char *)malloc(dicc->bytes);
	const unsigned int blocksize = 8192;
	unsigned int counter=0;
	char *ptr = (char *)dicc->text;
	while(counter<dicc->bytes && fp.good()) {
		fp.read(ptr, dicc->bytes-counter > blocksize ? blocksize : dicc->bytes-counter);
		ptr += fp.gcount();
		counter += fp.gcount();
	}
	//cout << "FINAL Read: " << counter << " / " << dicc->bytes << endl;
#else
	dicc->text = (uchar *) malloc(dicc->bytes*sizeof(unsigned char*));
	fp.read((char *)dicc->text, dicc->bytes);
#endif

	dicc->blocksize = loadValue<uint32_t>(fp);
	dicc->nblocks = loadValue<uint32_t>(fp);
	dicc->blocks = new Array(fp);

	return dicc;
}

bool CSD_PFC::locateBlock(const uchar *s, uint *block)
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
		//cout << "Compare: " << (char*)(text+blocks->getField(center)) << " with " << (char*)s << endl;
		cmp = strcmp((char*)(text+blocks->getField(center)), (char*)s);

		char *compared = (char*)(text+blocks->getField(center));

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

uint CSD_PFC::locateInBlock(uint block, const uchar *s, uint len)
{
	if(block>=nblocks){
		return 0;
	}

	uint pos = blocks->getField(block);
	uchar *string = new uchar[maxlength+1];

	uint slen = strlen((char*)text+pos)+1;

	uint delta = 0;
	uint idInBlock = 0;
	uint pshared = 0, cshared = 0;

	// Reading the first string
	strncpy((char*)string, (char*)(text+pos), slen);
	pos+=slen;
	idInBlock++;

	//cout << "First string: " << string << endl;

	// Scanning the block until a decission about the existence
	// of 's' can be made.
	while ( (idInBlock<blocksize) && (pos<bytes))
	{

		// Decoding the prefix
		pos += VByte::decode(&delta, text+pos);

		// Copying the suffix
		slen = strlen((char*)text+pos)+1;
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

void CSD_PFC::extractInBlock(uint block, uint o, uchar *s)
{
	uint pos = blocks->getField(block);
	uint len = strlen((char*)text+pos)+1;

	uint delta = 0;

	// Reading the first string
	strncpy((char*)s, (char*)(text+pos), len);

	for (uint j=0; j<o; j++)
	{
		pos += len;

		// Decoding the prefix
		pos += VByte::decode(&delta, text+pos);
		// Copying the suffix
		len = strlen((char*)text+pos)+1;
		strncpy((char*)(s+delta), (char*)(text+pos), len);
	}
}

uint CSD_PFC::longest_common_prefix(const uchar* str1, const uchar* str2, uint lstr1, uint lstr2)
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
    locateBlock((cds_utils::uchar *)base, &block);

    if(!text || !blocks || block>=nblocks){
            return;
    }

    uchar *string = new uchar[maxlength+1];
    uint baselen = strlen(base);

    bool terminate = false;

    while(block<nblocks && !terminate) {
        uint pos = blocks->getField(block);
        uint slen = strlen((char*)text+pos)+1;
        uint delta = 0;
        uint idInBlock = 0;

        // Reading the first string
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
            //cout << "POS: " << pos << "/" << bytes << " Next block: "<< blocks->getField(block+1)<<endl;

            // Decoding the prefix
            pos += VByte::decode(&delta, text+pos);

            // Copying the suffix
            slen = strlen((char*)text+pos)+1;
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
