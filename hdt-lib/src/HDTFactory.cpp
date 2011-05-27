/*
 * HDTFactory.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include "BasicHDT.hpp"
#include <HDTFactory.hpp>

#include "dictionary/PlainDictionary.hpp"

#include "triples/TriplesList.hpp"
#include "triples/TripleListDisk.hpp"
#include "triples/PlainTriples.hpp"
#include "triples/CompactTriples.hpp"
#include "triples/BitmapTriples.hpp"


using namespace hdt;


namespace hdt {

HDT *HDTFactory::createDefaultHDT()
{
	BasicHDT *h = new BasicHDT();
	return h;
}

HDT *HDTFactory::createBasicHDT(HDTSpecification &spec)
{
	BasicHDT *h = new BasicHDT(spec);
	return h;
}

ModifiableHDT *HDTFactory::createModifiableHDT(HDTSpecification &spec)
{
	BasicModifiableHDT *h = new BasicModifiableHDT(spec);
	return h;
}

Triples *HDTFactory::readTriples(ControlInformation &controlInformation) {

#if 1
	if(!controlInformation.getTriples())
		throw "Trying to get Triples from Non-Triples section";
#endif

	std::string triplesType = controlInformation.get("codification");

	/// FIXME: SELECT

	if(triplesType=="http://purl.org/HDT/hdt#triplesBitmap") {
		return new BitmapTriples();
	} else if(triplesType=="http://purl.org/HDT/hdt#triplesCompact") {
		return new CompactTriples();
	} else if(triplesType=="http://purl.org/HDT/hdt#triplesPlain") {
		return new PlainTriples();
	} else if(triplesType=="http://purl.org/HDT/hdt#triplesList") {
		return new TriplesList();
	} else if(triplesType=="http://purl.org/HDT/hdt#triplesListDisk") {
		return new TripleListDisk();
	}

	throw "Triples Implementation not available";
}

Dictionary *HDTFactory::readDictionary(ControlInformation &controlInformation) {

#if 1
	if(!controlInformation.getDictionary())
		throw "Trying to get Dictionary from Non-Dictionary section";
#endif

	string type = controlInformation.get("codification");

	if(type=="http://purl.org/HDT/hdt#dictionaryPlain") {
		return new PlainDictionary();
	}
	/// else...
	/// else...

	throw "Dictionary Implementation not available";
}

}
