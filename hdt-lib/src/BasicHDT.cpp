/*
 * Dictionary.hpp
 *
 * Copyright (C) 2011, Javier D. Fernandez, Miguel A. Martinez-Prieto
 *                     Guillermo Rodriguez-Cano, Alejandro Andres,
 *                     Mario Arias
 * All rights reserved.
 *
 * TODO: Define me
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
 *   Guillermo Rodriguez-Cano:  wileeam@acm.org
 *   Alejandro Andres:          fuzzy.alej@gmail.com
 *   Mario Arias:               mario.arias@gmail.com
 *
 * @version $Id$
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
#include "triples/FOQTriples.hpp"
#include "triples/TripleOrderConvert.hpp"

#include "ControlInformation.hpp"
#include "rdf/RDFParserN3.hpp"
#ifdef USE_RAPTOR
#include "rdf/RDFParserRaptor.hpp"
#endif
#include "rdf/RDFSerializerN3.hpp"
#include "util/StopWatch.hpp"


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
		dictionary = new PlainDictionary(spec);
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
	} else if(triplesType==HDTVocabulary::TRIPLES_TYPE_FOQ) {
		triples = new FOQTriples();
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

#define NOTIFY(listener, percent, message) \
    if(listener!=NULL) listener->notifyProgress( (percent), message);

#define NOTIFYCOND(listener, percent, message, number) \
    if(listener!=NULL && ((number)%20000) == 0) listener->notifyProgress( (percent), message);

void BasicHDT::loadFromRDF(std::istream &input, RDFNotation notation, ProgressListener *listener)
{
	// FIXME: Add other parsers.
	if(notation!=N3) {
		throw "Not implemented: Only parsing available: N3";
	}

	long begin = input.tellg();
	input.seekg(0, ios::end);
	long end = input.tellg();
	input.seekg(0, ios::beg);

	RDFParser *parser;

	if(notation==N3) {
		parser = new RDFParserN3(input);
	} else {
#ifdef USE_RAPTOR
		parser = new RDFParserRaptor(input, notation);
#else
		throw "No parser available for format";
#endif
	}

	// Generate Dictionary
	cout << "Generate Dictionary... "<< endl;

	PlainDictionary *dict = new PlainDictionary();

	unsigned int numtriples = 0;
	StopWatch st;
	dict->startProcessing();
	while(parser->hasNext()) {
		TripleString *ts = parser->next();
		numtriples++;
		//std::cout << ts << std::endl;

		dict->insert(ts->getSubject(), SUBJECT);
		dict->insert(ts->getPredicate(), PREDICATE);
		dict->insert(ts->getObject(), OBJECT);

		NOTIFYCOND(listener, input.tellg()*48/end, "Generating Dictionary", numtriples);
	}
	NOTIFY(listener, 48, "Reorganizing Dictionary");

	dict->stopProcessing();

	NOTIFY(listener, 49, "Converting Dictionary");
	if(dictionary->getType()==dict->getType()) {
		delete dictionary;
		dictionary = dict;
	} else {
		if(dictionary->getType()==HDTVocabulary::DICTIONARY_TYPE_PFC){
			cout << "Convert to PFCDictionary" << endl;
			PFCDictionary *pfcd = dynamic_cast<PFCDictionary*>(dictionary);
			pfcd->import(dict);
			delete dict;
		} else {
			throw "Dictionary implementation not available.";
		}
	}
	dictionary->populateHeader(*header, "<http://purl.org/hdt/dictionary>"); // FIXME: Assing appropiate rootnode.
	cout << dictionary->getNumberOfElements() << " entries added in " << st << endl << endl;

	// Generate Triples
	cout << "Generating triples... "<< endl;
	parser->reset();

	ModifiableTriples *triplesList = new TriplesList(spec);
	TripleID ti;
	st.reset();
	triplesList->startProcessing();
	while(parser->hasNext()) {
		TripleString *ts = parser->next();
		//std::cout << ts << std::endl;

		dictionary->tripleStringtoTripleID(*ts, ti);
		//cout << "TripleID: " << ti << endl;
		triplesList->insert(ti);

		NOTIFYCOND(listener, 50+(triplesList->getNumberOfElements()*40)/numtriples, "Generating Triples", triplesList->getNumberOfElements());
	}
	triplesList->stopProcessing();

	// SORT & Duplicates
	TripleComponentOrder order = parseOrder(spec.get("triples.component.order").c_str());
	if(order==Unknown){
		order = SPO;
	}

	NOTIFY(listener, 92, "Sorting Triples");
	triplesList->sort(order);

	NOTIFY(listener, 95, "Removing duplicate Triples");
	triplesList->removeDuplicates();

	NOTIFY(listener, 98, "Convert to final format");
	if(triples->getType()==triplesList->getType()) {
		delete triples;
		triples = triplesList;
	}else{
		triples->load(*triplesList);
		delete triplesList;
	}

	triples->populateHeader(*header, "<http://purl.org/hdt/triples>");
	cout << triples->getNumberOfElements() << " triples added in " << st << endl << endl;

	header->insert("<http://purl.org/hdt/dataset>", HDTVocabulary::ORIGINAL_SIZE, end-begin);
	header->insert("<http://purl.org/hdt/dataset>", HDTVocabulary::HDT_SIZE, getDictionary().size()+getTriples().size());

	delete parser;
}

void BasicHDT::saveToRDF(std::ostream & output, RDFNotation notation, ProgressListener *listener)
{
	RDFSerializerN3 serializer(output);

	IteratorTripleString *it = search("", "", "");
	serializer.serialize(it);
	serializer.endProcessing();
}


void BasicHDT::loadFromHDT(std::istream & input, ProgressListener *listener)
{
	delete header;
	delete dictionary;
	delete triples;

	ControlInformation controlInformation;

	// Load header
	controlInformation.load(input);
	header = HDTFactory::readHeader(controlInformation);
	header->load(input, controlInformation, listener);

	//Load Dictionary.
	controlInformation.clear();
	controlInformation.load(input);
	dictionary = HDTFactory::readDictionary(controlInformation);
	dictionary->load(input, controlInformation, listener);

	// Load Triples
	controlInformation.clear();
	controlInformation.load(input);
	triples = HDTFactory::readTriples(controlInformation);
	triples->load(input, controlInformation, listener);
}

void BasicHDT::saveToHDT(std::ostream & output, ProgressListener *listener)
{
	StopWatch st;
	ControlInformation controlInformation;

	cout << "Saving header" << endl;
	st.reset();
	controlInformation.setHeader(true);
	header->save(output, controlInformation, listener);
	cout << "Header saved in " << st << endl;

	cout << "Saving dictionary" << endl;
	st.reset();
	controlInformation.setDictionary(true);
	dictionary->save(output, controlInformation, listener);
	cout << "Dictionary saved in " << st << endl;

	cout << "Saving triples" << endl;
	st.reset();
	controlInformation.clear();
	controlInformation.setTriples(true);
	triples->save(output, controlInformation, listener);
	cout << "Triples saved in " << st << endl;
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



void BasicModifiableHDT::loadFromRDF(std::istream &input, RDFNotation notation, ProgressListener *listener)
{
	// FIXME: Add other parsers.
		if(notation!=N3) {
			throw "Not implemented: Only parsing available: N3";
		}

		long begin = input.tellg();
		input.seekg(0, ios::end);
		long end = input.tellg();
		input.seekg(0, ios::beg);

		RDFParserN3 parser(input);

		PlainDictionary *dict = new PlainDictionary();
		ModifiableTriples *triplesList = new TriplesList(spec);

		unsigned int numtriples = 0;
		TripleID tid;
		StopWatch st;
		dict->startProcessing();
		triplesList->startProcessing();
		while(parser.hasNext()) {
			TripleString *ts = parser.next();
			numtriples++;
			//std::cout << ts << std::endl;

			unsigned int sid = dict->insert(ts->getSubject(), SUBJECT);
			unsigned int pid = dict->insert(ts->getPredicate(), PREDICATE);
			unsigned int oid = dict->insert(ts->getObject(), OBJECT);
			tid.setAll(sid, pid, oid);

			triplesList->insert(tid);

			NOTIFYCOND(listener, input.tellg()*48/end, "Generating Dictionary", numtriples);
		}
		NOTIFY(listener, 48, "Reorganizing Dictionary");

		dict->stopProcessing();
		triplesList->stopProcessing();

		NOTIFY(listener, 49, "Converting Dictionary");
		if(dictionary->getType()==dict->getType()) {
			delete dictionary;
			dictionary = dict;
		} else {
			if(dictionary->getType()==HDTVocabulary::DICTIONARY_TYPE_PFC){
				PFCDictionary *pfcd = dynamic_cast<PFCDictionary*>(dictionary);
				pfcd->import(dict);
				delete dict;
			} else {
				throw "Dictionary implementation not available.";
			}
		}
		dictionary->populateHeader(*header, "<http://purl.org/hdt/dictionary>"); // FIXME: Assing appropiate rootnode.
		cout << dictionary->getNumberOfElements() << " entries added in " << st << endl << endl;

		// SORT & Duplicates
		TripleComponentOrder order = parseOrder(spec.get("triples.component.order").c_str());
		if(order==Unknown){
			order = SPO;
		}

		NOTIFY(listener, 92, "Sorting Triples");
		triplesList->sort(order);

		NOTIFY(listener, 95, "Removing duplicate Triples");
		triplesList->removeDuplicates();

		NOTIFY(listener, 98, "Convert Triples to final format");
		if(triples->getType()==triplesList->getType()) {
			delete triples;
			triples = triplesList;
		}else{
			triples->load(*triplesList);
			delete triplesList;
		}

		triples->populateHeader(*header, "<http://purl.org/hdt/triples>");
		cout << triples->getNumberOfElements() << " triples added in " << st << endl << endl;

		header->insert("<http://purl.org/hdt/dataset>", HDTVocabulary::ORIGINAL_SIZE, end-begin);
		header->insert("<http://purl.org/hdt/dataset>", HDTVocabulary::HDT_SIZE, getDictionary().size()+getTriples().size());
}

void BasicModifiableHDT::saveToRDF(std::ostream & output, RDFNotation notation, ProgressListener *listener)
{
	RDFSerializerN3 serializer(output);

	IteratorTripleString *it = search("", "", "");
	serializer.serialize(it);
	serializer.endProcessing();
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
