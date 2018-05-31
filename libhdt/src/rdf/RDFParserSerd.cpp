#ifdef HAVE_SERD

#ifdef HAVE_LIBZ
#include <zlib.h>
#endif

#include "../util/fileUtil.hpp"
#include "RDFParserSerd.hpp"
#include "RDFParser.hpp"

namespace hdt {

string RDFParserSerd::getString(const SerdNode *term) {
	string out;
	out.reserve(term->n_bytes + 2);

	if(term->type==SERD_URI) {
		out.append((const char *)term->buf, term->n_bytes);
	} else if(term->type==SERD_BLANK) {
		out.append("_:");
		out.append((const char *)term->buf, term->n_bytes);
	} else if(term->type==SERD_CURIE) {
		SerdChunk uri_prefix, uri_suffix;
		if (serd_env_expand(env, term, &uri_prefix, &uri_suffix)) {
			// ERROR BAD Curie / Prefix
		}
		out.append((const char *)uri_prefix.buf, uri_prefix.len);
		out.append((const char *)uri_suffix.buf, uri_suffix.len);
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
	out.append((const char *)term->buf, term->n_bytes);
	out.push_back('\"');
	if(lang!=NULL){
		out.push_back('@');
		out.append((const char *)lang->buf, lang->n_bytes);
	}
	if(dataType!=NULL) {
		out.append("^^<");
		out.append(getString(dataType));
		out.push_back('>');
	}

	return out;
}

SerdStatus hdtserd_on_error(void *handle, const SerdError *error) {
	fprintf(stderr, "error: %s:%u:%u: ",
	        error->filename, error->line, error->col);
	vfprintf(stderr, error->fmt, *error->args);
	throw std::runtime_error("Error parsing input.");
	return error->status;
}

// Callback for base URI changes (@base directives)
SerdStatus hdtserd_on_base(void *handle, const SerdNode *uri) {
	RDFParserSerd *serdParser = reinterpret_cast<RDFParserSerd *>(handle);

	return serd_env_set_base_uri(serdParser->env, uri);
}

// Callback for namespace definitions (@prefix directives)
SerdStatus hdtserd_on_prefix(void           *handle,
                             const SerdNode *name,
                             const SerdNode *uri) {
	RDFParserSerd *serdParser = reinterpret_cast<RDFParserSerd *>(handle);

	return serd_env_set_prefix(serdParser->env, name, uri);
}

// Callback for statements
SerdStatus hdtserd_on_statement(void               *handle,
                                SerdStatementFlags  flags,
                                const SerdNode     *graph,
                                const SerdNode     *subject,
                                const SerdNode     *predicate,
                                const SerdNode     *object,
                                const SerdNode     *datatype,
                                const SerdNode     *lang) {
	RDFParserSerd *serdParser = reinterpret_cast<RDFParserSerd *>(handle);

	serdParser->callback->processTriple(
		TripleString(serdParser->getString(subject),
		             serdParser->getString(predicate),
		             serdParser->getStringObject(object, datatype, lang)),
		serdParser->numByte);

	return SERD_SUCCESS;
}

#ifdef HAVE_LIBZ

static const size_t SERD_PAGE_SIZE = 4096;

struct LibzSerdStream {
	LibzSerdStream(const char *fileName)
		: file(gzopen(fileName, "rb"))
		, err(0)
	{
		if (!file) {
			throw ParseException("Could not open input file for parsing");
		}
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
	case NQUAD: // Deprecated: use `NQUADS' instead.
		return SERD_NQUADS;
	case NQUADS:
		return SERD_NQUADS;
	case NTRIPLES:
		return SERD_NTRIPLES;
	case TRIG:
		return SERD_TRIG;
	case TURTLE:
		return SERD_TURTLE;
	default:
		throw ParseException("Serd parser only supports N-Triples, N-Quads, TriG, and Turtle.");
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
		(SerdBaseSink)hdtserd_on_base,
		(SerdPrefixSink)hdtserd_on_prefix,
		(SerdStatementSink)hdtserd_on_statement,
		NULL);

	serd_reader_set_error_sink(reader, hdtserd_on_error, NULL);

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
