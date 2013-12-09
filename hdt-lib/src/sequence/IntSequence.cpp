/*
 * File: IntSequence.cpp
 * Last modified: $Date: 2011-08-21 05:35:30 +0100 (dom, 21 ago 2011) $
 * Revision: $Revision: 180 $
 * Last modified by: $Author: mario.arias $
 *
 * Copyright (C) 2012, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
 * All rights reserved.
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
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#ifdef HAVE_CDS
#include "LogSequence.hpp"
#include "HuffmanSequence.hpp"
#include "WaveletSequence.hpp"
#endif

#include "LogSequence2.hpp"
#include "ArraySequence.hpp"

#include <HDTVocabulary.hpp>

namespace hdt {

IntSequence *IntSequence::getArray(std::string type)
{
	if(type==HDTVocabulary::SEQ_TYPE_INT32) {
		return new ArraySequence();
	} else if(type==HDTVocabulary::SEQ_TYPE_LOG2) {
		return new LogSequence2();
#ifdef HAVE_CDS
	} else if(type==HDTVocabulary::SEQ_TYPE_HUFFMAN) {
		return new HuffmanSequence();
	} else if(type==HDTVocabulary::SEQ_TYPE_WAVELET) {
		return new WaveletSequence();
#endif
	}
	return new LogSequence2();
}

IntSequence *IntSequence::getArray(unsigned char type)
{
	if(type==SEQ_TYPE_INT32) {
		return new ArraySequence();
	} else if(type==SEQ_TYPE_LOG) {
		return new LogSequence2();
#ifdef HAVE_CDS
	} else if(type==SEQ_TYPE_HUFFMAN) {
		return new HuffmanSequence();
	} else if(type==SEQ_TYPE_WAVELET) {
		return new WaveletSequence();
#endif
	}
	return new LogSequence2();
}


IntSequence *IntSequence::getArray(std::istream &input)
{
	return getArray((unsigned char)input.peek());
}


}
