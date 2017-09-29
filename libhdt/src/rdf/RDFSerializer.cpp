/*
 * RDFSerializer.cpp
 *
 *  Created on: 11/07/2011
 *      Author: mck
 */
#include <stdexcept>
#include "RDFSerializer.hpp"
#ifdef HAVE_SERD
#include "RDFSerializerSerd.hpp"
#endif
#include "RDFSerializerNTriples.hpp"

namespace hdt {

RDFSerializer *RDFSerializer::getSerializer(std::ostream &out, RDFNotation notation) {

#ifdef HAVE_SERD
	return new RDFSerializerSerd(out, notation);
#else
	if(notation==NTRIPLES) {
		return new RDFSerializerNTriples(out,notation);
	} else {
		throw std::runtime_error("RDFSerialization not available");
	}
#endif
}

RDFSerializer *RDFSerializer::getSerializer(const char *fileName, RDFNotation notation) {

#ifdef HAVE_SERD
	return new RDFSerializerSerd(fileName, notation);
#else
	if(notation==NTRIPLES) {
		return new RDFSerializerNTriples(fileName,notation);
	} else {
		throw std::runtime_error("RDFSerialization not available");
	}
#endif
}

}
