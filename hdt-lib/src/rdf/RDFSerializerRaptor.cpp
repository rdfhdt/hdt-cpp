/*
 * RDFSerializerRaptor.cpp
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifdef USE_RAPTOR
#include "RDFSerializerRaptor.hpp"

using namespace std;

namespace hdt {

RDFSerializerRaptor::RDFSerializerRaptor(const char *fileName, RDFNotation notation)
	: RDFSerializer(notation),
	  readingFromStream(false)
{
	world = raptor_new_world();

	base_uri = raptor_new_uri(world, (const unsigned char*)"http://www.rdfhdt.org/");

	raptor_serializer* rdf_serializer = raptor_new_serializer(world, getType(notation)) ;

	raptor_serializer_start_to_filename(rdf_serializer, fileName);
}

int iostream_write_bytes (void *context, const void *ptr, size_t size, size_t nmemb) {
	std::ostream *out = reinterpret_cast<std::ostream *>(context);
	if(out->good()) {
		out->write((const char *)ptr, size*nmemb);
		return 0;
	} else {
		return 1;
	}
}

int iostream_write_byte(void *context, const int byte) {
	std::ostream *out = reinterpret_cast<std::ostream *>(context);
	if(out->good()) {
		out->write((const char *)&byte, sizeof(int));
		return 0;
	} else {
		return 1;
	}
}

RDFSerializerRaptor::RDFSerializerRaptor(std::ostream &s, RDFNotation notation)
	: RDFSerializer(notation),
	  readingFromStream(true)
{
	world = raptor_new_world();

	base_uri = raptor_new_uri(world, (const unsigned char*)"http://www.rdfhdt.org/");

	memset(&handler, 0, sizeof(handler));
	handler.write_byte = iostream_write_byte;
	handler.write_bytes = iostream_write_bytes;

	iostream = raptor_new_iostream_from_handler(world, (void *) &s, &handler);
	rdf_serializer = raptor_new_serializer(world, getType(notation)) ;

	raptor_serializer_start_to_iostream(rdf_serializer, base_uri, iostream);
}

RDFSerializerRaptor::~RDFSerializerRaptor() {
	raptor_serializer_serialize_end(rdf_serializer);

	raptor_free_serializer(rdf_serializer);

	if(readingFromStream) {
		raptor_free_iostream(iostream);
	}

	raptor_free_uri(base_uri);

	raptor_free_world(world);
}

raptor_term *getTerm(string &str, raptor_world *world) {
	if(str=="") {
		return NULL;
	}

	if(str.at(0)=='<'){
		return raptor_new_term_from_uri_string(world, (const unsigned char *)str.substr(1, str.length()-2).c_str());
	} else if(str.at(0)=='"') {
		return raptor_new_term_from_literal(world, (const unsigned char *)str.substr(1, str.length()-2).c_str(), NULL, NULL);
	} else if(str.at(0)=='_') {
		return raptor_new_term_from_blank(world, (const unsigned char *)str.c_str());
	} else {
		throw "Unknown kind of term";
	}
}

const char *RDFSerializerRaptor::getType(RDFNotation notation) {
	switch(notation) {
	case N3:
		return "n3";
	case NTRIPLES:
		return "ntriples";
	case XML:
		return "rdfxml-abbrev";
	case TURTLE:
		return "turtle";
	}
	return "ntriples";
}

#include <stdio.h>

void RDFSerializerRaptor::serialize(IteratorTripleString *it, ProgressListener *listener, unsigned int totalTriples)
{
	while( it->hasNext() ) {
		TripleString *ts = it->next();

		if(!ts->isEmpty()) {
			raptor_statement* triple = raptor_new_statement(world);
			triple->subject = getTerm(ts->getSubject(), world);
			triple->predicate = getTerm(ts->getPredicate(), world);
			triple->object = getTerm(ts->getObject(), world);
			raptor_statement_print(triple, stdout);
			raptor_serializer_serialize_statement(rdf_serializer, triple);
			raptor_free_statement(triple);
		}
	}
}

}
#endif
