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

RDFParser *RDFParser::getParser(std::istream &input, RDFNotation notation) {
#if USE_RAPTOR
	if(notation == NTRIPLES) {
		cout << "RDFParserRaptorLine" << endl;
		return new RDFParserRaptorLine(input,notation);
	}
	cout << "RDFParserRaptor" << endl;
	return new RDFParserRaptor(input,notation);
#else
	if(notation==NTRIPLES) {
		cout << "RDFParserN3" << endl;
		return new RDFParserN3(input,notation);
	} else {
		throw "No Parser available for Input RDF Format";
	}
#endif
}

}
