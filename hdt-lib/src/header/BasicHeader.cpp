/*
 * File: BasicHeader.cpp
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




#include <HDTVocabulary.hpp>
#include <HDTFactory.hpp>

#include "BasicHeader.hpp"

namespace hdt {

BasicHeader::BasicHeader() {
	// FIXME: Specify best HDT types for header using spec.

	spec.set("noheader", "true");

	hdt = HDTFactory::createModifiableHDT(spec);
}

BasicHeader::BasicHeader(HDTSpecification &specification) : spec(specification) {
	// FIXME: Specify best HDT types for header using spec.

	spec.set("noheader", "true");

	hdt = HDTFactory::createModifiableHDT(spec);
}

BasicHeader::~BasicHeader() {
	delete hdt;
}

void BasicHeader::load(std::istream & input, ControlInformation &controlInformation, ProgressListener *listener)
{
	std::string codification = controlInformation.get("codification");
	if(codification != HDTVocabulary::HEADER_PLAIN) {
		throw "Unexpected BasicHeader format";
	}

	throw "Not implemented";
}

void BasicHeader::save(std::ostream & output, ControlInformation &controlInformation, ProgressListener *listener)
{
	controlInformation.clear();
	controlInformation.set("codification", HDTVocabulary::HEADER_PLAIN);
	controlInformation.save(output);

	RDFSerializer *serializer = RDFSerializer::getSerializer(output, NTRIPLES);

	hdt->saveToRDF(*serializer);
	delete serializer;
	output << endl;
}

unsigned int BasicHeader::getNumberOfElements() {
	return hdt->getTriples().getNumberOfElements();
}

void BasicHeader::insert(TripleString & triple)
{
	hdt->insert(triple);
}

void BasicHeader::remove(TripleString & triple)
{
	hdt->remove(triple);
}

void BasicHeader::insert(IteratorTripleString *triples)
{
	hdt->insert(triples);
}

void BasicHeader::remove(IteratorTripleString *triples)
{
	hdt->remove(triples);
}

string BasicHeader::getAnon()
{
	anonCounter++;
	string str = "_:anon"+anonCounter;
	cout << "Anon: " << str << endl;
	return str;
}

IteratorTripleString *BasicHeader::search(const char *subject, const char *predicate, const char *object)
{
	return hdt->search(subject, predicate, object);
}

}


