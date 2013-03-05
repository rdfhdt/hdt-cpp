#ifndef RDFPARSERSERD_HPP
#define RDFPARSERSERD_HPP

#ifdef USE_SERD

#include <stdint.h>

#include <serd-0/serd/serd.h>

#include <HDTEnums.hpp>

#include "RDFParser.hpp"

namespace hdt {

class RDFParserSerd : public RDFParserCallback {

private:
    SerdEnv *env;
    RDFCallback *callback;
    char *error;

    string getString(const SerdNode *term);
    string getStringObject(const SerdNode *term, const SerdNode *dataType, const SerdNode *lang);
    const SerdSyntax getParserType(RDFNotation notation);
public:
    RDFParserSerd();
    virtual ~RDFParserSerd();

    void doParse(const char *fileName, const char *baseUri, RDFNotation notation, RDFCallback *callback);

    friend SerdStatus hdtserd_process_triple(void* handle,
                                            SerdStatementFlags flags,
                                            const SerdNode*    graph,
                                            const SerdNode*    subject,
                                            const SerdNode*    predicate,
                                            const SerdNode*    object,
                                            const SerdNode*    object_datatype,
                                            const SerdNode*    object_lang);

    friend SerdStatus hdtserd_prefixchanged(void* handle,const SerdNode* name, const SerdNode* uri);
    friend SerdStatus hdtserd_basechanged(void* handle, const SerdNode* uri);
};

}

#endif

#endif // RDFPARSERSERD_HPP
