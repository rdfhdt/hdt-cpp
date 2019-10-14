#ifdef HAVE_SERD

#ifdef HAVE_LIBZ
#include <zlib.h>
#endif

#include "../util/fileUtil.hpp"
#include "RDFParserSerd.hpp"
#include "RDFParser.hpp"

namespace hdt {

string RDFParserSerd::getString(const SerdNode *term) {
	const SerdNodeType type   = serd_node_get_type(term);
	const size_t       length = serd_node_get_length(term);
	const char* const  str    = serd_node_get_string(term);

	string out;
	out.reserve(length + 2);

	if(type==SERD_URI) {
		out.append(str, length);
	} else if(type==SERD_BLANK) {
		out.append("_:");
		out.append(str, length);
	} else if(type==SERD_CURIE) {
		SerdNode* uri = serd_env_expand(env, term);
		if (uri) {
			out.append(serd_node_get_string(uri));
			serd_node_free(uri);
		} else {
			// ERROR BAD Curie / Prefix
		}
	}
	return out;
}

string RDFParserSerd::getStringObject(const SerdNode *term) {
	const SerdNodeType type   = serd_node_get_type(term);
	const size_t       length = serd_node_get_length(term);
	const char* const  str    = serd_node_get_string(term);

	if(type!=SERD_LITERAL) {
		return getString(term);
	}

	const SerdNode* dataType = serd_node_get_datatype(term);
	const SerdNode* lang = serd_node_get_language(term);

	string out;
	out.reserve(length + 2 +
	            (type ? serd_node_get_length(dataType) + 4 : 0) +
	            (lang ? serd_node_get_length(lang) + 1 : 0));

	out.push_back('\"');
	out.append(str, length);
	out.push_back('\"');
	if(lang!=NULL){
		out.push_back('@');
		out.append(serd_node_get_string(lang), serd_node_get_length(lang));
	}
	if(dataType!=NULL) {
		out.append("^^<");
		out.append(getString(dataType));
		out.push_back('>');
	}

	return out;
}

// FIXME: Add to API?
static const SerdLogField*
find_field(const SerdLogField* fields, size_t n_fields, const char* key)
{
	for (size_t i = 0; i < n_fields; ++i) {
		if (!strcmp(fields[i].key, key)) {
			return &fields[i];
		}
	}

	return NULL;
}

SerdStatus hdtserd_on_message(void               *handle,
                              const char         *domain,
                              SerdLogLevel        level,
                              const SerdLogField *fields,
                              size_t              n_fields,
                              const char         *fmt,
                              va_list             args)
{
	if (level <= SERD_LOG_LEVEL_ERR) {
		const SerdLogField* file = find_field(fields, n_fields, "SERD_FILE");
		const SerdLogField* line = find_field(fields, n_fields, "SERD_LINE");
		const SerdLogField* col  = find_field(fields, n_fields, "SERD_COL");
		if (file && line && col) {
			fprintf(stderr, "error: %s:%s:%s: ",
			        file->value, line->value, col->value);
		} else {
			fprintf(stderr, "error: ");
		}
		vfprintf(stderr, fmt, args);

		throw std::runtime_error("Error parsing input.");
	}

	return SERD_SUCCESS;
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
SerdStatus hdtserd_on_statement(void                *handle,
                                SerdStatementFlags   flags,
                                const SerdStatement *statement)
{
	RDFParserSerd  *serdParser = reinterpret_cast<RDFParserSerd *>(handle);
	const SerdNode *subject    = serd_statement_get_subject(statement);
	const SerdNode *predicate  = serd_statement_get_predicate(statement);
	const SerdNode *object     = serd_statement_get_object(statement);

	serdParser->callback->processTriple(
		TripleString(serdParser->getString(subject),
		             serdParser->getString(predicate),
		             serdParser->getStringObject(object)),
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
	SerdURI   base_uri = SERD_URI_NULL;
	SerdNode* base = serd_new_file_uri(fileName, NULL);
	world = serd_world_new();
	env = serd_env_new(base);

	SerdSink* sink = serd_sink_new(this, env);
	serd_sink_set_base_func(sink, (SerdBaseFunc)hdtserd_on_base);
	serd_sink_set_prefix_func(sink, (SerdPrefixFunc)hdtserd_on_prefix);
	serd_sink_set_statement_func(sink, (SerdStatementFunc)hdtserd_on_statement);

	SerdReader* reader = serd_reader_new(
		world, getParserType(notation), 0, sink, 4096);

	serd_world_set_message_func(world, hdtserd_on_message, NULL);

	SerdNode* input = serd_new_file_uri(fileName, NULL);

	if(fileUtil::str_ends_with(fileName,".gz")){

		// NOTE: Requires serd 0.27.1
#ifdef HAVE_LIBZ
		LibzSerdStream libzSerdStream(fileName);
		SerdStatus status = serd_reader_start_stream(
			reader, &LibzSerdStream::read, &LibzSerdStream::error,
			&libzSerdStream, input, SERD_PAGE_SIZE);
		if (status) {
			throw ParseException((const char*)serd_strerror(status));
		}

		status = serd_reader_read_document(reader);
		status = serd_reader_finish(reader);
#else
		cerr << "HDT Library has not been compiled with gzip support." << endl;
#endif

	} else {
		FILE *in_fd = fopen(fileName, "r");
		// TODO: fadvise sequential
		if(in_fd==NULL) {
			throw ParseException("Could not open input file for parsing");
		}
		SerdStatus status = serd_reader_start_stream(
			reader, (SerdReadFunc)fread, (SerdStreamErrorFunc)ferror,
			in_fd, input, SERD_PAGE_SIZE);
		status = serd_reader_read_document(reader);
		status = serd_reader_finish(reader);
		fclose(in_fd);
	}

	serd_reader_free(reader);

	serd_node_free(input);
	serd_env_free(env);
	serd_node_free(base);
}

}
#else
int RDFParserSerdDummySymbol;
#endif
