#ifdef HAVE_SERD

#include "../util/fileUtil.hpp"
#include "RDFParserSerd.hpp"
#include "RDFParser.hpp"

namespace hdt {

string RDFParserSerd::getString(const SerdNode *term) {
	string out;
	out.reserve(term->n_bytes + 2);

	if(term->type==SERD_URI) {
		out.append((const char *)term->buf);
	} else if(term->type==SERD_BLANK) {
		out.append("_:");
		out.append((const char *)term->buf);
	} else if(term->type==SERD_CURIE) {
		SerdChunk uri_prefix, uri_suffix;
		if (serd_env_expand(env, term, &uri_prefix, &uri_suffix)) {
			// ERROR BAD Curie / Prefix
		}
		out.append((const char *)uri_prefix.buf);
		out.append((const char *)uri_suffix.buf);
	}
	return out;
}

string RDFParserSerd::getStringObject(const SerdNode *term,
                                      const SerdNode *dataType,
                                      const SerdNode *lang) {
	if(term->type!=SERD_LITERAL) {
		return getString(term);
	}

	string out;
	out.reserve(term->n_bytes + 2 +
	            (dataType ? dataType->n_bytes + 4 : 0) +
	            (lang     ? lang->n_bytes + 1     : 0));

	out.push_back('\"');
	out.append((const char *)term->buf);
	out.push_back('\"');
	if(lang!=NULL){
		out.push_back('@');
		out.append((const char *)lang->buf);
	}
	if(dataType!=NULL) {
		out.append("^^<");
		out.append((const char *)dataType->buf);
		out.push_back('>');
	}

	return out;
}

SerdStatus hdtserd_error(void* handle, const SerdError* error) {
	fprintf(stderr, "error: %s:%u:%u: ",
	        error->filename, error->line, error->col);
	vfprintf(stderr, error->fmt, *error->args);
	throw std::runtime_error("Error parsing input.");
	return error->status;
}

/**
   Sink (callback) for base URI changes.

   Called whenever the base URI of the serialisation changes.
*/
SerdStatus hdtserd_basechanged(void* handle, const SerdNode* uri) {
	RDFParserSerd *serdParser = reinterpret_cast<RDFParserSerd *>(handle);

	return serd_env_set_base_uri(serdParser->env, uri);
}

/**
   Sink (callback) for namespace definitions.

   Called whenever a prefix is defined in the serialisation.
*/
SerdStatus hdtserd_prefixchanged(void* handle,const SerdNode* name, const SerdNode* uri) {
	RDFParserSerd *serdParser = reinterpret_cast<RDFParserSerd *>(handle);

	return serd_env_set_prefix(serdParser->env, name, uri);
}

/**
   Sink (callback) for statements.

   Called for every RDF statement in the serialisation.
*/
SerdStatus hdtserd_process_triple(void*              handle,
                                  SerdStatementFlags flags,
                                  const SerdNode*    graph,
                                  const SerdNode*    subject,
                                  const SerdNode*    predicate,
                                  const SerdNode*    object,
                                  const SerdNode*    object_datatype,
                                  const SerdNode*    object_lang) {

	RDFParserSerd *serdParser = reinterpret_cast<RDFParserSerd *>(handle);

	TripleString ts(serdParser->getString(subject), serdParser->getString(predicate), serdParser->getStringObject(object, object_datatype, object_lang));
	serdParser->callback->processTriple(ts, serdParser->numByte);

	return SERD_SUCCESS;
}

#ifdef HAVE_LIBZ

static const size_t SERD_PAGE_SIZE = 4096;
static const unsigned LIBZ_BUFFER_SIZE = 64 * 1024;

struct LibzSerdStream {
	LibzSerdStream(const char *fileName)
		: file(gzopen(fileName, "rb"))
		, err(0)
	{
		if (!file) {
			throw ParseException("Could not open input file for parsing");
		}
		gzbuffer(file, LIBZ_BUFFER_SIZE);
	}

	~LibzSerdStream() {
		gzclose(file);
	}

	static int error(void *stream) {
		LibzSerdStream *s = reinterpret_cast<LibzSerdStream *>(stream);
		return s->err;
	}

	static size_t read(void *buf, size_t size, size_t nmemb, void *stream) {
		LibzSerdStream *s = reinterpret_cast<LibzSerdStream *>(stream);

		const int numRead = gzread(s->file, buf, nmemb * size);
		if (numRead == -1) {
			s->err = 1;
		}

		return numRead;
	}

private:
	gzFile file;
	int err;
};

#endif

RDFParserSerd::RDFParserSerd() : numByte(0)
{
}

RDFParserSerd::~RDFParserSerd() {
}

SerdSyntax RDFParserSerd::getParserType(RDFNotation notation) {
	switch(notation){
	case NTRIPLES:
		return SERD_NTRIPLES;
	case TURTLE:
		return SERD_TURTLE;
	default:
		throw ParseException("Serd parser only supports ntriples and turtle.");
	}
}

void RDFParserSerd::doParse(const char *fileName, const char *baseUri, RDFNotation notation, bool ignoreErrors, RDFCallback *callback) {
	this->callback = callback;
	this->numByte = fileUtil::getSize(fileName);

	// Create Base URI and environment
	SerdURI  base_uri = SERD_URI_NULL;
	SerdNode base = serd_node_new_file_uri((const uint8_t *)fileName, NULL, &base_uri, false);
	env = serd_env_new(&base);

	SerdReader* reader = serd_reader_new(
		getParserType(notation), this, NULL,
		(SerdBaseSink)hdtserd_basechanged,
		(SerdPrefixSink)hdtserd_prefixchanged,
		(SerdStatementSink)hdtserd_process_triple,
		NULL);

	serd_reader_set_error_sink(reader, hdtserd_error, NULL);

	const uint8_t* input=serd_uri_to_path((const uint8_t *)fileName);

	if(fileUtil::str_ends_with(fileName,".gz")){

		// NOTE: Requires serd 0.27.1
#ifdef HAVE_LIBZ
		LibzSerdStream libzSerdStream(fileName);
		const SerdStatus status = serd_reader_read_source(
			reader, &LibzSerdStream::read, &LibzSerdStream::error,
			&libzSerdStream, input, SERD_PAGE_SIZE);
		if (status) {
			throw ParseException((const char*)serd_strerror(status));
		}
#else
		cerr << "HDT Library has not been compiled with gzip support." << endl;
#endif

	} else {
		FILE *in_fd = fopen((const char*)input, "r");
		// TODO: fadvise sequential
		if(in_fd==NULL) {
			throw ParseException("Could not open input file for parsing");
		}
		serd_reader_read_file_handle(reader, in_fd, (const uint8_t *)fileName);
		fclose(in_fd);
	}

	serd_reader_free(reader);

	serd_env_free(env);
	serd_node_free(&base);
}

}
#else
int RDFParserSerdDummySymbol;
#endif
