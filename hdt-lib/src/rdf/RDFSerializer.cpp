/*
 * RDFSerializer.cpp
 *
 *  Created on: 11/07/2011
 *      Author: mck
 */

#include "RDFSerializer.hpp"
#ifdef USE_RAPTOR
#include "RDFSerializerRaptor.hpp"
#endif
#include "RDFSerializerNTriples.hpp"

namespace hdt {

RDFSerializer *RDFSerializer::getSerializer(std::ostream &out, RDFNotation notation) {

	if(notation==NTRIPLES) {
		return new RDFSerializerNTriples(out,notation);
	} else {
#ifdef USE_RAPTOR
		return new RDFSerializerRaptor(out, notation);
#else
		throw "RDFSerialization not available";
#endif
	}
}

RDFSerializer *RDFSerializer::getSerializer(const char *fileName, RDFNotation notation) {

	if(notation==NTRIPLES) {
		return new RDFSerializerNTriples(fileName,notation);
	} else {
#ifdef USE_RAPTOR
	return new RDFSerializerRaptor(fileName, notation);
#else
		throw "RDFSerialization not available";
#endif
	}
}

}
