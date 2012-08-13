/* CSD_HTFC.h
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

#include "CSD_HTFC.h"

namespace csd
{
CSD_HTFC::CSD_HTFC()
{
	this->type = HTFC;
	this->numstrings = 0;
	this->maxlength = 0;
	this->bytes = 0;
	this->blocksize = 0;
	this->nblocks = 0;
	this->text = NULL;
	this->blocks = NULL;
}

CSD_HTFC::CSD_HTFC(hdt::IteratorUCharString *it, uint32_t blocksize, hdt::ProgressListener *listener)
{
	this->type = HTFC;
	this->numstrings = 0;
	this->maxlength = 0;
	this->bytes = 0;
	this->blocksize = blocksize;
	this->nblocks = 0;

	uint64_t reservedSize = 1024;
	uchar *textfc = (uchar*)malloc(reservedSize*sizeof(uchar));
	uint64_t bytesfc = 0;

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
		if ((bytesfc+currentLength+1) > reservedSize)
		{
			while((bytesfc+currentLength+1) > reservedSize) {
				reservedSize *= 2;
				if(reservedSize==0) {
					reservedSize=(bytesfc+currentLength+1)*2;
				}
			}
			textfc = (uchar*)realloc(textfc, reservedSize*sizeof(uchar));
		}

		if ((numstrings % blocksize) == 0)
		{
			// First string in the current block!
			//cout << "First of block: " << nblocks << " => " << currentStr << endl;
			// The current byte is the first one for the
			// current block,
			xblocks.push_back(bytesfc);
			nblocks++;

			// The string is explicitly copied to the
			// encoded sequence.
			strncpy((char*)(textfc+bytesfc), (char*)currentStr, currentLength);
			bytesfc+=currentLength;

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
			bytesfc += VByte::encode(textfc+bytesfc, delta);

			// The suffix is copied to the sequence
			strncpy((char*)(textfc+bytesfc), (char*)currentStr+delta, currentLength-delta);
			bytesfc+=currentLength-delta;
			//cout << nblocks-1 << "," << length << " => " << currentStr << endl;
		}

		textfc[bytesfc] = '\0';
		bytesfc++;

		// New string processed
		numstrings++;
		previousStr = currentStr;
		previousLength = currentLength;

		//NOTIFYCOND(listener, "Converting dictionary to HTFC", length, it->getNumberOfElements());
	}

	// Storing the final byte position in the vector of positions
	xblocks.push_back(bytesfc);

	// Trunc encoded sequence to save unused memory
	textfc = (uchar *) realloc(textfc, bytesfc*sizeof(uchar));

	/********************************
	 * HERE STARTS HuTucker
	 */
	int freqs[256];
	for (uint64_t i=0; i<256; i++) freqs[i] = 1;
	for (uint64_t i=0; i<bytesfc; i++) freqs[(int)(textfc[i])]++;

	HuTucker<int> ht(freqs,256);
	leafs = ht.getCodes(&HTcode, &tree);

	uint64_t tsize = reservedSize/2;
	text = (uchar*)malloc(tsize*sizeof(uchar));
	for (uint64_t i=0; i<tsize; i++) text[i] = 0; // Fixme: Replace for calloc

	// Auxiliar variables for Hu-Tucker encoding
	uint offset = 0, cblocks = 0;
	uint64_t i = 0, slength=0;

	while (i < bytesfc)
	{
		// Checking the current size of the encoded
		// sequence: realloc if necessary
		if ((bytes+maxlength+1) >= tsize)
		{
			while((bytes+maxlength+1) > tsize) {
                                tsize *= 2;
                                if(tsize==0) {
                                        tsize=(bytes+maxlength+1)*2;
                                }
                        }
			text = (uchar*)realloc(text, tsize*sizeof(uchar));

			for (uint64_t j=bytes+1; j<tsize; j++) text[j] = 0;
		}

		if (i == xblocks[cblocks])
		{
			// Starting a new block: 'bytes' and 'offset'
			// are reseted (if offset > 0) to obtain
			// byte-aligned blocks.
			if (offset > 0){ bytes++; offset = 0; }
			xblocks[cblocks] = bytes;
			cblocks++;

			// Encoding the first string
			uchar *first = new uchar[maxlength*2];
			first[0] = 0;
			uint fb = 0, fo = 0; // Variables managing bytes and offsets in the string 'first'

			while (true)
			{
				encodeHT(HTcode[(int)textfc[i]].code, HTcode[(int)textfc[i]].cbits, first, &fb, &fo);
				if (textfc[i] == '\0') break;
				i++;
			}

			if (fo > 0) fb++;

			// Encoding the string length
			bytes += VByte::encode(text+bytes, fb);

			// Copying the encoded string
			// **** strncpy((char*)(text+bytes), (char*)first, fb); POR QUE ESTO NO FUNCIONA SIEMPRE BIEN? :(
			for (uint64_t i=0; i<fb; i++) text[bytes+i] = first[i];
			bytes += fb;

			delete [] first;
		}
		else
		{
			while (true)
			{
				encodeHT(HTcode[(int)textfc[i]].code, HTcode[(int)textfc[i]].cbits, text, (uint*)(&bytes), &offset);
				if (textfc[i] == '\0') break;
				i++;
			}
		}

		i++;
	}

	// Storing the final byte position in the vector of positions
	bytes++;
	xblocks[cblocks] = bytes;
	cblocks++;

	// Representing the vector of positions with log(bytes) bits
	uint bbits = bits(bytes);
	if (bbits == 32) {
		blocks = new Array(xblocks);
	} else {
		blocks = new Array(xblocks, bbits);
	}

	free(textfc);


	// Build tree
	HTtree = new Node[tree->getLength()/2];
	vector<uint> traversing;
	uint node = 0, symbol = 0;

	for (uint i=0; i<tree->getLength(); i++)
	{
		if (tree->getBit(i) == 0)
		{
			HTtree[node].children[0] = -1;
			HTtree[node].children[1] = -1;
			HTtree[node].symbol = 0;

			if (traversing.size() > 0)
			{
				uint parent = traversing[traversing.size()-1];

				if (HTtree[parent].symbol == 0) HTtree[parent].children[0] = node;
				else HTtree[parent].children[1] = node;

				HTtree[parent].symbol++;
			}

			traversing.push_back(node);
			node++;
		}
		else
		{
			uint last = traversing[traversing.size()-1];
			traversing.pop_back();

			if (last == (node-1))
			{
				HTtree[last].symbol = symbol;
				symbol++;
			}
			else
			{
				HTtree[last].symbol = -1;
			}
		}
	}
}

