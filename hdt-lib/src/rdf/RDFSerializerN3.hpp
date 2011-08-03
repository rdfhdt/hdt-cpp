/*
 * RDFSerializerN3.h
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifndef RDFSERIALIZERN3_H_
#define RDFSERIALIZERN3_H_

#include <fstream>
#include "RDFSerializer.hpp"

namespace hdt {

class RDFSerializerN3: public RDFSerializer {
	std::ostream *output;
public:
	RDFSerializerN3(std::ostream &s, RDFNotation notation);
	RDFSerializerN3(const char *fileName, RDFNotation notation);
	virtual ~RDFSerializerN3();
	void serialize(IteratorTripleString *it, ProgressListener *listener=NULL, unsigned int totalTriples=0);
};

}

#endif /* RDFSERIALIZERN3_H_ */
