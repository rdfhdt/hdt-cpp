/* SSA.cpp
 * Copyright (C) 2011, Rodrigo Canovas & Miguel A. Martinez-Prieto
 * all rights reserved.
 * 
 * Abstract class for implementing Compressed String Dictionaries following:
 * 
 *   ==========================================================================
 *    "Compressed String Dictionaries"
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
 * Rodrigo Canovas:  rcanovas@dcc.uchile.cl
 * Miguel A. Martinez-Prieto:  migumar2@infor.uva.es
 */

#ifdef HAVE_CDS

#include <assert.h>

#include "SSA.h"

namespace csd{

	SSA::SSA(uchar *text, uint n, bool free_text, bool use_sampling) {
		assert(n>0);
        
        this->shiftString(text);
        // this is necessary to make sure the BWT sorting works correctly
        // there was a possible bugs if the two last strings were (case-insensitive) equal
        text[n-1] = 255;
        
		// Initial values and default constructors
		this->n=n-1; // don't need \0 delimiter
		this->_seq = text;
		this->built = false;
		this->free_text=free_text;
		this->use_sampling = use_sampling;
		_sbb = new BitSequenceBuilderRG(20);
		_sbb->use();
		_ssb = new SequenceBuilderWaveletTreeNoptrs(_sbb,new MapperNone());
		_ssb->use();

		// Default sampling values
		samplesuff = 64;

		// Structures that will be built after calling build_index()
		_sa = NULL;
		bwt = NULL;
		_bwt = NULL;
		sampled = NULL;
		suff_sample = NULL;
		alphabet = new bool[256];
		for(size_t i=0; i<256; i++)
			alphabet[i]=false;
	}


	SSA::~SSA() {
		if(_seq!=NULL && free_text)
			delete [] _seq;
		if(_bwt!=NULL)
			delete [] _bwt;
		if(_ssb!=NULL)
			delete _ssb;
		if(bwt!=NULL)
			delete bwt;
		if(_sa!=NULL)
			delete [] _sa;
		if(_sbb!=NULL)
			delete _sbb;
		if(suff_sample!=NULL)
			delete [] suff_sample;
		if(sampled!=NULL)
			delete sampled;
		if(alphabet!=NULL)
			delete [] alphabet;
		if(occ != NULL)
			delete [] occ;
	}


    void SSA::save(ostream &fp) {
		saveValue(fp, n);
		saveValue(fp, maxV);
		saveValue(fp, occ, maxV+1);
		bwt->save(fp);
		saveValue(fp, use_sampling);
		if(use_sampling){
			saveValue(fp, samplesuff);
			saveValue(fp, suff_sample, (n)/samplesuff+1);
			sampled->save(fp);
		}
		saveValue(fp, alphabet, 256);
	}

    SSA * SSA::load(istream & fp){
		SSA *fm = new SSA();
		fm->n = loadValue<uint>(fp);
		fm->maxV = loadValue<uint>(fp);
		fm->occ = loadValue<uint>(fp, fm->maxV+1);
		fm->bwt = Sequence::load(fp); 
		fm->use_sampling = loadValue<bool>(fp);
		if(fm->use_sampling){
			fm->samplesuff = loadValue<uint>(fp);
			fm->suff_sample = loadValue<uint>(fp, (fm->n)/fm->samplesuff+1);	
			fm->sampled = BitSequence::load(fp);
		}
		fm->alphabet = loadValue<bool>(fp,256);
		fm->free_text = false;
		fm->built = true;
		return fm;
	}

	uint SSA::length() {
		return n;
	}


	SSA::SSA() {
		_sa = NULL;
		bwt = NULL;
		_bwt = NULL;
		sampled = NULL;
		suff_sample = NULL;

		_seq=NULL;
		_ssb=NULL;
		_sbb=NULL;
	}


	uint SSA::size() {
		uint size = bwt->getSize();
		if(use_sampling){
			size += sizeof(uint)*(1+n/samplesuff);
			size += sampled->getSize();
		}
		size += sizeof(bool)*(256);
		size += sizeof(SSA);
		size += (1+maxV)*sizeof(uint);
		return size;
	}


	void SSA::print_stats() {
		cout << "ssa stats:" << endl;
		cout << "****************" << endl;
		cout << "Total space  : " << size() << endl;
		cout << endl;
		cout << " bwt         : " << bwt->getSize() << endl;
		if(use_sampling){
			cout << " suff sample : " << sizeof(uint)*(1+n/samplesuff) << endl;
			cout << " sampled: " << sampled->getSize() << endl;
		}
		cout << " occ         : " << (maxV+1)*sizeof(uint) << endl;
		cout << endl;
	}