CSD_HTFC::~CSD_HTFC()
{
        if(text)
                free(text);
        if(blocks)
                delete blocks;
}

uint32_t CSD_HTFC::locate(const uchar *s, uint32_t len)
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

void CSD_HTFC::dumpAll() {
	cout << "*****************" << endl;
	for(uint i=0;i<nblocks;i++){
		dumpBlock(i);
	}
	cout << "*****************" << endl;
}

void CSD_HTFC::dumpBlock(uint block) {
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
		pos += VByte::decode(text+pos, &delta);

		// Copying the suffix
		slen = strlen((char*)text+pos)+1;
		strncpy((char*)(string+delta), (char*)(text+pos), slen);

		cout << block*blocksize+idInBlock << " (" << idInBlock << ") => " << string << " Delta=" << delta << " Len="<< slen<< endl;

		pos+=slen;
		idInBlock++;
	}

	delete [] string;
}

uchar* CSD_HTFC::extract(uint32_t id)
{
	if(!text || !blocks) {
		return NULL;
	}

	if ((id > 0) && (id <= numstrings))
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

void CSD_HTFC::freeString(const unsigned char *str) {
	delete [] str;
}

uint64_t CSD_HTFC::getSize()
{
	if(!text || !blocks) {
		return 0;
	}
	return bytes*sizeof(uchar)+blocks->getSize()+sizeof(CSD_HTFC);
}

void CSD_HTFC::save(ofstream & fp)
{
	if(!text || !blocks) {
		return;
	}

	saveValue<uchar>(fp, type);
	saveValue<uint32_t>(fp, numstrings);
	saveValue<uint32_t>(fp, tlength);
	saveValue<uint32_t>(fp, maxlength);
	saveValue<uint64_t>(fp, bytes);
	saveValue<uchar>(fp, text, bytes);
	saveValue<uint32_t>(fp, blocksize);
	saveValue<uint32_t>(fp, nblocks);
	blocks->save(fp);

	// HuTucker:
	tree->save(fp);

	saveValue<uint>(fp, leafs);
	for (uint i=0; i<leafs; i++)
	{
		saveValue<uint>(fp, HTcode[i].code);
		saveValue<uint>(fp, HTcode[i].cbits);
	}
}

CSD* CSD_HTFC::load(ifstream & fp)
{
	CSD_HTFC *dicc = new CSD_HTFC();

	dicc->type = HTFC;  // Type already read by CSD
	dicc->numstrings = loadValue<uint32_t>(fp);
	dicc->tlength = loadValue<uint32_t>(fp);
	dicc->maxlength = loadValue<uint32_t>(fp);
	dicc->bytes = loadValue<uint64_t>(fp);


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

	/* HUTUCKER */

	dicc->tree = new BitString(fp);

	// Building HTtree
	dicc->HTtree = new Node[dicc->tree->getLength()/2];

	vector<uint> traversing;
	uint node = 0, symbol = 0;

	for (uint i=0; i<dicc->tree->getLength(); i++)
	{
		if (dicc->tree->getBit(i) == 0)
		{
			dicc->HTtree[node].children[0] = -1;
			dicc->HTtree[node].children[1] = -1;
			dicc->HTtree[node].symbol = 0;

			if (traversing.size() > 0)
			{
				uint parent = traversing[traversing.size()-1];

				if (dicc->HTtree[parent].symbol == 0) dicc->HTtree[parent].children[0] = node;
				else dicc->HTtree[parent].children[1] = node;

				dicc->HTtree[parent].symbol++;
			}

			traversing.push_back(node);
			node++;
		}
		else
		{
			uint last = traversing[traversing.size()-1];
			traversing.pop_back();

			if (last == (node-1))
			{
				dicc->HTtree[last].symbol = symbol;
				symbol++;
			}
			else
			{
				dicc->HTtree[last].symbol = -1;
			}
		}
	}

	dicc->leafs = loadValue<uint>(fp);
	dicc->HTcode = new Tcode[dicc->leafs];

	for (uint i=0; i<dicc->leafs; i++)
	{
		dicc->HTcode[i].code = loadValue<uint>(fp);
		dicc->HTcode[i].cbits = loadValue<uint>(fp);
	}


	return dicc;
}

bool CSD_HTFC::locateBlock(const uchar *s, uint *block)
{
	uint slen = strlen((char*)s)+1;
	uchar *encoded = new uchar[2*slen];
	encoded[0] = 0;

	// Pattern (s) encoding
	uint encpos = 0, encoffset = 0;
	for (uint i=0; i<slen; i++) encodeHT(HTcode[s[i]].code, HTcode[s[i]].cbits, encoded, &encpos, &encoffset);
	if (encoffset > 0) encpos++;

	long long int l = 0, r = nblocks-1, c;
	uint delta, cmplen;
	int cmp;

	while (l <= r)
	{
		c = (l+r)/2;
		uint pos = blocks->getField(c);

		// Reading the compressed string length
		pos += VByte::decode(text+pos, &delta);

		// The comparison is performed by considering the
		// shortest compressed string
		if (slen < delta) cmplen = slen;
		else cmplen = delta;

		cmp = memcmp((text+pos), encoded, encpos);

		if (cmp > 0)
		{
			// The required string is in any preceding block
			r = c-1;
		}
		else
		{
			if (cmp < 0)
			{
				// The required string is in any subsequent block
				l = c+1;
			}
			else
			{
				if (encpos == delta)
				{
					// The required string is the first one in the c-th block
					*block = c;
					delete [] encoded;
					return true;
				}
				else
				{
					r = c-1;
				}
			}
		}
	}

	// If (cmp < 0) the URI is in the current block (c)
	// If (cmp > 0) the URI is in the preceding block (c-1)
	if (cmp < 0) *block = c;
	else *block = c-1;

	delete [] encoded;
	return false;
}

uint CSD_HTFC::locateInBlock(uint block, const uchar *s, uint len)
{
	if(block>=nblocks){
		return 0;
	}

	uchar *deltaseq = new uchar[DELTA];
	uchar *tmp = new uchar[maxlength];
	uint delta, tmplen;
	uint offset = 0;

	uint pos = blocks->getField(block);
	pos += VByte::decode(text+pos, &delta);
	tmplen = decompressFirstWord(text, &pos, tmp);

	uint plcp_len = 0;
	uint clcp_len = longest_common_prefix(tmp, s, tmplen, len);

	uint read = 1;
	uint id = 0;

	while (read < blocksize && pos<bytes-1)
	{
		//cout << "Pos: " << pos << " Bytes: " << bytes << endl;

		// Decoding the prefix (delta)
		decompressDelta(text, &pos, &offset, deltaseq);
		VByte::decode(deltaseq, &delta);

		if (delta < clcp_len)
		{
			// delta is less than the clcp_len value, so
			// the current string is subsequent to the
			// required one -> it is not in the dictionary!
			id = 0;
			break;
		}
		else
		{
			// Decoding the suffix
			tmplen = decompressWord(text, &pos, &offset, tmp+delta)+delta;

			plcp_len = clcp_len;
			clcp_len += longest_common_prefix(tmp+clcp_len, s+clcp_len, tmplen-clcp_len, len-clcp_len);

			// This is the required string
			if ((tmplen == len) && (clcp_len == len))
			{
				id = read;
				break;
			}

			// The string is not in the dictionary
			if (clcp_len < plcp_len)
			{
				id = 0;
				break;
			}
		}

		read++;
	}

	delete [] tmp;
	delete [] deltaseq;

	return id;
}

void CSD_HTFC::extractInBlock(uint block, uint o, uchar *s)
{
	uchar *deltaseq = new uchar[DELTA];
	uint delta;
	uint offset = 0;

	uint pos = blocks->getField(block);
	pos += VByte::decode(text+pos, &delta);
	delta = decompressFirstWord(text, &pos, s);

	for (uint j=0; j<o; j++)
	{
		// Decoding the prefix (delta)
		decompressDelta(text, &pos, &offset, deltaseq);
		VByte::decode(deltaseq, &delta);

		// Decoding the suffix
		delta += decompressWord(text, &pos, &offset, s+delta);
	}

	s[delta] = '\0';
	delete [] deltaseq;
}

void CSD_HTFC::decompressDelta(uchar *seq, uint *pos, uint *offset, uchar *deltaseq)
{
	uint i = 0;

	do
	{
		deltaseq[i] = (uchar)decodeHT(seq, pos, offset);
		i++;
	}
	while (deltaseq[i-1] < 128);
}

uint CSD_HTFC::decompressFirstWord(uchar *seq, uint *pos, uchar *word)
{
	uint ptr = 0, offset = 0;

	while (true)
	{
		word[ptr] = decodeHT(seq, pos, &offset);
		if (word[ptr] == '\0') break;
		ptr++;
	}

	if (offset > 0) (*pos)++;
	return ptr;
}

uint CSD_HTFC::decompressWord(uchar *seq, uint *pos, uint* offset, uchar *suffix)
{
	uint ptr = 0;

	while (true)
	{
		suffix[ptr] = decodeHT(seq, pos, offset);
		if (suffix[ptr] == '\0') break;

		ptr++;
	}

	return ptr;
}

uchar CSD_HTFC::decodeHT(uchar *seq, uint *pos, uint *offset)
{
	// REVISAR: OTRA IMPLEMENTACION QUE HAGA LOS DESPLAZAMIENTOS
	// DE UNO EN UNO CONSIDERANDO UNA ESTRUCTURA TEMPORAL DONDE
	// COPIE DESDE LA POSICIN DE INICIO
	uint node = 0;

	while (HTtree[node].symbol < 0)
	{
		bool bit = ((seq[*pos] >> (7-(*offset))) & 1);
		node = HTtree[node].children[bit];

		(*offset)++;

		if ((*offset) == 8)
		{
			(*pos)++;
			(*offset) = 0;
		}
	}

	return (uchar)HTtree[node].symbol;
}

void CSD_HTFC::encodeHT(uint code, uint len, uchar *seq, uint *pos, uint *offset)
{
	uchar uccode;
	uint uicode;
	uint processed = 0;

	while ((len-processed) >= (8-(*offset)))
	{
		// "Saco fuera" los bits ya procesados en 'code'.
		uicode = code << (W-len+processed);
		// Me quedo con los que quiero
		uccode = (uchar)(uicode >> (W-(8-(*offset))));
		// Los aado en la posicin actual
		seq[*pos] = seq[*pos] | uccode;

		processed += 8-(*offset);
		(*pos)++;
		seq[*pos] = 0;
		(*offset) = 0;
	}

	if (len-processed > 0)
	{
		uicode = code << (W-len+processed);
		uccode = (uchar)(uicode >> (W-(8-(*offset))));
		seq[*pos] = seq[*pos] | uccode;
		(*offset) += len-processed;
	}
}

uint CSD_HTFC::longest_common_prefix(const uchar* str1, const uchar* str2, uint lstr1, uint lstr2)
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

void CSD_HTFC::fillSuggestions(const char *base, vector<std::string> &out, int maxResults)
{

}

}


