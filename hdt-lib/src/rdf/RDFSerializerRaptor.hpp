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
	raptor_serializer *rdf_serializer;
	raptor_uri *base_uri;
	raptor_iostream_handler handler;
	raptor_iostream *iostream;
	bool readingFromStream;

	const char *getType(RDFNotation notation);
public:
	RDFSerializerRaptor(const char *fileName, RDFNotation notation);
	RDFSerializerRaptor(std::ostream &s, RDFNotation notation);
	virtual ~RDFSerializerRaptor();
	void serialize(IteratorTripleString *it, ProgressListener *listener=NULL, unsigned int totalTriples=0);
};

}

#endif
#endif /* RDFSERIALIZERRAPTOR_H_ */
