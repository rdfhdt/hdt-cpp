/* CSD_FMIndex.cpp
 * Copyright (C) 2011, Rodrigo Canovas & Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Abstract class for implementing Compressed String Dictionaries following:
 *
 *   ==========================================================================
 *     "Compressed String Dictionaries"
 *     Nieves R. Brisaboa, Rodrigo Canovas, Francisco Claude, 
 *     Miguel A. Martinez-Prieto and Gonzalo Navarro.
 *     10th Symposium on Experimental Algorithms (SEA'2011), p.136-147, 2011.
 *   ==========================================================================
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
 *   Rodrigo Canovas:  rcanovas@dcc.uchile.cl
 *   Miguel A. Martinez-Prieto:  migumar2@infor.uva.es
 */

#if HAVE_CDS

#include <HDTListener.hpp>

#include "CSD_FMIndex.h"
#include <string.h>
#include <sstream>
#include <vector>
#include <functional>

namespace csd {

CSD_FMIndex::CSD_FMIndex() {
	fm_index = NULL;
	separators = NULL;
}

CSD_FMIndex::CSD_FMIndex(hdt::IteratorUCharString *it, bool sparse_bitsequence, int bparam, size_t bwt_sample, bool use_sample,
		hdt::ProgressListener *listener) {

	this->type = FMINDEX;
	string element;
	unsigned char *text;
	uint *bitmap = 0;
	//uint32_t *bitmap = 0;

	size_t len = 0;
	size_t reservedSize = 1024;
	text = (unsigned char*) malloc(reservedSize * sizeof(unsigned char));
	std::vector < size_t > samplingsPositions;

	text[0] = '\1'; //We suppose that \1 is not part of the text
	maxlength = 0;
	numstrings = 0;
	//uint m_l = 0;

	size_t total = 1;

	unsigned char *currentStr = NULL;
    size_t currentLength = 0;

	while (it->hasNext()) {
		currentStr = it->next();
		//cout << "FMINDEX insert: "<< currentStr << endl;
		if(currentStr[0]!='"') {
			cerr << "Warning: Saving non-literal in an FM-Index";
		}

		numstrings++; //new element

		currentLength = strlen((char*) currentStr);

		if (currentLength > maxlength)
			maxlength = currentLength;

		// Checking the current size of the encoded
		// sequence: realloc if necessary
		if ((total + currentLength + 1) > reservedSize) {
			while (((size_t) total + currentLength + 1) > reservedSize) {
				reservedSize <<= 1;
				if (reservedSize == 0) {
					reservedSize = ((size_t) total + currentLength) * 2;
				}
			}
			text = (unsigned char*) realloc(text, reservedSize * sizeof(unsigned char));
		}
		strncpy((char*)(text+total), (char*)currentStr, currentLength);

		total +=currentLength;

		text[total] = '\1';

		if (use_sample) {
			samplingsPositions.push_back(total);
		}

		it->freeStr(currentStr);
		total++;
	}

	this->tlength = total;
	char *textFinal;
	textFinal = new char[total+ 1];
//	cout<<"testing:total cpy:"<<total<<endl;
//	cout<<"testing:text:"<<text<<endl;
	strncpy((char*)(textFinal), (char*)text, total);
	textFinal[total] = '\0'; //end of the text
//	cout<<"testing:textFinal:"<<textFinal<<endl;

	len = tlength;

	len = total + 1;
	//just one '\0' at the end
	while (textFinal[len - 2] == textFinal[len - 3]) {
		textFinal[len - 2] = '\0';
		len--;
	}

	if (use_sample) {
		 bitmap = new uint[(total + 1 + W) / W];
		 memset((void*)bitmap, 0, 4*((total + 1 + W) / W));
		 cds_utils::bitset(bitmap, 0);
		 for (size_t i=0;i<samplingsPositions.size();i++){
			 cds_utils::bitset(bitmap, samplingsPositions[i]);
		 }
	}
//	cout<<"testing:len:"<<len<<endl;
//	cout<<"testing:textFinal:"<<textFinal<<endl;

	build_ssa((unsigned char *) textFinal, len, sparse_bitsequence, bparam, use_sample,	bwt_sample);
	if (use_sample) {
		//separators = new BitSequenceRRR(bitmap, len);
		separators = new BitSequenceRG(bitmap, len, 4);
		delete[] bitmap;
	}
	delete[] text;

}

void CSD_FMIndex::build_ssa(unsigned char *text, size_t len, bool sparse_bitsequence,
		int bparam, bool use_sample, size_t bwt_sample) {
	use_sampling = use_sample;
	fm_index = new SSA((unsigned char *) text, len, false, use_sample);

	Mapper * am = new MapperNone();
    am->use();
    wt_coder * wc = new wt_coder_huff((unsigned char *) text, len, am);
	BitSequenceBuilder * sbb;
	if (sparse_bitsequence)
		sbb = new BitSequenceBuilderRRR(bparam);
	else
		sbb = new BitSequenceBuilderRG(bparam);
	fm_index->set_static_bitsequence_builder(sbb);

    SequenceBuilder * ssb = new SequenceBuilderWaveletTree(sbb, am, wc);

	fm_index->set_static_sequence_builder(ssb);
	fm_index->set_samplesuff(bwt_sample);
    fm_index->build_index();

    am->unuse();
}

CSD_FMIndex::~CSD_FMIndex() {
	if (fm_index != NULL)
		delete fm_index;
	if (use_sampling)
		if (separators != NULL)
			delete separators;
}

size_t CSD_FMIndex::locate(const unsigned char *s, size_t len) {
	unsigned char *n_s = new unsigned char[len + 2];
	uint o;
	n_s[0] = '\1';
    for (size_t i = 1; i <= len; i++)
		n_s[i] = s[i - 1];
	n_s[len + 1] = '\1';
	o = fm_index->locate_id(n_s, len + 2);
	delete[] n_s;
	if (o != 0)
		return o - 2;
	return 0;
}

size_t CSD_FMIndex::locate_substring(unsigned char *s, size_t len, uint32_t **occs) {
    uint32_t dummy;
    return this->locate_substring(s, len, 0, 0, true, occs, &dummy);
}

size_t CSD_FMIndex::locate_substring(unsigned char *s, size_t len, size_t offset, size_t limit, bool deduplicate, uint32_t **occs, uint32_t* num_occ) {
	if (!use_sampling) {
		*occs = NULL;
		return 0;
	}
	uint matches, i;
    size_t res = 0;
    size_t temp;
	matches = fm_index->locate(s, (uint) len, offset, limit, occs, num_occ);
	if (*num_occ > 0) {
		// TODO: another reason not to combine limit/offset with deduplicate
		if (deduplicate)
			quicksort((*occs), 0, *num_occ - 1);
		i = 1;
		(*occs)[res] = separators->rank1((*occs)[0]);
		// TODO: combining limit/offset and deduplicate will give wrong results
		while (i < *num_occ) {
			temp = separators->rank1((*occs)[i]);
			if (!deduplicate || temp != (*occs)[res]) {
				(*occs)[res + 1] = temp;
				res++;
			}
			i++;
		}
		*num_occ = res + 1;
	}
	return matches;
}

unsigned char * CSD_FMIndex::extract(size_t id) {
	if (id == 0 || id > numstrings)
		return NULL;
	uint i;
	if (id == numstrings)
		i = 2;
	else
		i = id + 3;
	return fm_index->extract_id(i, maxlength);
}

void CSD_FMIndex::freeString(const unsigned char *str) {
	delete [] str;
}

uint CSD_FMIndex::decompress(unsigned char **dict) {
	uint len = 0;
	unsigned char *text = new unsigned char[tlength];
	unsigned char *str;
	uint j;
	for (uint i = 0; i < numstrings; i++) {
		str = extract(i + 1);
		j = 0;
		while (str[j] != '\0') {
			text[len] = str[j];
			len++;
			j++;
		}
		text[len] = '\n';
		len++;
		delete[] str;
	}
	text[len] = '\0';
	len++;
	*dict = text;
	return len;
}

uint64_t CSD_FMIndex::getSize() {
	uint64_t mem = sizeof(CSD_FMIndex);
	mem += fm_index->size();
	if (use_sampling)
		mem += separators->getSize();
	return mem;
}

void CSD_FMIndex::save(ostream &fp) {
	saveValue<unsigned char>(fp, type);
	saveValue<uint32_t>(fp, numstrings);
	saveValue<uint32_t>(fp, tlength);
	saveValue<uint32_t>(fp, maxlength);
	saveValue<bool>(fp, use_sampling);
	if (use_sampling)
		separators->save(fp);
    fm_index->save(fp);

}

struct char_array_buffer : public std::streambuf {
public:
	char_array_buffer(const char *begin, const char *end) :
		begin_(begin), end_(end), current_(begin_) {
        assert(less_equal<const char *>()(begin_, end_));
	};

private:
	streampos seekoff(streamoff off, ios_base::seekdir way, ios_base::openmode which) {
		assert(which == ios_base::in);
		const char* new_pos = 0;
		if (way == ios_base::beg)
			new_pos = begin_ + off;
		else if (way == ios_base::cur)
			new_pos = current_ + off;
		else if (way == ios_base::end)
			new_pos = end_ + off;
		if (new_pos < begin_ || new_pos > end_)
			return -1;
		current_ = new_pos;
		return current_ - begin_;
	};

