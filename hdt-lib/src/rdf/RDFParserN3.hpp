/*
 * RDFParserN3.h
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifndef RDFPARSERN3_H_
#define RDFPARSERN3_H_

#include <stdint.h>
#include <fstream>

#include "RDFParser.hpp"

namespace hdt {

class RDFParserN3 : public RDFParser {

private:
	uint64_t size;
	std::string line;
	TripleString ts;
	std::istream *input;

public:
	RDFParserN3(std::istream &in, RDFNotation notation);
	RDFParserN3(const char *fileName, RDFNotation notation);
	virtual ~RDFParserN3();

	bool hasNext();
	TripleString *next();
	void reset();
	uint64_t getPos();
	uint64_t getSize();
};

}

#endif /* RDFPARSERN3_H_ */
