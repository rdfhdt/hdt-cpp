/*
 * RDFParser.cpp
 *
 *  Created on: 11/07/2011
 *      Author: mck
 */

#include "RDFParser.hpp"
#ifdef HAVE_RAPTOR
#include "RDFParserRaptorCallback.hpp"
#endif
#include "RDFParserNtriples.hpp"
#include "RDFParserNtriplesCallback.hpp"
#ifdef HAVE_SERD
#include "RDFParserSerd.hpp"
#endif

namespace hdt {

RDFParserPull *RDFParserPull::getParserPull(std::istream &stream, RDFNotation notation) {
	if(notation==NTRIPLES) {
		return new RDFParserNtriples(stream,notation);
	} else {
		throw "No Parser available for input RDF Format";
	}
}

RDFParserPull *RDFParserPull::getParserPull(const char *fileName, RDFNotation notation) {
	if(notation==NTRIPLES) {
		return new RDFParserNtriples(fileName,notation);
	} else {
		throw "No Parser available for input RDF Format";
	}
}


RDFParserCallback *RDFParserCallback::getParserCallback(RDFNotation notation, string outputFormat) {
#ifdef HAVE_SERD
    if(notation==TURTLE || notation==NTRIPLES) {
        return new RDFParserSerd(outputFormat);
    }
#endif
	if(notation==NTRIPLES) {
		return new RDFParserNtriplesCallback();
	} else {
#ifdef HAVE_RAPTOR
		return new RDFParserRaptorCallback();
#else
		throw "No Parser available for input RDF Format";
#endif
	}
}

}