	streampos seekpos(streampos sp, ios_base::openmode which) {
		return seekoff(sp, ios_base::beg, which);
	};

	int_type underflow() {
		if (current_ == end_)
			return traits_type::eof();

		return traits_type::to_int_type(*current_);
	};

	int_type uflow() {
		if (current_ == end_)
			return traits_type::eof();

		return traits_type::to_int_type(*current_++);
	};

	int_type pbackfail(int_type ch) {
		if (current_ == begin_ || (ch != traits_type::eof() && ch != current_[-1]))
			return traits_type::eof();

		return traits_type::to_int_type(*--current_);
	};

	std::streamsize showmanyc() {
		assert(std::less_equal<const char *>()(current_, end_));
		return end_ - current_;
	}

	// copying not allowed
	char_array_buffer(const char_array_buffer &);
	char_array_buffer &operator= (const char_array_buffer &);

private:
	const char * const begin_;
	const char * const end_;
	const char * current_;
};

size_t CSD_FMIndex::load(unsigned char *ptr, unsigned char *ptrMax) {
    char_array_buffer sbuf((char*)ptr, (char*)ptrMax);
    std::istream localStream(&sbuf);
    localStream.get(); // Load expects the type already read.

    this->type = FMINDEX;
    this->numstrings = loadValue<uint32_t>(localStream);
    this->tlength = loadValue<uint32_t>(localStream);
    this->maxlength = loadValue<uint32_t>(localStream);
    this->use_sampling = loadValue<bool>(localStream);
    if (this->use_sampling)
        this->separators = BitSequence::load(localStream);
    this->fm_index = SSA::load(localStream);

    return localStream.tellg();
}

CSD * CSD_FMIndex::load(istream & fp) {
	CSD_FMIndex *fm = new CSD_FMIndex();

	fm->type = FMINDEX;
	fm->numstrings = loadValue<uint32_t>(fp);
	fm->tlength = loadValue<uint32_t>(fp);
	fm->maxlength = loadValue<uint32_t>(fp);
	fm->use_sampling = loadValue<bool>(fp);
	if (fm->use_sampling)
		fm->separators = BitSequence::load(fp);
	fm->fm_index = SSA::load(fp);

	return fm;
}

size_t CSD_FMIndex::get_pivot(uint32_t *occs, size_t ini, size_t end) {
	size_t izq, der;
	size_t pivot;
	size_t temp;
	pivot = occs[ini];
	izq = ini;
	der = end;
	while (izq < der) {
		while (occs[der] > pivot)
			der--;
		while ((izq < der) && (occs[izq] <= pivot))
			izq++;
		if (izq < der) {
			temp = occs[izq];
			occs[izq] = occs[der];
			occs[der] = temp;
		}
	}
	temp = occs[der];
	occs[der] = occs[ini];
	occs[ini] = temp;
	return der;
}

void CSD_FMIndex::quicksort(uint32_t *occs, size_t ini, size_t end) {
	size_t pivot;
	if (ini < end) {
		pivot = get_pivot(occs, ini, end);
		if (pivot == ini)
			return;
		quicksort(occs, ini, pivot - 1);
		quicksort(occs, pivot + 1, end);
	}
}

void CSD_FMIndex::dumpAll() {
	//FIXME: To be completed
	throw std::logic_error("Not implemented");
}

hdt::IteratorUCharString *csd::CSD_FMIndex::getSuggestions(const char *prefix){
	//FIXME: Not implemented
	throw std::logic_error("Not implemented");
}

hdt::IteratorUInt *csd::CSD_FMIndex::getIDSuggestions(const char *prefix){
	//FIXME: Not implemented
	throw std::logic_error("Not implemented");
}

void csd::CSD_FMIndex::fillSuggestions(const char *base,
        vector<std::string> &out, int maxResults) {
    size_t len = strlen(base);
    unsigned char *n_s = new unsigned char[len + 1];
    //uint o;
    n_s[0] = '\1';
    for (uint32_t i = 1; i <= len; i++)
        n_s[i] = base[i - 1];

    uint32_t *results = NULL;
    size_t numresults = this->locate_substring(n_s, len + 1, &results);
    int maxIter = maxResults;
    if (maxIter >= 0 && numresults < static_cast<size_t>(maxIter))
        maxIter = numresults;
    for (size_t i = 0; i < numresults; i++) {
        out.push_back((char*) (this->extract(results[i])));
    }
}

}
#else
int FMIndexDummySymbol;
#endif
