/*
 * RDFParser.h
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#ifndef RDFPARSER_H_
#define RDFPARSER_H_

#include <string>
#include <istream>

#include "SingleTriple.hpp"

namespace hdt {

class RDFParser: public IteratorTripleString {

protected:
	std::istream &input;

public:
	RDFParser(std::istream &in) : input(in) { }
	~RDFParser() { }

	virtual bool hasNext()=0;
	virtual TripleString *next()=0;
};
}

#endif /* RDFPARSER_H_ */