	bool SSA::set_static_sequence_builder(SequenceBuilder *ssb) {
		if(built) return false;
		ssb->use();
		if(_ssb!=NULL) _ssb->unuse();
		_ssb = ssb;
		return true;
	}


	bool SSA::set_static_bitsequence_builder(BitSequenceBuilder * sbb) {
		if(built) return false;
		sbb->use();
		if(_sbb!=NULL) _sbb->unuse();
		_sbb=sbb;
		return true;
	}

	
	bool SSA::set_samplesuff(uint sample) {
		if(built) return false;
		samplesuff = sample;
		return true;
	}


	bool SSA::build_index() {
		built = true;
		assert(_seq!=NULL);
		assert(_ssb!=NULL);
		if(bwt!=NULL) {
			delete bwt;
			bwt = NULL;
		}
		build_bwt();
		if(free_text) {
			delete [] _seq;
			_seq = NULL;
		}
		bwt = (_ssb->build(_bwt,n+1));

		maxV = 0;
		for(uint i=0;i<n+1;i++){
			alphabet[_bwt[i]]=true;
			maxV = max(_bwt[i],maxV);
		}
		maxV++;

		//cout << " Max value: " << maxV << endl;
		occ = new uint[maxV+1];
		for(uint i=0;i<maxV+1;i++)
			occ[i]=0;

		for(uint i=0;i<n+1;i++)
			occ[_bwt[i]+1]++;
        
		for(uint i=1;i<=maxV;i++)
			occ[i] += occ[i-1];

		delete [] _bwt;
		_bwt = NULL;
		_ssb->unuse();
		_ssb = NULL;
		_sbb->unuse();
		_sbb = NULL;
		if(!use_sampling){
			delete [] suff_sample;
			suff_sample = NULL;
			delete sampled;
			sampled = NULL;
		}
		return true;
	}


	void SSA::build_bwt() {
		assert(_seq!=NULL);
		assert(_sbb!=NULL);
		if(_bwt!=NULL)
			delete [] _bwt;
		_bwt = new uint[n+1];
		build_sa();
		for(uint i=0;i<n+1;i++) {
            // _sa first index refers to \0 added by sa algorithm
			if(_sa[i+1]==0) _bwt[i]=255; // loop around
			else _bwt[i] = _seq[_sa[i+1]-1];
		}
		uint j=0;
		uint * sampled_vector = new uint[uint_len(n+1,1)];
		suff_sample = new uint[(n+1)/samplesuff+1];
		for(uint i=0;i<uint_len(n+1,1);i++) sampled_vector[i] = 0;
		for(uint i=0;i<n+1;i++) {
			if(_sa[i+1]%samplesuff==0) {
				suff_sample[j++]=(uint)_sa[i+1];
				cds_utils::bitset(sampled_vector,i);
			}
		}
		cds_utils::bitset(sampled_vector,n+1);
		sampled = _sbb->build(sampled_vector,n+1);
		delete [] sampled_vector;
		//delete [] _sa;
		free (_sa);
		_sa = NULL;
	}


	void SSA::build_sa() {
		long *sa_i;
		assert(_seq!=NULL);
		if(_sa!=NULL)
			delete [] _sa;
		SuffixArray *suffix = new SuffixArray();
		sa_i = suffix->sort(_seq, n+1);
		_sa = (unsigned long*)sa_i;
		delete suffix;
		//assert(_sa[0]==n);
		//for(unsigned long i=0;i<n;i++)
			//assert(cmp((uint)_sa[i],(uint)_sa[i+1])<=0);
	}
    
    void SSA::charToUpperLower(uint c, uint* cp, uint* Cp) {
        if (c >= 'A' && c < 'A' + 2*26) {
            *Cp = (c-'A')/2*2 + 'A';
            *cp = *Cp+1;
        } else {
            *cp = c;
            *Cp = 0;
        }
    }
    
    void SSA::locateStringPointers(uchar* pattern, uint m, uint* spp, uint* epp) {
        unsigned long i=m-1;
        uint c, C = 0;
        this->charToUpperLower(this->shiftChar(pattern[i]), &c, &C);
        *spp = C ? occ[C] : occ[c];
        *epp = occ[c+1]-1;
        while (*spp<=*epp && i>=1) {
            this->charToUpperLower(this->shiftChar(pattern[--i]), &c, &C);
            if(!alphabet[c] && !(C && alphabet[C])) {
                *spp = 1;
                *epp = 0;
                return;
            }
            *spp = (C ? occ[C] : occ[c]) + (alphabet[c] ? bwt->rank(c,*spp-1) : 0) + (C && alphabet[C] ? bwt->rank(C,*spp-1) : 0);
            *epp = (C ? occ[C] : occ[c]) + (alphabet[c] ? bwt->rank(c,*epp) : 0)   + (C && alphabet[C] ? bwt->rank(C,*epp) : 0) - 1;
        }
    }
    
