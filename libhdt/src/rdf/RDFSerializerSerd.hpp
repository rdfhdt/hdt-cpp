#ifndef RDFSERIALIZERSERD_H_
#define RDFSERIALIZERSERD_H_

#ifdef HAVE_SERD

#include <serd/serd.h>

#include "RDFSerializer.hpp"

namespace hdt {

class RDFSerializerSerd : public RDFSerializer {
private:
	FILE       *file;
	SerdEnv    *env;
	SerdWriter *writer;

public:
	RDFSerializerSerd(const char *fileName, RDFNotation notation);
	RDFSerializerSerd(std::ostream &s, RDFNotation notation);

	virtual ~RDFSerializerSerd();

	void serialize(IteratorTripleString *it,
	               ProgressListener *listener=NULL,
	               size_t totalTriples=0);
};

}

#endif

#endif /* RDFSERIALIZERSERD_H_ */
