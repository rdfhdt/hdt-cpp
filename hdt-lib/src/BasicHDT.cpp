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
#include "PlainDictionary.hpp"
#include "PlainTriples.hpp"
#include "RDFParser.hpp"

namespace hdt {


BasicHDT::BasicHDT() {
}

BasicHDT::BasicHDT(HDTSpecification &spec) {
	this->spec = spec;
	createComponents();
}

BasicHDT::~BasicHDT() {
	createComponents();
}

void BasicHDT::createComponents() {
	std::string dictType = spec.get("dictionary.type");
	std::string triplesType = spec.get("triples.type");

	std::cout << "Create" << std::endl << "Dict: " << dictType << std::endl <<"Triples: " << triplesType << std::endl;

	dictionary = new PlainDictionary();
	triples = new PlainTriples();
}

void BasicHDT::remove(TripleString & triples)
{

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


IteratorTripleString BasicHDT::search(const char *subject, const char *predicate, const char *object)
{
	TripleString ts(subject, predicate, object);

	TripleID tid(dictionary->tripleStringtoTripleID(ts));

	IteratorTripleID iterID = triples->retrieve(tid);

	BasicIteratorTripleString iterator(dictionary, &iterID);
	return iterator;
}



void BasicHDT::loadFromRDF(std::istream &input)
{
	RDFParser parser(&input);

	// Dictionary
	dictionary->startProcessing();
	while(parser.hasNext()) {
		TripleString ts = parser.next();
		std::cout << ts << std::endl;

		dictionary->insert(ts.getSubject(), SUBJECT);
		dictionary->insert(ts.getPredicate(), PREDICATE);
		dictionary->insert(ts.getObject(), OBJECT);
	}
	dictionary->stopProcessing();

	// Triples
	input.seekg(0, std::ios::beg);

	while(parser.hasNext()) {
		TripleString ts = parser.next();
		std::cout << ts << std::endl;

		TripleID ti = dictionary->tripleStringtoTripleID(ts);

		triples->insert(ti);
	}
}



void BasicHDT::insert(TripleString & triple)
{
}



void BasicHDT::saveToHDT(std::ostream & output)
{
}



void BasicHDT::loadFromHDT(std::istream & input)
{
}



void BasicHDT::saveToRDF(std::ostream & output, RDFNotation notation)
{
}



void BasicHDT::insert(IteratorTripleString & triple)
{
}



void BasicHDT::remove(IteratorTripleString & triples)
{
}


}
