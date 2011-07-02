/*
 * RDFParserN3.h
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifndef RDFPARSERRAPTOR_H_
#define RDFPARSERRAPTOR_H_

#include <istream>
#include <vector>

#include <raptor2/raptor2.h>

#include <HDTEnums.hpp>

#include "RDFParser.hpp"

namespace hdt {

class RDFParserRaptor : public RDFParser {

private:
	TripleString ts;
	RDFNotation notation;

	vector<TripleString> vectorOutput;
	unsigned int pos;

	unsigned int globalLine;

	// Raptor
	raptor_world *world;
	raptor_parser *rdf_parser;
	raptor_uri *base_uri;

	const char *getParserType(RDFNotation notation);
public:
	RDFParserRaptor(std::istream &in, RDFNotation notation);
	virtual ~RDFParserRaptor();

	void readBlock();

	bool hasNext();
	TripleString *next();
	void reset();

	friend void process_triple(void *user_data, raptor_statement *triple);
};



}

#endif
