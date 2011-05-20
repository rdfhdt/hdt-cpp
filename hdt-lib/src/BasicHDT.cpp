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

#include <HDTFactory.hpp>

#include "BasicHDT.hpp"
#include "header/BasicHeader.hpp"
#include "header/EmptyHeader.hpp"
#include "dictionary/PlainDictionary.hpp"

#include "triples/TriplesList.hpp"
#include "triples/TripleListDisk.hpp"
#include "triples/PlainTriples.hpp"
#include "triples/CompactTriples.hpp"
#include "triples/BitmapTriples.hpp"
#include "triples/TripleOrderConvert.hpp"

#include "ControlInformation.hpp"
#include "RDFParserN3.hpp"
#include "RDFSerializerN3.hpp"
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
	if(spec.get("noheader")=="true") {
		header = new EmptyHeader();
	} else {
		header = new BasicHeader(spec);
	}

	// DICTIONARY
	std::string dictType = spec.get("dictionary.type");
	dictionary = new PlainDictionary(spec);

	// TRIPLES
	std::string triplesType = spec.get("triples.type");
	if(triplesType=="http://purl.org/HDT/hdt#triplesBitmap") {
		triples = new BitmapTriples(spec);
	} else if(triplesType=="http://purl.org/HDT/hdt#triplesCompact") {
		triples = new CompactTriples(spec);
	} else if(triplesType=="http://purl.org/HDT/hdt#triplesPlain") {
		triples = new PlainTriples(spec);
	} else if(triplesType=="http://purl.org/HDT/hdt#triplesList") {
		triples = new TriplesList(spec);
	} else if(triplesType=="http://purl.org/HDT/hdt#triplesListDisk") {
		triples = new TripleListDisk();
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
		TripleID tid(dictionary->tripleStringtoTripleID(ts));

		//cout << "TID: "<< tid.getSubject() << "," << tid.getPredicate() << "," << tid.getObject() << endl;
		IteratorTripleID *iterID = triples->search(tid);

		BasicIteratorTripleString *iterator = new BasicIteratorTripleString(dictionary, iterID);
		return iterator;
	} catch (char *e){
		cout << "Exception: " << e << endl;
		return new IteratorTripleString();
	}
}



void BasicHDT::loadFromRDF(std::istream &input, RDFNotation notation)
{
	// FIXME: Add other parsers.
	if(notation!=N3) {
		throw "Not implemented: Only parsing available: N3";
	}

	RDFParserN3 parser(input);

	// Generate Dictionary
	cout << "Generate Dictionary... "<< endl;

	StopWatch st;
	dictionary->startProcessing();
	while(parser.hasNext()) {
		TripleString ts = parser.next();
		//std::cout << ts << std::endl;

		dictionary->insert(ts.getSubject(), SUBJECT);
		dictionary->insert(ts.getPredicate(), PREDICATE);
		dictionary->insert(ts.getObject(), OBJECT);
	}
	dictionary->stopProcessing();
	dictionary->populateHeader(*header);
	cout << dictionary->numberOfElements() << " entries added in " << st << endl << endl;

	// Generate Triples
	cout << "Generating triples... "<< endl;
	input.clear(); // Resets EOF
	input.seekg(0, std::ios::beg);

	ModifiableTriples *triplesList = new TriplesList(spec);
	st.reset();
	triplesList->startProcessing();
	while(parser.hasNext()) {
		TripleString ts = parser.next();
		//std::cout << ts << std::endl;

		TripleID ti = dictionary->tripleStringtoTripleID(ts);
		triplesList->insert(ti);

		//std::cout << ti << std::endl;
	}
	triplesList->stopProcessing();

	// SORT & Duplicates
	TripleComponentOrder order = parseOrder(spec.get("triples.component.order").c_str());
	if(order==Unknown){
		order = SPO;
	}
	triplesList->sort(order);
	triplesList->removeDuplicates();

	// Convert to final Triples
	triples->load(*triplesList);
	delete triplesList;

	triples->populateHeader(*header);
	cout << triples->getNumberOfElements() << " triples added in " << st << endl << endl;
}

