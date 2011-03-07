/*
 * RDFSerializer.h
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifndef RDFSERIALIZER_H_
#define RDFSERIALIZER_H_

#include <ostream>
#include <SingleTriple.hpp>

namespace hdt {

class RDFSerializer {
protected:
	std::ostream &stream;

public:
	RDFSerializer(std::ostream &s) : stream(s) { }
	virtual ~RDFSerializer() { }
	virtual void serialize(IteratorTripleString &it)=0;
	virtual void endProcessing()=0;
};

}

#endif /* RDFSERIALIZER_H_ */
