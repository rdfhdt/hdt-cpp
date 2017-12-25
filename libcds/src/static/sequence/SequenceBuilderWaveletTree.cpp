/*  SequenceBuilderWaveletTree.cpp
 * Copyright (C) 2010, Francisco Claude, all rights reserved.
 *
 * Francisco Claude <fclaude@cs.uwaterloo.ca>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <SequenceBuilderWaveletTree.h>

namespace cds_static
{

	SequenceBuilderWaveletTree::SequenceBuilderWaveletTree(BitSequenceBuilder * bsb, Mapper * am, wt_coder * wc) {
		this->bsb = bsb;
		this->am = am;
		this->wc = wc;
		bsb->use();
		am->use();
		if(wc!=NULL)
			wc->use();
	}

	SequenceBuilderWaveletTree::~SequenceBuilderWaveletTree() {
		bsb->unuse();
		am->unuse();
		if(wc!=NULL)
			wc->unuse();
	}

	Sequence * SequenceBuilderWaveletTree::build(uint * sequence, size_t len) {
		Sequence * ret;
		if(wc==NULL) {
			wt_coder * wcaux = new wt_coder_huff(sequence,len,am);
			wcaux->use();
			ret = new WaveletTree(sequence, len, wcaux, bsb, am);
			wcaux->unuse();
		}
		else {
			ret = new WaveletTree(sequence, len, wc, bsb, am);
		}
		return ret;
	}

	Sequence * SequenceBuilderWaveletTree::build(const Array & seq) {
		Sequence * ret;
		if(wc==NULL) {
			wt_coder * wcaux = new wt_coder_huff(seq,am);
			wcaux->use();
			ret = new WaveletTree(seq, wcaux, bsb, am);
			wcaux->unuse();
		}
		else {
			ret = new WaveletTree(seq, wc, bsb, am);
		}
		return ret;
	}
};
