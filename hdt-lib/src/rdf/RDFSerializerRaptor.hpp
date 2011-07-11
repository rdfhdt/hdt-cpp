/*
 * RDFSerializerRaptor.h
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifndef RDFSERIALIZERRAPTOR_H_
#define RDFSERIALIZERRAPTOR_H_

#ifdef USE_RAPTOR
#include <raptor2/raptor2.h>

#include "RDFSerializer.hpp"

namespace hdt {

class RDFSerializerRaptor: public RDFSerializer {
private:
	// Raptor
	raptor_world *world;
	raptor_parser *rdf_parser;
	raptor_uri *base_uri;


public:
	RDFSerializerRaptor(std::ostream &s, RDFNotation notation);
	virtual ~RDFSerializerRaptor();
	void serialize(IteratorTripleString *it);
	void endProcessing();
};

}

#endif
#endif /* RDFSERIALIZERRAPTOR_H_ */
