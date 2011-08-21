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
#include "RDFParserRaptorCallback.hpp"
#endif
#include "RDFParserNtriples.hpp"
#include "RDFParserNtriplesCallback.hpp"

namespace hdt {

RDFParserPull *RDFParserPull::getParserPull(std::istream &stream, RDFNotation notation) {
#if USE_RAPTOR
	cout << "RDFParserRaptor" << endl;
	return new RDFParserRaptor(stream, notation);
#else
	if(notation==NTRIPLES) {
		cout << "RDFParserN3" << endl;
		return new RDFParserNtriples(stream,notation);
	} else {
		throw "No Parser available for input RDF Format";
	}
#endif
}

RDFParserPull *RDFParserPull::getParserPull(const char *fileName, RDFNotation notation) {
#if USE_RAPTOR
	/*if(notation == NTRIPLES) {
		cout << "RDFParserRaptorLine" << endl;
		return new RDFParserRaptorLine(fileName,notation);
	}*/
	cout << "RDFParserRaptor" << endl;
	return new RDFParserRaptor(fileName,notation);
#else
	if(notation==NTRIPLES) {
		cout << "RDFParserN3" << endl;
		return new RDFParserNtriples(fileName,notation);
	} else {
		throw "No Parser available for input RDF Format";
	}
#endif
}


RDFParserCallback *RDFParserCallback::getParserCallback(RDFNotation notation) {
#if USE_RAPTOR
	/*if(notation == NTRIPLES) {
		cout << "RDFParserRaptorLine" << endl;
		return new RDFParserRaptorLine(stream,notation);
	}*/
	cout << "RDFParserRaptor" << endl;
	return new RDFParserRaptorCallback();
#else
	if(notation==NTRIPLES) {
		cout << "RDFParserN3" << endl;
		return new RDFParserNtriplesCallback();
	} else {
		throw "No Parser available for input RDF Format";
	}
#endif
}

}
