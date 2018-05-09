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

		// Initial values and default constructors
		this->n=n;
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
			saveValue(fp, suff_sample, (n+1)/samplesuff+1);
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
			fm->suff_sample = loadValue<uint>(fp, (fm->n+1)/fm->samplesuff+1);	
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

		for(uint i=0;i<=n;i++)
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
		_bwt = new uint[n+2];
		build_sa();
		for(uint i=0;i<n+1;i++) {
			if(_sa[i]==0) _bwt[i]=0;
			else _bwt[i] = _seq[_sa[i]-1];
		}
		uint j=0;
		uint * sampled_vector = new uint[uint_len(n+2,1)];
		suff_sample = new uint[(n+1)/samplesuff+1];
		for(uint i=0;i<uint_len(n+1,1);i++) sampled_vector[i] = 0;
		for(uint i=0;i<n+1;i++) {
			if(_sa[i]%samplesuff==0) {
				suff_sample[j++]=(uint)_sa[i];
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
		sa_i = suffix->sort(_seq, n);
		_sa = (unsigned long*)sa_i;
		delete suffix;
		assert(_sa[0]==n);
		for(unsigned long i=0;i<n;i++)
			assert(cmp((uint)_sa[i],(uint)_sa[i+1])<=0);
	}

	uint SSA::locate_id(uchar * pattern, uint m) {
		unsigned long i=m-1;
		uint c = pattern[i];
		uint sp = occ[c];
		uint ep = occ[c+1]-1;
		while (sp<=ep && i>=1) {
			c = pattern[--i];
			if(!alphabet[c]){
				return 0;
			}	
			sp = occ[c]+bwt->rank(c,sp-1);
			ep = occ[c]+bwt->rank(c,ep)-1;
		}
		if (sp<=ep) {
			return sp;
		}
		else
			return 0;
	}

	uint SSA::locate(uchar * pattern, uint m, uint32_t **occs){
        uint dummy = 0; // return value is equal to array size in this call
        return this->locate(pattern, m, 0, 0, occs, &dummy);
    }
    
	uint SSA::locate(uchar * pattern, uint m, uint offset, uint limit, uint32_t **occs, uint* num_occ){
		*occs=NULL;
		*num_occ = 0;
		if(!use_sampling)
			return 0;
		unsigned long i=m-1;
		uint c = pattern[i];
		uint sp = occ[c];
		uint ep = occ[c+1]-1;
		while (sp<=ep && i>=1) {
			c = pattern[--i];
			if(!alphabet[c]){
				return 0;
			}
			sp = occ[c]+bwt->rank(c,sp-1);
			ep = occ[c]+bwt->rank(c,ep)-1;
		}
        uint matches = ep-sp+1;
		sp = sp + offset;
        if (limit > 0 && sp + limit - 1 < ep)
            ep = sp + limit - 1;
		if (sp<=ep) {
			*num_occ = ep-sp+1; // ep and sp can have different values by this point
			*occs = new uint[*num_occ];
            uint i = sp;
			uint j,dist;
			size_t rank_tmp;
			while(i<=ep) {
				j = i;
				dist = 0;
				while(!sampled->access(j)) {
					c = bwt->access(j,rank_tmp);
					rank_tmp--;
					j = occ[c]+rank_tmp;
					dist++;
				}
				(*occs)[i-sp] = suff_sample[sampled->rank1(j)-1]+dist;
				i++;
			}
		}
		return matches;
	}


	uint SSA::LF(uint i){
		size_t rank_tmp;
		uint c = bwt->access(i, rank_tmp);
	  //rank_tmp = bwt->rank(c,i);
		return rank_tmp -1 + occ[c];
	}

	uchar * SSA::extract_id(uint id, uint max_len){	
		uchar *res = new uchar[max_len+2];
		uint i = id;
		uint pos = max_len+1;
		res[pos] = '\0';
		pos--;
		uint cont =0;
		size_t rank_tmp;
		uint c = bwt->access(id, rank_tmp);
		//rank_tmp = bwt->rank(c,id);
		
		while(c!= 1){
			res[pos] = (uchar)c;
			pos --;
			cont++;
			i = rank_tmp -1 + occ[c];
			c = bwt->access(i, rank_tmp);
			//rank_tmp = bwt->rank(c,i);
		}
		pos++;
		for(uint j=0; j<cont;j++)
			res[j] = res[pos+j];
		res[cont] = '\0';

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

};
#else
int FMIndexSSADummySymbol;
#endif
