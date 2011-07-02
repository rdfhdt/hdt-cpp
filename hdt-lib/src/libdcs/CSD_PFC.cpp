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
	search = true;
}

CSD_PFC::CSD_PFC(char *filename, uint64_t blocksize)
{
	throw "Not implemented";
}

CSD_PFC::CSD_PFC(uchar *dict, uint tlength, uint64_t blocksize)
{
	throw "Not implemented";
}

CSD_PFC::CSD_PFC(IteratorUCharString *it, uint64_t blocksize)
{
	this->type = PFC;
	this->length = 0;
	this->maxlength = 0;
	this->bytes = 0;
	this->blocksize = blocksize;
	this->nblocks = 0;

	build(it);
}

uint64_t
CSD_PFC::locate(const uchar *s, uint64_t len)
{
	// Locating the candidate block for 's'
	uint block;
	bool cmp = locateBlock(s, &block);

//	dumpBlock(block);

	if (cmp)
	{
		// The URI is located at the first position of the block
		return (block*blocksize)+1;
	}
	else
	{
		// The block is sequentially scanned to find the URI
		uint idblock = locateInBlock(block, s, len);

		// If idblock = 0, the URI is not in the dictionary
		if (idblock != 0)
			return (block*blocksize)+idblock+1;
		else
			return 0;
	}
}

void CSD_PFC::dumpAll() {
	for(uint i=0;i<nblocks;i++){
		dumpBlock(i);
	}
}

void CSD_PFC::dumpBlock(uint block) {
	if(block>=nblocks){
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
	while ( (idInBlock<blocksize) && (pos<bytes))  // FIXME: What if last block is not full?
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

uchar*
CSD_PFC::extract(uint64_t id)
{
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

uint
CSD_PFC::decompress(uchar **dict)
{
        return 0;
}

uint64_t
CSD_PFC::getSize()
{
	return bytes*sizeof(uchar)+blocks->getSize()+sizeof(CSD_PFC);
}

void
CSD_PFC::save(ofstream & fp)
{
	saveValue<uint64_t>(fp, type);
	saveValue<uint64_t>(fp, length);
	saveValue<uint64_t>(fp, maxlength);
	saveValue<uint64_t>(fp, bytes);
	saveValue<uchar>(fp, text, bytes);
	saveValue<uint64_t>(fp, blocksize);
	saveValue<uint64_t>(fp, nblocks);
	blocks->save(fp);
}

CSD*
CSD_PFC::load(ifstream & fp)
{
//	uint64_t type = loadValue<uint64_t>(fp);
//	if(type != PFC) return NULL;

	CSD_PFC *dicc = new CSD_PFC();

        dicc->type = PFC;
	dicc->length = loadValue<uint64_t>(fp);
	dicc->maxlength = loadValue<uint64_t>(fp);
	dicc->bytes = loadValue<uint64_t>(fp);
	dicc->text = loadValue<uchar>(fp, dicc->bytes);
	dicc->blocksize = loadValue<uint64_t>(fp);
	dicc->nblocks = loadValue<uint64_t>(fp);
	dicc->blocks = new Array(fp);

	return dicc;
}

void CSD_PFC::build(IteratorUCharString *iterator)
{
	uint64_t tsize = 1024;
	text = (uchar*)malloc(tsize*sizeof(uchar));

	vector<uint> xblocks; // Temporal storage for start positions

	uchar *previousStr, *currentStr = NULL;
	uint previousLength = 0, currentLength = 0;

	while (iterator->hasNext())
	{
		currentStr = iterator->next();
		currentLength = strlen( (char*) currentStr);

		if (currentLength > maxlength) maxlength = currentLength;

		// Checking the current size of the encoded
		// sequence: realloc if necessary
		if ((bytes+currentLength+1) >= tsize)
		{
			if (currentLength < tsize) tsize*=2;
			else tsize+=(2*currentLength);
			text = (uchar*)realloc(text, tsize*sizeof(uchar));
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
			uint delta = lcp(previousStr, currentStr, previousLength, currentLength);

			//cout << "Block: " << nblocks << " Pos: "<< length << endl;
			//cout << previousStr << " vs " << currentStr << " Delta: " << delta << " Difference: " << currentStr + delta << endl;

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
	}

	// Storing the final byte position in the vector of positions
	xblocks.push_back(bytes);

	// Representing the vector of positions with log(bytes) bits
	blocks = new Array(xblocks, bits(bytes));

	search = false;
}

bool
CSD_PFC::locateBlock(const uchar *s, uint *block)
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

	return false;
}

uint
CSD_PFC::locateInBlock(uint block, const uchar *s, uint len)
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

	// Scanning the block until a decission about the existence
	// of 's' can be made.
	while ( (idInBlock<blocksize) && (pos<bytes))
	{

		// Decoding the prefix
		pos += VByte::decode(&delta, text+pos);

		// Copying the suffix
		slen = strlen((char*)text+pos)+1;
		strncpy((char*)(string+delta), (char*)(text+pos), slen);


		if (delta >= cshared)
		{
			// Current delta value means that this string
			// has a larger long common prefix than the
			// previous one
			pshared = cshared;
			cshared += lcp(string+cshared, s+cshared, delta+slen-cshared, len-cshared);

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

void
CSD_PFC::extractInBlock(uint block, uint o, uchar *s)
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

uint
CSD_PFC::lcp(const uchar* str1, const uchar* str2, uint lstr1, uint lstr2)
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

CSD_PFC::~CSD_PFC()
{
	if (search) delete [] text;
	else free(text);

	delete blocks;
}
};