void BasicHDT::saveToRDF(std::ostream & output, RDFNotation notation)
{
	RDFSerializerN3 serializer(output);

	IteratorTripleString *it = search("", "", "");
	serializer.serialize(it);
	serializer.endProcessing();
}


void BasicHDT::loadFromHDT(std::istream & input)
{
	delete dictionary;
	delete triples;

	//header->load(input);
	ControlInformation controlInformation;
	controlInformation.load(input);

	dictionary = HDTFactory::readDictionary(controlInformation);
	dictionary->load(input, controlInformation);

	controlInformation.clear();
	controlInformation.load(input);
	triples = HDTFactory::readTriples(controlInformation);
	triples->load(input, controlInformation);
}

void BasicHDT::saveToHDT(std::ostream & output)
{
	ControlInformation controlInformation;

	//header->save(output);

	cout << "Saving dictionary" << endl;
	StopWatch st;
	controlInformation.setDictionary(true);
	dictionary->save(output, controlInformation);
	cout << "Dictionary saved in " << st << endl;

	cout << "Saving triples" << endl;
	st.reset();
	controlInformation.clear();
	controlInformation.setTriples(true);
	triples->save(output, controlInformation);
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

	TripleID tid(dictionary->tripleStringtoTripleID(ts));

//	cout << "TID: "<< tid.getSubject() << "," << tid.getPredicate() << "," << tid.getObject() << endl;

	IteratorTripleID *iterID = triples->search(tid);

	BasicIteratorTripleString *iterator = new BasicIteratorTripleString(dictionary, iterID);
	return iterator;
}



void BasicModifiableHDT::loadFromRDF(std::istream &input, RDFNotation notation)
{
	// FIXME: Add other parsers.
	if(notation!=N3) {
		throw "Not implemented: Only parsing available: N3";
	}

	RDFParserN3 parser(input);

	// Generate Dictionary
	cout << "Generate Dictionary "<< endl;

	StopWatch st;
	dictionary->startProcessing();
	while(parser.hasNext()) {
		TripleString ts = parser.next();
		//std::cout << ts << std::endl;

		dictionary->insert(ts.getSubject(), SUBJECT);
		dictionary->insert(ts.getPredicate(), PREDICATE);
		dictionary->insert(ts.getObject(), OBJECT);
	}
	dictionary->stopProcessing();
	dictionary->populateHeader(*header);
	cout << dictionary->numberOfElements() << " entries added in " << st << endl;

	// Generate Triples
	cout << "Generating triples "<< endl;
	input.clear(); // Resets EOF
	input.seekg(0, std::ios::beg);

	st.reset();
	triples->startProcessing();
	while(parser.hasNext()) {
		TripleString ts = parser.next();
		//std::cout << ts << std::endl;

		TripleID ti = dictionary->tripleStringtoTripleID(ts);

		triples->insert(ti);
	}
	triples->stopProcessing();
	triples->populateHeader(*header);
	cout << triples->getNumberOfElements() << " triples added in " << st << endl;
}

void BasicModifiableHDT::saveToRDF(std::ostream & output, RDFNotation notation)
{
	RDFSerializerN3 serializer(output);

	IteratorTripleString *it = search("", "", "");
	serializer.serialize(it);
	serializer.endProcessing();
}


void BasicModifiableHDT::loadFromHDT(std::istream & input)
{
	//header->load(input);
	ControlInformation controlInformation;
	controlInformation.load(input);
	dictionary->load(input, controlInformation);

	controlInformation.clear();
	controlInformation.load(input);
	triples->load(input, controlInformation);
}

void BasicModifiableHDT::saveToHDT(std::ostream & output)
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
	TripleID tid = dictionary->tripleStringtoTripleID(triple);
	triples->insert(tid);
}


void BasicModifiableHDT::insert(IteratorTripleString *triples)
{
	throw "Not implemented";
}

void BasicModifiableHDT::remove(TripleString & triple)
{
	// Fixme: Only if modifiable triples.
	TripleID tid = dictionary->tripleStringtoTripleID(triple);
	triples->remove(tid);

	// Fixme: Need to remove from dictionary?
}

void BasicModifiableHDT::remove(IteratorTripleString *triples)
{
	throw "Not implemented";
}




}
