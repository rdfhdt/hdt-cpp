/*
 * File: BasicHDT.cpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
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
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#include <iomanip>
#include <algorithm>
#include <string>
#include <time.h>

#include <HDTVocabulary.hpp>
#include <HDTFactory.hpp>
#include <RDFParser.hpp>

#include "../util/StopWatch.hpp"
#include "../util/fileUtil.hpp"

#include "ControlInformation.hpp"
#include "BasicHDT.hpp"
#include "../header/PlainHeader.hpp"
#include "../dictionary/PlainDictionary.hpp"
#include "../dictionary/KyotoDictionary.hpp"
#include "../dictionary/PFCDictionary.hpp"
#include "../dictionary/LiteralDictionary.hpp"

#include "../triples/TriplesList.hpp"
#include "../triples/TriplesKyoto.hpp"

#ifndef WIN32
#include "../triples/TripleListDisk.hpp"
#endif
#include "../triples/PlainTriples.hpp"
#include "../triples/CompactTriples.hpp"
#include "../triples/BitmapTriples.hpp"
#include "../triples/TripleOrderConvert.hpp"

#include "TripleIDStringIterator.hpp"

using namespace std;

namespace hdt {


BasicHDT::BasicHDT() {
	createComponents();
}

BasicHDT::BasicHDT(HDTSpecification &spec) {
	this->spec = spec;
	createComponents();
}

BasicHDT::~BasicHDT() {
	deleteComponents();
}

void BasicHDT::createComponents() {
	// HEADER
	header = new PlainHeader();

	// DICTIONARY
	std::string dictType = spec.get("dictionary.type");
	if(dictType==HDTVocabulary::DICTIONARY_TYPE_PFC) {
		dictionary = new PFCDictionary(spec);
	} else if(dictType==HDTVocabulary::DICTIONARY_TYPE_PLAIN) {
		dictionary = new PlainDictionary(spec);
	} else if(dictType==HDTVocabulary::DICTIONARY_TYPE_LITERAL) {
			dictionary = new LiteralDictionary(spec);
	} else {
		dictionary = new PFCDictionary(spec);
	}

	// TRIPLES
	std::string triplesType = spec.get("triples.type");
	if(triplesType==HDTVocabulary::TRIPLES_TYPE_BITMAP) {
		triples = new BitmapTriples(spec);
	} else if(triplesType==HDTVocabulary::TRIPLES_TYPE_COMPACT) {
		triples = new CompactTriples(spec);
	} else if(triplesType==HDTVocabulary::TRIPLES_TYPE_PLAIN) {
		triples = new PlainTriples(spec);
	} else if(triplesType==HDTVocabulary::TRIPLES_TYPE_TRIPLESLIST) {
                triples = new TriplesList(spec);
	} else if (triplesType == HDTVocabulary::TRIPLES_TYPE_TRIPLESLISTDISK) {
		triples = new TripleListDisk();
	} else {
		triples = new BitmapTriples(spec);
	}
}

void BasicHDT::deleteComponents() {
	if (header != NULL)
		delete header;

	if (dictionary != NULL)
		delete dictionary;

	if (triples != NULL)
		delete triples;
}

Header *BasicHDT::getHeader() {
    return header;
}

Dictionary *BasicHDT::getDictionary() {
    return dictionary;
}

Triples *BasicHDT::getTriples() {
    return triples;
}

IteratorTripleString* BasicHDT::search(const char* subject,	const char* predicate, const char* object) {
	TripleString ts(subject, predicate, object);
	try {
		TripleID tid;
		dictionary->tripleStringtoTripleID(ts, tid);
		IteratorTripleID* iterID = triples->search(tid);
		TripleIDStringIterator* iterator = new TripleIDStringIterator(dictionary, iterID);
		return iterator;
	} catch (char* e) {
		cout << "Exception: " << e << endl;
	}
	return new IteratorTripleString();
}


ModifiableDictionary* BasicHDT::getLoadDictionary() {
	return new PlainDictionary(spec);
	//return new KyotoDictionary(spec);
}

ModifiableTriples* BasicHDT::getLoadTriples() {
	return new TriplesList(spec);
	//return new KyotoTriples(spec);
}


void DictionaryLoader::processTriple(hdt::TripleString& triple,	unsigned long long pos) {
	//cout << "Triple String: " << triple << endl;
	dictionary->insert(triple.getSubject(), SUBJECT);
	dictionary->insert(triple.getPredicate(), PREDICATE);
	dictionary->insert(triple.getObject(), OBJECT);
	char str[100];
	if ((listener != NULL) && (count % 100000) == 0) {
		sprintf(str, "Generating Dictionary: %lld K triples processed.", count / 1000);
		listener->notifyProgress(0, str);
	}
	count++;
}

void BasicHDT::loadDictionary(const char* fileName, const char* baseUri, RDFNotation notation, ProgressListener* listener) {

	StopWatch st;
	IntermediateListener iListener(listener);

	// Create temporary dictionary
	ModifiableDictionary *dict = getLoadDictionary();
	dict->startProcessing();

	try {
		NOTIFY(listener, "Loading Dictionary", 0, 100);
		iListener.setRange(0, 80);

		// Load data
		DictionaryLoader dictLoader(dict, &iListener);

		RDFParserCallback *parser = RDFParserCallback::getParserCallback(notation);
		parser->doParse(fileName, baseUri, notation, &dictLoader);
		delete parser;

		iListener.setRange(80, 90);
		dict->stopProcessing(&iListener);

		// Convert to final format
		dictionary->import(dict);

		delete dict;
	} catch (const char *e) {
		cout << "Catch exception dictionary: " << e << endl;
		delete dict;
		throw e;
	} catch (char *e) {
		cout << "Catch exception dictionary: " << e << endl;
		delete dict;
		throw e;
	}
}

void TriplesLoader::processTriple(hdt::TripleString& triple, unsigned long long pos) {
	TripleID ti;
	dictionary->tripleStringtoTripleID(triple, ti);
	if (ti.isValid()) {
		triples->insert(ti);
	} else {
		cerr << "ERROR: Could not convert triple to IDS! " << endl << triple << endl << ti << endl;
		//throw "ERROR, Could not convert triple to ids.";
	}
	//cout << "TripleID: " << ti << endl;
	char str[100];
	if ((listener != NULL) && (count % 100000) == 0) {
		sprintf(str, "Generating Triples: %lld K triples processed.", count / 1000);
		listener->notifyProgress(0, str);
	}
	count++;
}

void BasicHDT::loadTriples(const char* fileName, const char* baseUri, RDFNotation notation, ProgressListener* listener) {
	// Generate Triples
	ModifiableTriples* triplesList = new TriplesList(spec);
	//ModifiableTriples *triplesList = new TriplesKyoto(spec);
	//ModifiableTriples *triplesList = new TripleListDisk();
	StopWatch st;
	IntermediateListener iListener(listener);
	try {
		NOTIFY(listener, "Loading Triples", 0, 100);
		iListener.setRange(0, 60);

		triplesList->startProcessing(&iListener);

		TriplesLoader tripLoader(dictionary, triplesList, &iListener);

		RDFParserCallback *pars = RDFParserCallback::getParserCallback(
				notation);
		pars->doParse(fileName, baseUri, notation, &tripLoader);
		delete pars;

		triplesList->stopProcessing(&iListener);

		// SORT & Duplicates
		TripleComponentOrder order = parseOrder(
				spec.get("triples.component.order").c_str());
		if (order == Unknown) {
			order = SPO;
		}

		iListener.setRange(80, 85);
		triplesList->sort(order, &iListener);

		iListener.setRange(85, 90);
		triplesList->removeDuplicates(&iListener);
	} catch (const char *e) {
		cout << "Catch exception triples" << e << endl;
		delete triplesList;
		throw e;
	} catch (char *e) {
		cout << "Catch exception triples" << e << endl;
		delete triplesList;
		throw e;
	}
	if (triples->getType() == triplesList->getType()) {
		delete triples;
		triples = triplesList;
	} else {
		iListener.setRange(90, 100);
		try {
			triples->load(*triplesList, &iListener);
		} catch (const char* e) {
			delete triplesList;
			throw e;
		}
		delete triplesList;
	}

	//cout << triples->getNumberOfElements() << " triples added in " << st << endl << endl;
}

void BasicHDT::fillHeader(string& baseUri) {
	header->insert(baseUri, HDTVocabulary::RDF_TYPE,
			HDTVocabulary::HDT_DATASET);
	string formatNode = "_:format";
	header->insert(baseUri, HDTVocabulary::HDT_FORMAT_INFORMATION, formatNode);
	string dictNode = "_:dictionary";
	header->insert(formatNode, HDTVocabulary::HDT_DICTIONARY, dictNode);
	string triplesNode = "_:triples";
	header->insert(formatNode, HDTVocabulary::HDT_TRIPLES, triplesNode);
	string statisticsNode = "_:statistics";
	header->insert(baseUri, HDTVocabulary::HDT_STATISTICAL_INFORMATION,
			statisticsNode);
	string publicationInfoNode = "_:publicationInformation";
	header->insert(baseUri, HDTVocabulary::HDT_PUBLICATION_INFORMATION,
			publicationInfoNode);
	dictionary->populateHeader(*header, dictNode);
	triples->populateHeader(*header, triplesNode);
	header->insert(statisticsNode, HDTVocabulary::HDT_SIZE,
            getDictionary()->size() + getTriples()->size());
	// Current time
	time_t now;
	char date[40];
	time(&now);
	struct tm* today = localtime(&now);
	strftime(date, 40, "%Y-%m-%dT%H:%M:%S%z", today);
	header->insert(publicationInfoNode, HDTVocabulary::DUBLIN_CORE_ISSUED, date);
}

void BasicHDT::loadFromRDF(const char *fileName, string baseUri, RDFNotation notation, ProgressListener *listener)
{
	try {
		IntermediateListener iListener(listener);

		iListener.setRange(0,50);
		if(dictionary->getNumberOfElements()==0) {
			loadDictionary(fileName, baseUri.c_str(), notation, &iListener);
		}
		iListener.setRange(50,99);
		loadTriples(fileName, baseUri.c_str(), notation, &iListener);

		fillHeader(baseUri);
		header->insert("_:statistics", HDTVocabulary::ORIGINAL_SIZE, fileUtil::getSize(fileName));

	}catch (const char *e) {
		cout << "Catch exception load: " << e << endl;
		deleteComponents();
		createComponents();
		throw e;
	} catch (char *e) {
		cout << "Catch exception load: " << e << endl;
		deleteComponents();
		createComponents();
		throw e;
	}
}

void BasicHDT::saveToRDF(RDFSerializer &serializer, ProgressListener *listener)
{
	IteratorTripleString *it = search("", "", "");
        try {
            serializer.serialize(it, listener, getTriples()->getNumberOfElements());
        } catch (const char *e) {
            throw e;
        } catch (char *e) {
            throw e;
        }
	delete it;
}

void BasicHDT::loadFromHDT(const char *fileName, ProgressListener *listener) {
	this->fileName = fileName;

	ifstream input(fileName, ios::binary | ios::in);
	if(!input.good()){
		throw "Error opening file to load HDT.";
	}
	this->loadFromHDT(input, listener);
	input.close();
}

void BasicHDT::loadFromHDT(std::istream & input, ProgressListener *listener)
{
    try {
	ControlInformation controlInformation;
	IntermediateListener iListener(listener);

	// Load header
	iListener.setRange(0,5);
	controlInformation.load(input);
	delete header;
	header = NULL;
	header = HDTFactory::readHeader(controlInformation);
	header->load(input, controlInformation, &iListener);

	//Load Dictionary.
	iListener.setRange(5, 60);
	controlInformation.clear();
	controlInformation.load(input);
	delete dictionary;
	dictionary = NULL;
	dictionary = HDTFactory::readDictionary(controlInformation);
	dictionary->load(input, controlInformation, &iListener);

	// Load Triples
	iListener.setRange(60,100);
	controlInformation.clear();
	controlInformation.load(input);
	delete triples;
	triples = NULL;
	triples = HDTFactory::readTriples(controlInformation);
	triples->load(input, controlInformation, &iListener);
    } catch (const char *ex) {
        cout << "Exception loading HDT: " << ex;
        deleteComponents();
        createComponents();
        throw ex;
    } catch (char *ex) {
    	cout << "Exception loading HDT: " << ex;
    	deleteComponents();
        createComponents();
        throw ex;
    }
}

void BasicHDT::saveToHDT(const char *fileName, ProgressListener *listener)
{
    try {
	ofstream out(fileName, ios::binary | ios::out);
	if(!out.good()){
		throw "Error opening file to save HDT.";
	}
	this->saveToHDT(out, listener);
	this->saveIndex(listener);
	out.close();
    } catch (const char *ex) {
        // Fixme: delete file if exists.
        throw ex;
    }

    this->fileName = fileName;
}

void BasicHDT::saveToHDT(std::ostream & output, ProgressListener *listener)
{
	ControlInformation controlInformation;
	IntermediateListener iListener(listener);

	controlInformation.clear();
	controlInformation.setHeader(true);
	iListener.setRange(0,5);
	header->save(output, controlInformation, &iListener);

	controlInformation.clear();
	controlInformation.setDictionary(true);
	iListener.setRange(5,70);
	dictionary->save(output, controlInformation, &iListener);

	controlInformation.clear();
	controlInformation.setTriples(true);
	iListener.setRange(70,100);
	triples->save(output, controlInformation, &iListener);
}

void BasicHDT::generateIndex(ProgressListener *listener) {

	string indexname = this->fileName + ".index";
	ifstream in(indexname.c_str(), ios::binary);

	if(in.good()) {
		ControlInformation ci;
		ci.load(in);
		triples->loadIndex(in, ci, listener);
		in.close();
	} else {
		triples->generateIndex(listener);
		this->saveIndex(listener);
	}
}

void BasicHDT::saveIndex(ProgressListener *listener) {
	if(this->fileName.size()==0) {
		cerr << "Cannot save Index if the HDT is not saved" << endl;
		return;
	}

	string indexname = this->fileName + ".index";
	ofstream out(indexname.c_str(), ios::binary);
	ControlInformation ci;
	triples->saveIndex(out, ci, listener);
	out.close();
}

void BasicHDT::convert(HDTSpecification &spec)
{

}




}
