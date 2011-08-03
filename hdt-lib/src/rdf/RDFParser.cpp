/*
 * RDFParser.cpp
 *
 *  Created on: 11/07/2011
 *      Author: mck
 */

#include "RDFParser.hpp"
#ifdef USE_RAPTOR
#include "RDFParserRaptor.hpp"
#include "RDFParserRaptorLine.hpp"
#endif
#include "RDFParserN3.hpp"

namespace hdt {

RDFParser *RDFParser::getParser(std::istream &stream, RDFNotation notation) {
#if USE_RAPTOR
	if(notation == NTRIPLES) {
		cout << "RDFParserRaptorLine" << endl;
		return new RDFParserRaptorLine(stream,notation);
	}
	cout << "RDFParserRaptor" << endl;
	return new RDFParserRaptor(stream,notation);
#else
	if(notation==NTRIPLES) {
		cout << "RDFParserN3" << endl;
		return new RDFParserN3(stream,notation);
	} else {
		throw "No Parser available for input RDF Format";
	}
#endif
}

RDFParser *RDFParser::getParser(const char *fileName, RDFNotation notation) {
#if USE_RAPTOR
	if(notation == NTRIPLES) {
		cout << "RDFParserRaptorLine" << endl;
		return new RDFParserRaptorLine(fileName,notation);
	}
	cout << "RDFParserRaptor" << endl;
	return new RDFParserRaptor(fileName,notation);
#else
	if(notation==NTRIPLES) {
		cout << "RDFParserN3" << endl;
		return new RDFParserN3(fileName,notation);
	} else {
		throw "No Parser available for input RDF Format";
	}
#endif
}


}
