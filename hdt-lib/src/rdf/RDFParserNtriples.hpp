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

class RDFParserNtriples : public RDFParserPull {

private:
	uint64_t size;
	std::string line;
	TripleString ts;
	std::istream *input;

public:
	RDFParserNtriples(std::istream &in, RDFNotation notation);
	RDFParserNtriples(const char *fileName, RDFNotation notation);
	virtual ~RDFParserNtriples();

	bool hasNext();
	TripleString *next();
	void reset();
	uint64_t getPos();
	uint64_t getSize();
};

}

#endif /* RDFPARSERN3_H_ */
