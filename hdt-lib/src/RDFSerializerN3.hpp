/*
 * RDFSerializerN3.h
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifndef RDFSERIALIZERN3_H_
#define RDFSERIALIZERN3_H_

#include "RDFSerializer.hpp"

namespace hdt {

class RDFSerializerN3: public RDFSerializer {
public:
	RDFSerializerN3(std::ostream &s) : RDFSerializer(s) { }
	virtual ~RDFSerializerN3();
	void serialize(IteratorTripleString *it);
	void endProcessing();
};

}

#endif /* RDFSERIALIZERN3_H_ */
