/*
 * File HDTFactory.cpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * Copyright (C) 2011, Javier D. Fernandez, Miguel A. Martinez-Prieto
 *                     Mario Arias, Alejandro Andres.
 * All rights reserved.
 *
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
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *   Mario Arias:               mario.arias@gmail.com
 *   Alejandro Andres:          fuzzy.alej@gmail.com
 *
 */


#include <HDT.hpp>
#include <HDTVocabulary.hpp>
#include <HDTFactory.hpp>

#include "BasicHDT.hpp"

#include "header/BasicHeader.hpp"
#include "header/PlainHeader.hpp"

#include "dictionary/PlainDictionary.hpp"
#include "dictionary/PFCDictionary.hpp"

#include "triples/TriplesList.hpp"
#ifndef WIN32
#include "triples/TripleListDisk.hpp"
#endif
#include "triples/PlainTriples.hpp"
#include "triples/CompactTriples.hpp"
#include "triples/BitmapTriples.hpp"
#ifdef USE_FOQ
#include "triples/FOQTriples.hpp"
#endif


using namespace hdt;


namespace hdt {

HDT *HDTFactory::createDefaultHDT()
{
	BasicHDT *h = new BasicHDT();
	return h;
}

HDT *HDTFactory::createHDT(HDTSpecification &spec)
{
	BasicHDT *h = new BasicHDT(spec);
	return h;
}

ModifiableHDT *HDTFactory::createDefaultModifiableHDT()
{
	BasicModifiableHDT *h = new BasicModifiableHDT();
	return h;
}

ModifiableHDT *HDTFactory::createModifiableHDT(HDTSpecification &spec)
{
	BasicModifiableHDT *h = new BasicModifiableHDT(spec);
	return h;
}

Triples *HDTFactory::readTriples(ControlInformation &controlInformation) {
	if(!controlInformation.getTriples())
		throw "Trying to get Triples from Non-Triples section";

	std::string triplesType = controlInformation.get("codification");

	if(triplesType==HDTVocabulary::TRIPLES_TYPE_BITMAP) {
		return new BitmapTriples();
	} else if(triplesType==HDTVocabulary::TRIPLES_TYPE_COMPACT) {
		return new CompactTriples();
	} else if(triplesType==HDTVocabulary::TRIPLES_TYPE_PLAIN) {
		return new PlainTriples();
	} else if(triplesType==HDTVocabulary::TRIPLES_TYPE_TRIPLESLIST) {
		return new TriplesList();
#ifndef WIN32
	} else if(triplesType==HDTVocabulary::TRIPLES_TYPE_TRIPLESLISTDISK) {
		return new TripleListDisk();
#endif
#ifdef USE_FOQ
	} else if(triplesType==HDTVocabulary::TRIPLES_TYPE_FOQ) {
		return new FOQTriples();
#endif
	}

	throw "Triples Implementation not available";
}

Dictionary *HDTFactory::readDictionary(ControlInformation &controlInformation) {
	if(!controlInformation.getDictionary())
		throw "Trying to get Dictionary from Non-Dictionary section";

	string type = controlInformation.get("codification");

	if(type==HDTVocabulary::DICTIONARY_TYPE_PLAIN) {
		return new PlainDictionary();
	} else if(type==HDTVocabulary::DICTIONARY_TYPE_PFC) {
		return new PFCDictionary();
	}

	throw "Dictionary Implementation not available";
}

Header *HDTFactory::readHeader(ControlInformation &controlInformation) {
	if(!controlInformation.getHeader())
		throw "Trying to get Header from Non-Header section";

	string type = controlInformation.get("codification");

	if(type==HDTVocabulary::HEADER_PLAIN) {
		return new PlainHeader();
	}

	throw "Header Implementation not available";
}

}
