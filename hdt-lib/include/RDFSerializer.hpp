/*
 * RDFSerializer.h
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifndef RDFSERIALIZER_H_
#define RDFSERIALIZER_H_

#include <iostream>
#include <SingleTriple.hpp>
#include <HDTEnums.hpp>
#include <HDTListener.hpp>

namespace hdt {

class RDFSerializer {
protected:
	RDFNotation notation;

public:
	RDFSerializer(RDFNotation notation) : notation(notation) { }
	virtual ~RDFSerializer() { }
	virtual void serialize(IteratorTripleString *it, ProgressListener *listener=NULL, unsigned int totalTriples=0)=0;

	static RDFSerializer *getSerializer(const char *fileName, RDFNotation notation);
	static RDFSerializer *getSerializer(std::ostream &output, RDFNotation notation);
};

}

#endif /* RDFSERIALIZER_H_ */
