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
#include "RDFSerializerN3.hpp"

namespace hdt {

RDFSerializer *RDFSerializer::getSerializer(std::ostream &out, RDFNotation notation) {
#ifdef USE_RAPTOR
	return new RDFSerializerRaptor(out, notation);
#else
	return new RDFSerializerN3(out,notation);
#endif
}


}
