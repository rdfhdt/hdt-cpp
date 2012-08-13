/*
 * File: PlainHeader.cpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * Copyright (C) 2012, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
 * All rights reserved.
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
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#include <HDTVocabulary.hpp>
#include <HDTFactory.hpp>
#include "../rdf/RDFParserNtriples.hpp"
#include "../libdcs/VByte.h"

#include "PlainHeader.hpp"

namespace hdt {

PlainHeader::PlainHeader() : anonCounter(0) {
}

PlainHeader::PlainHeader(HDTSpecification &specification) : spec(specification), anonCounter(0) {
}

PlainHeader::~PlainHeader() {
}

void PlainHeader::load(std::istream & input, ControlInformation &controlInformation, ProgressListener *listener)
{
	std::string codification = controlInformation.get("codification");
	if(codification != HDTVocabulary::HEADER_PLAIN) {
		throw "Unexpected PlainHeader format";
	}

	// Read Size
	uint64_t headerSize = csd::VByte::decode(input);

	// Read all header into a string
	string str(headerSize,'\0');
	input.read(&str[0], headerSize);
	if(input.gcount()!=headerSize) {
	    throw "Error reading header";
	}

	// Convert into a stringstream
	stringstream strstream(str, stringstream::in);

	// Parse header
	RDFParserNtriples parser(strstream, N3);
	while(parser.hasNext()) {
		TripleString *ts = parser.next();
		triples.push_back(*ts);
	}
}

void PlainHeader::save(std::ostream & output, ControlInformation &controlInformation, ProgressListener *listener)
{
	controlInformation.clear();
	controlInformation.set("codification", HDTVocabulary::HEADER_PLAIN);
	controlInformation.save(output);

	// Dump header into a stringbuffer to know size.
	stringstream strbuf(stringstream::out);
	for(vector<TripleString>::iterator it = triples.begin(); it!=triples.end(); it++){
		strbuf << *it << " ." << endl;
	}
	string str = strbuf.str();

	// Dump length	 & buffer
	csd::VByte::encode(output, str.length());
	output << str;
}

unsigned int PlainHeader::getNumberOfElements() {
	return triples.size();
}

void PlainHeader::insert(TripleString & triple)
{
	triples.push_back(triple);
}

void PlainHeader::remove(TripleString & triple)
{
	throw "Not implemented";
}

void PlainHeader::insert(IteratorTripleString *tripit)
{
	while(tripit->hasNext()){
		TripleString *next = tripit->next();
		triples.push_back(*next);
	}

}

void PlainHeader::remove(IteratorTripleString *triples)
{
	throw "Not implemented";
}

IteratorTripleString *PlainHeader::search(const char *subject, const char *predicate, const char *object)
{
    TripleString pattern(subject, predicate, object);

    return new PlainHeaderIteratorTripleString(this, pattern);
}

string PlainHeader::getAnon()
{
	anonCounter++;
	stringstream out;
	out << "_:anon" << anonCounter;
	return out.str();
}

/// ITERATOR
PlainHeaderIteratorTripleString::PlainHeaderIteratorTripleString(PlainHeader *header, TripleString &pattern)
    : header(header), pos(0), pattern(pattern)
{
    doFetch();
}

void PlainHeaderIteratorTripleString::doFetch() {
	do {
		getNextTriple();
	} while(hasMoreTriples && (!nextTriple.match(pattern)));
}

void PlainHeaderIteratorTripleString::getNextTriple()
{
	if(pos<header->triples.size())
		nextTriple = header->triples[pos];

	pos++;

    hasMoreTriples = pos <= header->triples.size();
}

bool PlainHeaderIteratorTripleString::hasNext()
{
    return hasMoreTriples;
}

hdt::TripleString * PlainHeaderIteratorTripleString::next()
{
	returnTriple = nextTriple;
	doFetch();
    return &returnTriple;
}

void PlainHeaderIteratorTripleString::goToStart()
{
	pos=0;
	doFetch();
}

}
