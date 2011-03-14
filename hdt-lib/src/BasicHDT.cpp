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

#include "BasicHDT.hpp"
#include "header/BasicHeader.hpp"
#include "header/EmptyHeader.hpp"
#include "dictionary/PlainDictionary.hpp"
#include "triples/PlainTriples.hpp"
#include "triples/TripleListDisk.hpp"
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
	std::string dictType = spec.get("dictionary.type");
	std::string triplesType = spec.get("triples.type");

	// FIXME: SELECT
	if(spec.get("noheader")=="true") {
		header = new EmptyHeader();
	} else {
		header = new BasicHeader();
	}
	dictionary = new PlainDictionary();
	triples = new TripleListDisk();
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

	TripleID tid(dictionary->tripleStringtoTripleID(ts));

	IteratorTripleID *iterID = triples->search(tid);

	BasicIteratorTripleString *iterator = new BasicIteratorTripleString(dictionary, iterID);
	return iterator;
}



void BasicHDT::loadFromRDF(std::istream &input, RDFNotation notation)
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

void BasicHDT::saveToRDF(std::ostream & output, RDFNotation notation)
{
	RDFSerializerN3 serializer(output);

	IteratorTripleString *it = search("", "", "");
	serializer.serialize(it);
	serializer.endProcessing();
}


void BasicHDT::loadFromHDT(std::istream & input)
{
	//header->load(input);
	dictionary->load(input, *header);
	triples->load(input, *header);
}

void BasicHDT::saveToHDT(std::ostream & output)
{
	//header->save(output);
	dictionary->save(output);
	triples->save(output);
}

void BasicHDT::insert(TripleString & triple)
{
	TripleID tid = dictionary->tripleStringtoTripleID(triple);
	triples->insert(tid);
}


void BasicHDT::insert(IteratorTripleString *triples)
{
	throw "Not implemented";
}

void BasicHDT::remove(TripleString & triple)
{
	// Fixme: Only if modifiable triples.
	TripleID tid = dictionary->tripleStringtoTripleID(triple);
	triples->remove(tid);

	// Fixme: Need to remove from dictionary?
}

void BasicHDT::remove(IteratorTripleString *triples)
{
	throw "Not implemented";
}

bool BasicHDT::edit(TripleString &oldTriple, TripleString &newTriple)
{
	remove(oldTriple);
	insert(newTriple);
}



}
