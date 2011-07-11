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
#include <HDTEnums.hpp>

namespace hdt {

class RDFSerializer {
protected:
	std::ostream &stream;
	RDFNotation notation;

public:
	RDFSerializer(std::ostream &s, RDFNotation notation) : stream(s), notation(notation) { }
	virtual ~RDFSerializer() { }
	virtual void serialize(IteratorTripleString *it)=0;
	virtual void endProcessing()=0;

	static RDFSerializer *getSerializer(std::ostream &s, RDFNotation notation);
};

}

#endif /* RDFSERIALIZER_H_ */
