/*
 * RDFParserRaptorLine.h
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifndef RDFPARSERRAPTORLINE_H_
#define RDFPARSERRAPTORLINE_H_

#include <fstream>
#include <vector>

#include <raptor2/raptor2.h>

#include <HDTEnums.hpp>

#include "RDFParser.hpp"

namespace hdt {

class RDFParserRaptorLine : public RDFParser {

private:
	std::istream *input;
	uint64_t size;
	TripleString ts;

	vector<TripleString> vectorOutput;
	unsigned int pos;

	unsigned int globalLine;

	// Raptor
	raptor_world *world;
	raptor_parser *rdf_parser;
	raptor_uri *base_uri;

	const char *getParserType(RDFNotation notation);
public:
	RDFParserRaptorLine(std::istream &in, RDFNotation notation);
	RDFParserRaptorLine(const char *fileName, RDFNotation notation);
	virtual ~RDFParserRaptorLine();

	void readBlock();

	bool hasNext();
	TripleString *next();
	void reset();
	uint64_t getPos();
	uint64_t getSize();

	friend void raptor_line_process_triple(void *user_data, raptor_statement *triple);
};



}

#endif
