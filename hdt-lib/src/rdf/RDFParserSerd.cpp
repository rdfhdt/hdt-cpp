
#ifdef USE_SERD
#include "RDFParserSerd.hpp"

namespace hdt {

string RDFParserSerd::getString(const SerdNode *term) {
    string out;

    if(term->type==SERD_URI) {
        out.append((char *)term->buf);
    } else if(term->type==SERD_BLANK) {
        out.append("_:");
        out.append((char *)term->buf);
    } else if(term->type==SERD_CURIE) {
        SerdChunk uri_prefix, uri_suffix;
        if (serd_env_expand(env, term, &uri_prefix, &uri_suffix)) {
            // ERROR BAD Curie / Prefix
        }
        out.append((char *)uri_prefix.buf);
        out.append((char *)uri_suffix.buf);
    }
    //cout << out << endl;
    return out;
}

string RDFParserSerd::getStringObject(const SerdNode *term, const SerdNode *dataType, const SerdNode *lang) {
    string out;

    if(term->type!=SERD_LITERAL) {
        return getString(term);
    }

    out.append("\"");
    out.append((char *)term->buf);
    if(lang!=NULL){
        out.append("\"@");
        out.append((char *)lang->buf);
    } else {
        out.append("\"");
    }
    if(dataType!=NULL) {
        out.append("^^");
        out.append((char *)dataType->buf);
    }

    //cout << out << endl;
    return out;
}

SerdStatus hdtserd_error(void* handle, const SerdError* error) {
    return SERD_SUCCESS;
}

/**
   Sink (callback) for base URI changes.

   Called whenever the base URI of the serialisation changes.
*/
SerdStatus hdtserd_basechanged(void* handle, const SerdNode* uri) {
    RDFParserSerd *raptorParser = reinterpret_cast<RDFParserSerd *>(handle);

    return serd_env_set_base_uri(raptorParser->env, uri);
}

/**
   Sink (callback) for namespace definitions.

   Called whenever a prefix is defined in the serialisation.
*/
SerdStatus hdtserd_prefixchanged(void* handle,const SerdNode* name, const SerdNode* uri) {
    RDFParserSerd *raptorParser = reinterpret_cast<RDFParserSerd *>(handle);

    return serd_env_set_prefix(raptorParser->env, name, uri);
}


/**
   Sink (callback) for statements.

   Called for every RDF statement in the serialisation.
*/
SerdStatus hdtserd_process_triple(void* handle,
                                        SerdStatementFlags flags,
                                        const SerdNode*    graph,
                                        const SerdNode*    subject,
                                        const SerdNode*    predicate,
                                        const SerdNode*    object,
                                        const SerdNode*    object_datatype,
                                        const SerdNode*    object_lang) {

    RDFParserSerd *raptorParser = reinterpret_cast<RDFParserSerd *>(handle);

    TripleString ts( raptorParser->getString(subject), raptorParser->getString(predicate), raptorParser->getStringObject(object, object_datatype, object_lang));
    raptorParser->callback->processTriple(ts, 0);

    return SERD_SUCCESS;
}

/**
   Sink (callback) for anonymous node end markers.

   This is called to indicate that the anonymous node with the given
   @c value will no longer be referred to by any future statements
   (i.e. the anonymous serialisation of the node is finished).
*/
SerdStatus hdtserd_end(void* handle, const SerdNode* node) {
    return SERD_SUCCESS;
}

static SerdStatus
quiet_error_sink(void* handle, const SerdError* e)
{
        return SERD_SUCCESS;
}


RDFParserSerd::RDFParserSerd()
{
}

RDFParserSerd::~RDFParserSerd() {

}

const SerdSyntax RDFParserSerd::getParserType(RDFNotation notation){
    switch(notation){
    case NTRIPLES:
        return SERD_NTRIPLES;
    case TURTLE:
        return SERD_TURTLE;
    default:
        throw "Serd parser only supports ntriples and turtle.";
    }
}

void RDFParserSerd::doParse(const char *fileName, const char *baseUri, RDFNotation notation, RDFCallback *callback) {

    this->callback = callback;

    SerdURI  base_uri = SERD_URI_NULL;
    SerdNode base = SERD_NODE_NULL;

    string file(fileName);

    if(file.substr(0,7)=="http://") {
        base = serd_node_new_uri_from_string((const uint8_t *)baseUri, NULL, &base_uri);
    } else {
        base = serd_node_new_file_uri((const uint8_t *)fileName, NULL, &base_uri, false);
    }

    env = serd_env_new(&base);

    SerdReader* reader = serd_reader_new(
                getParserType(notation), this, NULL,
                (SerdBaseSink)hdtserd_basechanged,
                (SerdPrefixSink)hdtserd_prefixchanged,
                (SerdStatementSink)hdtserd_process_triple,
                (SerdEndSink)hdtserd_end);

    serd_reader_set_error_sink(reader, quiet_error_sink, NULL);

    SerdStatus status = SERD_SUCCESS;

    const uint8_t* input=serd_uri_to_path((const uint8_t *)fileName);
    FILE *in_fd = fopen((const char*)input, "r");
    // TODO: fadvise sequential
    if(in_fd==NULL) {
        throw "Could not open input file for parsing";
    }
    status = serd_reader_read_file_handle(reader, in_fd, (const uint8_t *)fileName);

    serd_reader_free(reader);

    fclose(in_fd);

    serd_env_free(env);
    serd_node_free(&base);
}

}
#endif
