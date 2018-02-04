/*
 * RDFParser.cpp
 *
 *  Created on: 11/07/2011
 *      Author: mck
 */

#include "RDFParser.hpp"
#ifdef HAVE_SERD
#include "RDFParserSerd.hpp"
#endif

namespace hdt {

RDFParserCallback *RDFParserCallback::getParserCallback(RDFNotation notation) {
#ifdef HAVE_SERD
    if(notation==TURTLE || notation==NTRIPLES  || notation==NQUAD) {
        return new RDFParserSerd();
    }
#else
		throw ParseException("No Parser available for input RDF Format");
#endif
}

}
