#ifdef HAVE_SERD

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <stdexcept>

#include "RDFSerializerSerd.hpp"

namespace hdt {

static SerdSyntax getType(RDFNotation notation) {
	switch (notation) {
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
		throw std::runtime_error("Serd seriaizer only supports N-Quads, N-Triples, TriG, and Turtle.");
	}
}

static size_t file_sink(const void* buf, size_t len, void* stream)
{
	return fwrite(buf, len, 1, reinterpret_cast<FILE*>(stream));
}

RDFSerializerSerd::RDFSerializerSerd(const char *fileName,
                                     RDFNotation notation)
	: RDFSerializer(notation)
	, file(fopen(fileName, "w"))
	, env(serd_env_new(NULL))
	, writer(serd_writer_new(getType(notation), SERD_STYLE_ASCII,
	                         env, NULL, file_sink, file))
{
}

static size_t stream_sink(const void* buf, size_t len, void* stream)
{
	std::ostream *out = reinterpret_cast<std::ostream *>(stream);
	if (out->good()) {
		out->write((const char *)buf, len);
		return len;
	}
	return 0;
}

RDFSerializerSerd::RDFSerializerSerd(std::ostream &s, RDFNotation notation)
	: RDFSerializer(notation)
	, file(NULL)
	, env(serd_env_new(NULL))
	, writer(serd_writer_new(getType(notation), SERD_STYLE_ASCII,
	                         env, NULL, stream_sink, &s))
{
}

RDFSerializerSerd::~RDFSerializerSerd()
{
	serd_writer_finish(writer);
	serd_writer_free(writer);
	if (file) {
		fclose(file);
	}
}

SerdNode getTerm(const string &str, SerdNode* datatype, SerdNode* lang)
{
	if (str.empty()) {
		throw std::runtime_error("Empty Value on triple!");
	}

	const uint8_t *const buf = (const uint8_t*)str.c_str();
	const size_t         len = str.length();
	if (str.at(0) == '"') {
		const size_t endQuote = str.rfind("\"");
		if (!strncmp((const char*)buf + endQuote, "\"^^", 3)) {
			if (!datatype) {
				throw std::runtime_error("Unexpected datatype");
			}

			const uint8_t* datatypeStart = buf + endQuote + 3;
			if (*datatypeStart == '<') {
				*datatype = serd_node_from_substring(
					SERD_URI, datatypeStart + 1, len - endQuote - 5);
			} else {
				*datatype = serd_node_from_string(SERD_CURIE, datatypeStart);
			}
		} else if (!strncmp((const char*)buf + endQuote, "\"@", 2)) {
			if (!lang) {
				throw std::runtime_error("Unexpected language");
			}

			*lang = serd_node_from_string(SERD_LITERAL, buf + endQuote + 2);
		}

		return serd_node_from_substring(SERD_LITERAL, buf + 1, endQuote - 1);
	} else if (str.at(0) == '_') {
		return serd_node_from_string(SERD_BLANK, buf + 2);
	} else {
		return serd_node_from_string(SERD_URI, buf);
	}

	return SERD_NODE_NULL;
}

void RDFSerializerSerd::serialize(IteratorTripleString *it,
                                  ProgressListener     *listener,
                                  size_t          totalTriples)
{
	for (unsigned n = 0; it->hasNext(); ++n) {
		const TripleString *ts = it->next();
		if (!ts->isEmpty()) {
			SerdNode subject   = getTerm(ts->getSubject(), NULL, NULL);
			SerdNode predicate = getTerm(ts->getPredicate(), NULL, NULL);
			SerdNode datatype  = SERD_NODE_NULL;
			SerdNode lang      = SERD_NODE_NULL;
			SerdNode object    = getTerm(ts->getObject(), &datatype, &lang);

			serd_writer_write_statement(
				writer, 0, NULL,
				&subject, &predicate, &object, &datatype, &lang);

			NOTIFYCOND(listener, "Exporting HDT to RDF", n, totalTriples);
		}
	}
}

}

#else
int RDFSerializerSerdDummySymbol;
#endif
