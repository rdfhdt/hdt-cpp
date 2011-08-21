/*
 * RDFParserN3.h
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifndef RDFPARSERNTRIPLESCALLBACK_H_
#define RDFPARSERNTRIPLESCALLBACK_H_

#include <stdint.h>

#include <HDTEnums.hpp>

#include "RDFParser.hpp"

namespace hdt {

class RDFParserNtriplesCallback : public RDFParserCallback {

private:
	RDFCallback *callback;

	const char *getParserType(RDFNotation notation);
public:
	RDFParserNtriplesCallback();
	virtual ~RDFParserNtriplesCallback();

	void doParse(const char *fileName, const char *baseUri, RDFNotation notation, RDFCallback *callback);
};



}

#endif
