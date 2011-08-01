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
#include <stdint.h>

#include <SingleTriple.hpp>
#include <HDTEnums.hpp>


namespace hdt {

class RDFParser: public IteratorTripleString {

protected:
	std::istream &input;
	RDFNotation notation;

public:
	RDFParser(std::istream &in, RDFNotation notation) : input(in), notation(notation) { }
	~RDFParser() { }

	virtual bool hasNext()=0;
	virtual TripleString *next()=0;
	virtual void reset()=0;
	virtual uint64_t getPos()=0;
	virtual uint64_t getSize()=0;

	static RDFParser *getParser(std::istream &input, RDFNotation notation);
};

}

#endif /* RDFPARSER_H_ */
