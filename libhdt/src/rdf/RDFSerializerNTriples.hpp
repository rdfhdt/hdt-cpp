/*
 * RDFSerializerNTriples.h
 *
 */

#ifndef RDFSERIALIZERNTRIPLES_H_
#define RDFSERIALIZERNTRIPLES_H_

#include <fstream>
#include "RDFSerializer.hpp"

namespace hdt {

class RDFSerializerNTriples: public RDFSerializer {
	std::ostream *output;
public:
	RDFSerializerNTriples(std::ostream &s, RDFNotation notation);
	RDFSerializerNTriples(const char *fileName, RDFNotation notation);
	virtual ~RDFSerializerNTriples();
    void serialize(IteratorTripleString *it, ProgressListener *listener=NULL, size_t totalTriples=0);
};

}

#endif /* RDFSERIALIZERNTRIPLES_H_ */
