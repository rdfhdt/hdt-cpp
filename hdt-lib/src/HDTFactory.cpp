/*
 * HDTFactory.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTVocabulary.hpp>
#include <HDTFactory.hpp>

#include "BasicHDT.hpp"

#include "header/BasicHeader.hpp"
#include "header/PlainHeader.hpp"

#include "dictionary/PlainDictionary.hpp"

#include "triples/TriplesList.hpp"
#include "triples/TripleListDisk.hpp"
#include "triples/PlainTriples.hpp"
#include "triples/CompactTriples.hpp"
#include "triples/BitmapTriples.hpp"
#include "triples/FOQTriples.hpp"


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
	} else if(triplesType==HDTVocabulary::TRIPLES_TYPE_TRIPLESLISTDISK) {
		return new TripleListDisk();
	} else if(triplesType==HDTVocabulary::TRIPLES_TYPE_FOQ) {
		return new FOQTriples();
	}

	throw "Triples Implementation not available";
}

Dictionary *HDTFactory::readDictionary(ControlInformation &controlInformation) {
	if(!controlInformation.getDictionary())
		throw "Trying to get Dictionary from Non-Dictionary section";

	string type = controlInformation.get("codification");

	if(type==HDTVocabulary::DICTIONARY_TYPE_PLAIN) {
		return new PlainDictionary();
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
