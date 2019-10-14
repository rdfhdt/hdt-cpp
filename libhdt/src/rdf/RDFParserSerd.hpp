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
	SerdWorld *world;
	SerdEnv *env;
	RDFCallback *callback;
	string error;
	uint64_t numByte;

	string getString(const SerdNode *term);
	string getStringObject(const SerdNode *term);
	SerdSyntax getParserType(RDFNotation notation);

public:
	RDFParserSerd();
	virtual ~RDFParserSerd();

	void doParse(const char *fileName, const char *baseUri, RDFNotation notation, bool ignoreErrors, RDFCallback *callback);

	friend SerdStatus hdtserd_on_statement(void                *handle,
	                                       SerdStatementFlags   flags,
	                                       const SerdStatement *statement);

	friend SerdStatus hdtserd_on_prefix(void           *handle,
	                                    const SerdNode *name,
	                                    const SerdNode *uri);

	friend SerdStatus hdtserd_on_base(void *handle, const SerdNode *uri);

	friend SerdStatus hdtserd_on_message(void*               handle,
	                                     const char*         domain,
	                                     SerdLogLevel        level,
	                                     const SerdLogField* fields,
	                                     size_t              n_fields,
	                                     const char*         fmt,
	                                     va_list             args);
};

}

#endif

#endif // RDFPARSERSERD_HPP
