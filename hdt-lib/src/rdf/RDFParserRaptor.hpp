/*
 * RDFParserN3.h
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifndef RDFPARSERRAPTOR_H_
#define RDFPARSERRAPTOR_H_

#include <stdint.h>
#include <istream>
#include <vector>

#include <raptor2/raptor2.h>

#include <HDTEnums.hpp>

#include "RDFParser.hpp"

namespace hdt {

class RDFParserRaptor : public RDFParser {

private:
	uint64_t size;
	vector<char> buf;
	TripleString ts;

	vector<TripleString> vectorOutput;
	unsigned int pos;

	raptor_parser *rdf_parser;

	RDFCallback *callback;

	const char *getParserType(RDFNotation notation);
public:
	RDFParserRaptor(std::istream &in, RDFNotation notation);
	RDFParserRaptor(const char *fileName, RDFNotation notation);
	virtual ~RDFParserRaptor();

	bool hasNext();
	TripleString *next();
	void reset();
	uint64_t getPos();
	uint64_t getSize();

	void doParse(RDFCallback *callback);

	friend void raptor_process_triple(void *user_data, raptor_statement *triple);
	friend void raptor_log_handler(void *user_data, raptor_log_message *message);
};



}

#endif
