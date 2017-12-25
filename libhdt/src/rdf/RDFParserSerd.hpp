#ifndef RDFPARSERSERD_HPP
#define RDFPARSERSERD_HPP

#ifdef HAVE_SERD

#include <stdint.h>

#include <serd/serd.h>

#include <HDTEnums.hpp>

#include "RDFParser.hpp"

namespace hdt {

class RDFParserSerd : public RDFParserCallback {

private:
	SerdEnv *env;
	RDFCallback *callback;
	string error;
	uint64_t numByte;

	string getString(const SerdNode *term);
	string getStringObject(const SerdNode *term, const SerdNode *dataType, const SerdNode *lang);
	SerdSyntax getParserType(RDFNotation notation);

public:
	RDFParserSerd();
	virtual ~RDFParserSerd();

	void doParse(const char *fileName, const char *baseUri, RDFNotation notation, bool ignoreErrors, RDFCallback *callback);

	friend SerdStatus hdtserd_on_statement(void               *handle,
	                                       SerdStatementFlags  flags,
	                                       const SerdNode     *graph,
	                                       const SerdNode     *subject,
	                                       const SerdNode     *predicate,
	                                       const SerdNode     *object,
	                                       const SerdNode     *datatype,
	                                       const SerdNode     *lang);

	friend SerdStatus hdtserd_on_prefix(void           *handle,
	                                    const SerdNode *name,
	                                    const SerdNode *uri);

	friend SerdStatus hdtserd_on_base(void *handle, const SerdNode *uri);
	friend SerdStatus hdtserd_on_error(void *handle, const SerdError *e);
};

}

#endif

#endif // RDFPARSERSERD_HPP
