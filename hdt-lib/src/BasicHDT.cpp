/*
 * File BasicHDT.cpp
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

#include <string>

#include <HDTVocabulary.hpp>
#include <HDTFactory.hpp>

#include "BasicHDT.hpp"
#include "header/BasicHeader.hpp"
#include "header/EmptyHeader.hpp"
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
#include "triples/TripleOrderConvert.hpp"

#include "ControlInformation.hpp"
#include "util/StopWatch.hpp"

#include "rdf/RDFParserRaptorCallback.hpp"

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
	delete header;
	delete dictionary;
	delete triples;
}

void BasicHDT::createComponents() {
	// HEADER
#if 0
	if(spec.get("noheader")=="true") {
		header = new EmptyHeader();
	} else {
		header = new BasicHeader(spec);
	}
#else
	header = new PlainHeader();
#endif

	// DICTIONARY
	std::string dictType = spec.get("dictionary.type");
	if(dictType==HDTVocabulary::DICTIONARY_TYPE_PFC) {
		dictionary = new PFCDictionary(spec);
	} else if(dictType==HDTVocabulary::DICTIONARY_TYPE_PLAIN) {
		dictionary = new PlainDictionary(spec);
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
#ifndef WIN32
	} else if(triplesType==HDTVocabulary::TRIPLES_TYPE_TRIPLESLISTDISK) {
		triples = new TripleListDisk();
#endif
#ifdef USE_FOQ
	} else if(triplesType==HDTVocabulary::TRIPLES_TYPE_FOQ) {
		triples = new FOQTriples();
#endif
	} else {
		triples = new BitmapTriples(spec);
	}
}



Header & BasicHDT::getHeader()
{
	return *header;
}

Dictionary & BasicHDT::getDictionary()
{
	return *dictionary;
}

Triples & BasicHDT::getTriples()
{
	return *triples;
}


IteratorTripleString *BasicHDT::search(const char *subject, const char *predicate, const char *object)
{
	TripleString ts(subject, predicate, object);

	try {
		TripleID tid;
		dictionary->tripleStringtoTripleID(ts, tid);

		//cout << "TID: "<< tid.getSubject() << "," << tid.getPredicate() << "," << tid.getObject() << endl;
		IteratorTripleID *iterID = triples->search(tid);

		BasicIteratorTripleString *iterator = new BasicIteratorTripleString(dictionary, iterID);
		return iterator;
	} catch (char *e){
		cout << "Exception: " << e << endl;
		return new IteratorTripleString();
	}
}

void DictionaryLoader::processTriple(hdt::TripleString &triple, unsigned long long pos)
{
	dictionary->insert(triple.getSubject(), SUBJECT);
	dictionary->insert(triple.getPredicate(), PREDICATE);
	dictionary->insert(triple.getObject(), OBJECT);

	NOTIFY(listener, "Generating Dictionary", pos, size);
}

void BasicHDT::loadDictionary(const char *fileName, RDFNotation notation, ProgressListener *listener) {
	PlainDictionary *dict = new PlainDictionary();
    StopWatch st;
    IntermediateListener iListener(listener);
    struct stat fileStat;
    stat(fileName, &fileStat);

	try {
		NOTIFY(listener, "Loading Dictionary", 0, 100);
		iListener.setRange(0,80);
		dict->startProcessing();

		DictionaryLoader dictLoader(dict, &iListener, (unsigned long long) fileStat.st_size);
		RDFParserRaptorCallback pars;
		pars.doParse(fileName, notation, &dictLoader);

		iListener.setRange(80,90);
		dict->stopProcessing(&iListener);
	} catch (const char *e) {
		cout << "Catch exception dictionary" << e << endl;
		delete dict;
		throw e;
	} catch (char *e) {
		cout << "Catch exception dictionary" << e << endl;
		delete dict;
		throw e;
	}

	if(dictionary->getType()==dict->getType()) {
		delete dictionary;
		dictionary = dict;
	} else {
		if(dictionary->getType()==HDTVocabulary::DICTIONARY_TYPE_PFC){
			iListener.setRange(90,100);
			PFCDictionary *pfcd = dynamic_cast<PFCDictionary*>(dictionary);
			pfcd->import(dict, &iListener);
			delete dict;
		} else {
			throw "Dictionary implementation not available.";
		}
	}
	cout << dictionary->getNumberOfElements() << " entries added in " << st << endl << endl;
}

void TriplesLoader::processTriple(hdt::TripleString &triple, unsigned long long pos)
{
	TripleID ti;
	dictionary->tripleStringtoTripleID(triple, ti);
	//cout << "TripleID: " << ti << endl;
	triples->insert(ti);

	NOTIFY(listener, "Generating Triples", pos, size);
}

void BasicHDT::loadTriples(const char *fileName, RDFNotation notation, ProgressListener *listener){

    // Generate Triples
	ModifiableTriples *triplesList = new TriplesList(spec);
    StopWatch st;
    IntermediateListener iListener(listener);
    struct stat fileStat;
    stat(fileName, &fileStat);

	try {
		NOTIFY(listener, "Loading Triples", 0, 100);
		iListener.setRange(0,60);

		triplesList->startProcessing(&iListener);

		TriplesLoader tripLoader(dictionary, triplesList, &iListener, (unsigned long long)fileStat.st_size);
		RDFParserRaptorCallback pars;
		pars.doParse(fileName, notation, &tripLoader);

		triplesList->stopProcessing(&iListener);

		// SORT & Duplicates
		TripleComponentOrder order = parseOrder(spec.get("triples.component.order").c_str());
		if(order==Unknown){
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

	if(triples->getType()==triplesList->getType()) {
		delete triples;
		triples = triplesList;
	}else{
		iListener.setRange(90, 100);
		triples->load(*triplesList, &iListener);
		delete triplesList;
	}

	cout << triples->getNumberOfElements() << " triples added in " << st << endl << endl;
}

void BasicHDT::fillHeader(string &baseUri)
{
	header->insert(baseUri, HDTVocabulary::RDF_TYPE, HDTVocabulary::HDT_DATASET);
	string formatNode = "_:format";
	header->insert(baseUri, HDTVocabulary::HDT_FORMAT_INFORMATION, formatNode);
	string dictNode = "_:dictionary";
	header->insert(formatNode, HDTVocabulary::HDT_DICTIONARY, dictNode);
	string triplesNode = "_:triples";
	header->insert(formatNode, HDTVocabulary::HDT_TRIPLES, triplesNode);
	string statisticsNode = "_:statistics";
	header->insert(baseUri, HDTVocabulary::HDT_STATISTICAL_INFORMATION, statisticsNode);
	string publicationInfoNode = "_:publicationInformation";
	header->insert(baseUri, HDTVocabulary::HDT_PUBLICATION_INFORMATION, publicationInfoNode);

	dictionary->populateHeader(*header, dictNode);
	triples->populateHeader(*header, triplesNode);

	header->insert(statisticsNode, HDTVocabulary::HDT_SIZE, getDictionary().size()+getTriples().size());

#ifdef WIN32

#else
		time_t now;
		char date[40];
		time(&now);
		struct tm *today = localtime(&now);
		strftime(date, 40, "%Y-%m-%dT%H:%M:%S%z", today);
		header->insert(publicationInfoNode, HDTVocabulary::DUBLIN_CORE_ISSUED, date);
#endif

}

void BasicHDT::loadFromRDF(const char *fileName, RDFNotation notation, string baseUri, ProgressListener *listener)
{
	try {
		IntermediateListener iListener(listener);

		iListener.setRange(0,50);
		loadDictionary(fileName, notation, &iListener);

		iListener.setRange(50,99);
		loadTriples(fileName, notation, &iListener);

		//header->insert(statisticsNode, HDTVocabulary::ORIGINAL_SIZE, parser.getSize());

		fillHeader(baseUri);

	}catch (const char *e) {
		cout << "Catch exception load" << endl;
		delete header;
		delete dictionary;
		delete triples;
		createComponents();
		throw e;
	} catch (char *e) {
		cout << "Catch exception load" << endl;
		delete header;
		delete dictionary;
		delete triples;
		createComponents();
		throw e;
	}
}

void BasicHDT::saveToRDF(RDFSerializer &serializer, ProgressListener *listener)
{
	IteratorTripleString *it = search("", "", "");
        try {
            serializer.serialize(it, listener, getTriples().getNumberOfElements());
        } catch (const char *e) {
        } catch (char *e) {
        }
	delete it;
}

void BasicHDT::loadFromHDT(const char *fileName, ProgressListener *listener) {
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
	header = HDTFactory::readHeader(controlInformation);
	header->load(input, controlInformation, &iListener);

	//Load Dictionary.
	iListener.setRange(5, 60);
	controlInformation.clear();
        controlInformation.load(input);
        delete dictionary;
	dictionary = HDTFactory::readDictionary(controlInformation);
	dictionary->load(input, controlInformation, &iListener);

	// Load Triples
	iListener.setRange(60,100);
	controlInformation.clear();
	controlInformation.load(input);
        delete triples;
	triples = HDTFactory::readTriples(controlInformation);
	triples->load(input, controlInformation, &iListener);
    } catch (const char *ex) {
        delete header;
        delete dictionary;
        delete triples;
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
	out.close();
    } catch (const char *ex) {
        delete header;
        delete dictionary;
        delete triples;
        createComponents();
        throw ex;
    }
}

void BasicHDT::saveToHDT(std::ostream & output, ProgressListener *listener)
{
	StopWatch st;
	ControlInformation controlInformation;

	controlInformation.setHeader(true);
	header->save(output, controlInformation, listener);

	controlInformation.setDictionary(true);
	dictionary->save(output, controlInformation, listener);

	controlInformation.clear();
	controlInformation.setTriples(true);
	triples->save(output, controlInformation, listener);
}

void BasicHDT::convert(HDTSpecification &spec)
{

}



BasicModifiableHDT::BasicModifiableHDT() {
	createComponents();
}

BasicModifiableHDT::BasicModifiableHDT(HDTSpecification &spec) {
	this->spec = spec;
	createComponents();
}

BasicModifiableHDT::~BasicModifiableHDT() {
	delete header;
	delete dictionary;
	delete triples;
}

void BasicModifiableHDT::createComponents() {
	std::string dictType = spec.get("dictionary.type");
	std::string triplesType = spec.get("triples.type");

	// FIXME: SELECT
	if(spec.get("noheader")=="true") {
		header = new EmptyHeader();
	} else {
		header = new BasicHeader();
	}
	dictionary = new PlainDictionary();
	triples = new TriplesList();
}



Header & BasicModifiableHDT::getHeader()
{
	return *header;
}

Dictionary & BasicModifiableHDT::getDictionary()
{
	return *dictionary;
}

Triples & BasicModifiableHDT::getTriples()
{
	return *triples;
}


IteratorTripleString *BasicModifiableHDT::search(const char *subject, const char *predicate, const char *object)
{
	TripleString ts(subject, predicate, object);

	TripleID tid;
	dictionary->tripleStringtoTripleID(ts, tid);

//	cout << "TID: "<< tid.getSubject() << "," << tid.getPredicate() << "," << tid.getObject() << endl;

	IteratorTripleID *iterID = triples->search(tid);

	BasicIteratorTripleString *iterator = new BasicIteratorTripleString(dictionary, iterID);
	return iterator;
}

void BasicModifiableHDT::loadFromRDF(const char *fileName, RDFNotation notation, string baseUri, ProgressListener *listener)
{

}

void BasicModifiableHDT::saveToRDF(RDFSerializer &serializer, ProgressListener *listener)
{
    IteratorTripleString *it = search("","","");
    serializer.serialize(it, listener, this->getTriples().getNumberOfElements() );
	delete it;
}

void BasicModifiableHDT::loadFromHDT(const char *fileName, ProgressListener *listener) {
	ifstream input(fileName, ios::binary | ios::in);
	if(!input.good()){
		throw "Error opening file to save HDT.";
	}
	this->loadFromHDT(input, listener);
	input.close();
}

void BasicModifiableHDT::loadFromHDT(std::istream & input, ProgressListener *listener)
{
	//header->load(input);
	ControlInformation controlInformation;
	controlInformation.load(input);
	dictionary->load(input, controlInformation);

	controlInformation.clear();
	controlInformation.load(input);
	triples->load(input, controlInformation);
}

void BasicModifiableHDT::saveToHDT(const char *fileName, ProgressListener *listener)
{
	ofstream out(fileName, ios::binary | ios::out);
	if(!out.good()){
		throw "Error opening file to save HDT.";
	}
	this->saveToHDT(out, listener);
	out.close();
}

void BasicModifiableHDT::saveToHDT(std::ostream & output, ProgressListener *listener)
{
	ControlInformation controlInformation;

	//header->save(output);

	cout << "Saving dictionary" << endl;
	StopWatch st;
	dictionary->save(output, controlInformation);
	cout << "Dictionary saved in " << st << endl;

	cout << "Saving triples" << endl;
	st.reset();
	triples->save(output, controlInformation);
	cout << "Triples saved in " << st << endl;
}

void BasicModifiableHDT::convert(HDTSpecification &spec)
{

}

void BasicModifiableHDT::insert(TripleString & triple)
{
	TripleID tid;
	dictionary->tripleStringtoTripleID(triple, tid);
	triples->insert(tid);
}


void BasicModifiableHDT::insert(IteratorTripleString *triples)
{
	throw "Not implemented";
}

void BasicModifiableHDT::remove(TripleString & triple)
{
	TripleID tid;
	dictionary->tripleStringtoTripleID(triple, tid);
	triples->remove(tid);

	// Fixme: Need to remove from dictionary?
}

void BasicModifiableHDT::remove(IteratorTripleString *triples)
{
	throw "Not implemented";
}




}
