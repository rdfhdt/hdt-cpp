/* huff.cpp
   Copyright (C) 2008, Gonzalo Navarro, all rights reserved.

   Canonical Huffman

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/
// implements canonical Huffman

#if HAVE_CDS

#include "huff.h"
using namespace cds_utils;

namespace URICompressed{

    typedef struct
    {
        uint freq;
        uint symb;
        union
        {
            int prev;
            uint depth;
        } h;
        int ch1,ch2;
    } Ttree;

    static void sort (Ttree *tree, int lo, int up) {
        uint i, j;
        Ttree temp;
        while (up>lo) {
            i = lo;
            j = up;
            temp = tree[lo];
            while (i<j) {
                while (tree[j].freq > temp.freq) j--;
                tree[i] = tree[j];
                while (i<j && tree[i].freq <= temp.freq) i++;
                tree[j] = tree[i];
            }
            tree[i] = temp;
            if (i-lo < up-i) { sort(tree,lo,i-1); lo = i+1; }
            else { sort(tree,i+1,up); up = i-1; }
        }
    }

    static void setdepths (Ttree *tree, uint node, int depth) {
                                 // leaf
        if (tree[node].ch1 == -1) {
            tree[node].h.depth = depth;
            return;
        }
        setdepths (tree,tree[node].ch1,depth+1);
        setdepths (tree,tree[node].ch2,depth+1);
    }

    THuff createHuff (uint *freq, uint lim) {
        THuff H;
        int i,j,d;
        Ttree *tree;
        uint ptr,last,fre;
        // remove zero frequencies
        H.max = lim;
        tree = new Ttree[2*(lim+1)-1];
        j = 0;
        for (i=0;i<=(int)lim;i++) {
            if (freq[i]>0) {
                tree[j].freq = freq[i];
                tree[j].symb = i;
                j++;
            }
        }
        H.lim = lim = j-1;
        // now run Huffman algorithm
        sort (tree,0,lim);
        for (i=0;i<=(int)lim;i++) {
            tree[i].h.prev = i+1;
            tree[i].ch1 = tree[i].ch2 = -1;
        }
        tree[lim].h.prev = -1;
        // last = next node to process, ptr = search point, fre = next free cell
        // leaves are in 0..lim in decreasing freq order
        // internal nodes are in lim+1.. 2*lim, created in incr. fre order
        last=0; ptr = 0; fre = lim+1;
        for (i=0;i<(int)lim;i++) {
            tree[fre].ch1 = last;
            last = tree[last].h.prev;
            tree[fre].ch2 = last;
            tree[fre].freq = tree[tree[fre].ch1].freq+tree[tree[fre].ch2].freq;
            while ((tree[ptr].h.prev != -1) &&
                (tree[tree[ptr].h.prev].freq <= tree[fre].freq))
                ptr = tree[ptr].h.prev;
            tree[fre].h.prev = tree[ptr].h.prev;
            tree[ptr].h.prev = fre;
            last = tree[last].h.prev;
            fre++;
        }
        // now assign depths recursively
        setdepths (tree,2*lim,0);
        H.spos = new uint[H.max+1];
        for (i=0;i<=(int)H.max;i++) H.spos[i] = ~0;
        H.num_enc = new uint[lim+1]; // max possible depth
        d=0;
        for (i=lim;i>=0;i--) {
            H.spos[tree[i].symb] = i;
            while ((int)tree[i].h.depth > d)
                { H.num_enc[d] = i+1; d++; }
        }
        H.num_enc[d] = 0;
        H.depth = d;
        for (d=H.depth;d>0;d--) H.num_enc[d] = H.num_enc[d-1] - H.num_enc[d];
        H.num_enc[0] = (lim == 0);
        uint * Htmp = new uint[H.depth+1];
        for(uint i=0;i<H.depth+1;i++)
            Htmp[i] = 0;
        for(uint i=0;i<H.depth+1;i++)
            Htmp[i] = H.num_enc[i];
        delete [] H.num_enc;
        H.num_enc = Htmp;
        H.total = 0;
        for (i=0;i<=(int)lim;i++)
            H.total += freq[tree[i].symb] * tree[i].h.depth;
        delete [] tree;

				/**for decode**/
				H.symb = new uint[H.lim+1];
				uint aux;
				for(i=0;i<(int)(H.lim+1);i++) 
					H.symb[i] = 0;
				for (i=0;i<=(int)H.max;i++){
					aux = H.spos[i];
					if (aux != (uint)~0) 
						H.symb[aux] = i;
				}
				uint dold, dact;
				H.num_dec = new uint[H.depth+1];
				H.fst = new uint[H.depth+1];
				H.fst[H.depth] = 0; dold = 0;
				for (d=H.depth-1;d>=0;d--) {
					dact = H.num_enc[d+1];
					H.fst[d] = (H.fst[d+1]+dact) >> 1;
					H.num_dec[d+1] = dold;
					dold += dact;
					if(d==0)
						break;
				}
				H.num_dec[0] = dold;
        return H;
    }

    void bitzero (register uint *e, register uint64_t p,
    register uint len) {

        e += p/W; p %= W;
        if (p+len >= W) {
            *e &= ~((1<<p)-1);
            len -= p;
            e++; p = 0;
        }
        while (len >= W) {
            *e++ = 0;
            len -= W;
        }
        if (len > 0)
            *e &= ~(((1<<len)-1)<<p);
    }

    uint64_t encodeHuff (const THuff H, uint symb, uint *stream, uint64_t ptr) {
        uint64_t pos;
        uint64_t code;
        uint64_t d;
        pos = H.spos[symb];
        code = 0;
        d = H.depth;
        while (pos >= H.num_enc[d]) {
            code = (code + H.num_enc[d]) >> 1;
            pos -= H.num_enc[d--];
        }
        code += pos;
        if (d > W) { bitzero(stream,ptr,d-W); ptr += d-W; d = W; }
        while (d--) {
            if ((code >> d) & 1) cds_utils::bitset(stream,ptr);
            else bitclean(stream,ptr);
            ptr++;
        }
        return ptr;
    }

    uint64_t decodeHuff (const THuff H, uint *symb, uint *stream, uint64_t ptr) {
        uint64_t pos;
        uint64_t d;
        pos = 0;
        d = 0;
        while (pos < H.fst[d]) {
            pos = (pos << 1) | bitget(stream,ptr);
            ptr++; d++;
        }
        *symb = H.symb[H.num_dec[d]+pos-H.fst[d]];
        return ptr;
    }

    void saveHuff (const THuff H, ostream & f) {
        saveValue<uint>(f,H.max);
        saveValue<uint>(f,H.lim);
        saveValue<uint>(f,H.depth);
        saveValue<uint>(f,H.symb,H.lim+1);
        saveValue<uint>(f,H.num_enc,H.depth+1);
    }

    uint sizeHuff (const THuff H) {
			/*recalcular*/
			uint mem = sizeof(THuff);
			mem += 4*(H.depth+1)*sizeof(uint); //num_enc+num_dec+H.fst+H.pos
			mem += (H.lim+1)*sizeof(uint);    //H.symb
			return mem;
    }

    void freeHuff (THuff H) {
        delete [] H.spos;
				delete [] H.symb;
        delete [] H.fst;
				delete [] H.num_enc;
				delete [] H.num_dec;
    }

    THuff loadHuff (istream & f) {
    	uint i,dold,dact;
    	THuff H;
    	H.max = loadValue<uint>(f);
    	H.lim = loadValue<uint>(f);
    	H.depth = loadValue<uint>(f);
    	H.symb = loadValue<uint>(f,H.lim+1);
    	H.spos = new uint[H.max+1];
    	for (i=0;i<=H.max;i++)
    		H.spos[i] = (uint)~0;
    	for (i=0;i<=H.lim;i++)
    		H.spos[H.symb[i]] = i;
    	H.num_enc = loadValue<uint>(f,H.depth+1);
    	H.fst = new uint[H.depth+1];
    	H.num_dec = new uint[H.depth+1];
    	H.fst[H.depth] = 0; dold = 0;
    	for (long d=H.depth-1;d>=0;d--) {
    		dact = H.num_enc[d+1];
    		H.fst[d] = (H.fst[d+1]+dact) >> 1;
    		H.num_dec[d+1] = dold;
    		dold += dact;
    		if(d==0)
    			break;
    	}
    	H.num_dec[0] = dold;
    	return H;
    }

};
#else
int HuffmanDummySymbol;
#endif
