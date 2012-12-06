/*
 * RDFSerializerNTriples.cpp
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#include "RDFSerializerNTriples.hpp"

using namespace std;

namespace hdt {

RDFSerializerNTriples::RDFSerializerNTriples(std::ostream &s, RDFNotation notation)
	: RDFSerializer(notation),
	  output(&s)
	{

}

RDFSerializerNTriples::RDFSerializerNTriples(const char *fileName, RDFNotation notation)
	: RDFSerializer(notation),
	  output(new std::ofstream(fileName, ios::binary | ios::out))
{

}

RDFSerializerNTriples::~RDFSerializerNTriples() {

}

void serializeTerm(std::string str, ostream &output) {
	if(str=="") {
		throw "Empty Value on triple!";
	}

	// FIXME: Escape non-ascii.

	if(str.at(0)=='"') {
		output << str;
	} else if(str.at(0)=='_') {
		output << str;
	} else {
		output << "<" << str << ">";
	}
}

void RDFSerializerNTriples::serialize(IteratorTripleString *it, ProgressListener *listener, unsigned int totalTriples)
{
	unsigned int numTriple=0;
	while(it->hasNext()) {
		TripleString *ts = it->next();

		if(!ts->isEmpty()) {
			serializeTerm(ts->getSubject(), *output);
			*output << ' ';
			serializeTerm(ts->getPredicate(), *output);
			*output << ' ';
			serializeTerm(ts->getObject(), *output);
			*output << " ." << endl;
		}
		numTriple++;

		NOTIFYCOND(listener, "Exporting HDT to RDF", numTriple, totalTriples);
	}
}

}
