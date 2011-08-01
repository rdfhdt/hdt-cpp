/*
 * RDFParserN3.h
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifndef RDFPARSERN3_H_
#define RDFPARSERN3_H_

#include <istream>

#include "RDFParser.hpp"

namespace hdt {

class RDFParserN3 : public RDFParser {

private:
	uint64_t size;
	std::string line;
	TripleString ts;

public:
	RDFParserN3(std::istream &in, RDFNotation notation);
	virtual ~RDFParserN3();

	bool hasNext();
	TripleString *next();
	void reset();
	uint64_t getPos();
	uint64_t getSize();
};

}

#endif /* RDFPARSERN3_H_ */
