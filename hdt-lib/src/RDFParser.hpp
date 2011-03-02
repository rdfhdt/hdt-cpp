/*
 * RDFParser.h
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#ifndef RDFPARSER_H_
#define RDFPARSER_H_

#include <string>
#include <iostream>

#include "SingleTriple.hpp"

namespace hdt {
class RDFParser: public hdt::IteratorTripleString {

private:
	std::istream *in;
	std::string line;

public:
	RDFParser(std::istream *in);
	~RDFParser();

	bool hasNext();
	TripleString next();
};
}

#endif /* RDFPARSER_H_ */