    uint SSA::locate_id(uchar * pattern, uint m) {
        return this->locate_id(pattern, m, NULL);
    }

    uint SSA::locate_id(uchar * pattern, uint m, uint* epp) {
        uint sp, ep;
        this->locateStringPointers(pattern, m, &sp, &ep);
        if (epp)
            *epp = ep;
        if (sp<=ep)
            return sp;
        else
            return UINT_MAX;
    }
    
    uint SSA::locate(uchar * pattern, uint m, uint32_t **occs){
        if(!use_sampling){
            *occs = NULL;
            return UINT_MAX;
        }
        uint sp, ep;
        this->locateStringPointers(pattern, m, &sp, &ep);
        if (sp<=ep) {
            uint matches = ep-sp+1;
            *occs = new uint[matches];
            uint i = sp;
            uint j,dist;
            size_t rank_tmp;
            while(i<=ep) {
                j = i;
                dist = 0;
                while(!sampled->access(j)) {
                    uint c, C;
                    // TODO: use second access parameter
                    this->charToUpperLower(bwt->access(j), &c, &C);
                    j = (C ? occ[C] : occ[c]) + (alphabet[c] ? bwt->rank(c, j-1) : 0) + (C && alphabet[C] ? bwt->rank(C, j-1) : 0);
                    dist++;
                }
                (*occs)[i-sp] = suff_sample[sampled->rank1(j)-1]+dist;
                i++;
            }
            return ep-sp+1;
        }
        *occs=NULL;
        return UINT_MAX;
    }


	uint SSA::LF(uint i){
		size_t rank_tmp;
		uint c = bwt->access(i, rank_tmp);
	  //rank_tmp = bwt->rank(c,i);
		return rank_tmp -1 + occ[c];
	}
    
    uint SSA::select(uchar c, uint i) {
        return this->bwt->select(c, i);
    }

	uchar * SSA::extract_id(uint id, uint max_len){	
		uchar *res = new uchar[max_len+2];
		uint i = id;
		uint pos = max_len+1;
		res[pos] = '\0';
		pos--;
		uint cont = 0;
        uint c, C;
        c = bwt->access(id);
		//rank_tmp = bwt->rank(c,id);
		
		while(c!= 1){
			res[pos] = (uchar)c;
			pos --;
			cont++;
            this->charToUpperLower(c, &c, &C);
            i = (C ? occ[C] : occ[c]) + (alphabet[c] ? bwt->rank(c, i-1) : 0) + (C && alphabet[C] ? bwt->rank(C, i-1) : 0);
            //i = occ[c] + bwt->rank(c, i-1);
			c = bwt->access(i);
			//rank_tmp = bwt->rank(c,i);
		}
		pos++;
		for(uint j=0; j<cont;j++)
			res[j] = res[pos+j];
		res[cont] = '\0';
        
        this->unshiftString(res);

		return res;
	}

	int SSA::cmp(uint i, uint j) {
		while(i<n && j<n) {
			if(_seq[i]!=_seq[j])
				return (int)_seq[i]-_seq[j];
			i++; j++;
		}
		assert(i!=j);
		if(j<i) return -1;
		return 1;
	}
    
    uchar SSA::shiftChar(uchar c) {
        if (c >= 'A' && c <= 'Z')
            return c + (c-'A');
        if (c >= 'a' && c <= 'z')
            return c - 'a' + 'A' + 1 + (c-'a');
        if (c > 'Z' && c < 'a')
            return c + 26;
        return c;
    }
    uchar SSA::unshiftChar(uchar c) {
        if (c >= 'A' && c < 'A'+2*26) {
            if ((c - 'A') % 2 == 0)
                return c - (c-'A')/2;
            else
                return c - 1 - (c-'A')/2 + 'a' - 'A';
        }
        if (c <= 'z' && c >= 'A'+2*26)
            return c - 26;
        return c;
    }
            
    void SSA::shiftString(uchar* str) {
        int i = 0;
        while (str[i])
        {
            char c = str[i];
            str[i] = this->shiftChar(c);
            i++;
        }
    }
    void SSA::unshiftString(uchar* str) {
        int i = 0;
        while (str[i])
        {
            char c = str[i];
            str[i] = this->unshiftChar(c);
            i++;
        }
    }
};
#else
int FMIndexSSADummySymbol;
#endif
