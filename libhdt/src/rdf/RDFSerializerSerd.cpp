#ifdef HAVE_SERD

#include <cassert>
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

RDFSerializerSerd::RDFSerializerSerd(const char *fileName,
                                     RDFNotation notation)
	: RDFSerializer(notation)
	, file(fopen(fileName, "w"))
	, world(serd_world_new())
	, env(serd_env_new(NULL))
	, writer(serd_writer_new(world, getType(notation), 0,
	                         env, (SerdWriteFunc)fwrite, file))
{
}

static size_t stream_sink(const void* buf, size_t size, size_t nmemb, void* stream)
{
	std::ostream *out = reinterpret_cast<std::ostream *>(stream);
	if (out->good()) {
		out->write((const char *)buf, nmemb);
		return nmemb;
	}
	return 0;
}

RDFSerializerSerd::RDFSerializerSerd(std::ostream &s, RDFNotation notation)
	: RDFSerializer(notation)
	, file(NULL)
	, env(serd_env_new(NULL))
	, writer(serd_writer_new(world, getType(notation), 0,
	                         env, (SerdWriteFunc)stream_sink, &s))
{
}

RDFSerializerSerd::~RDFSerializerSerd()
{
	serd_writer_finish(writer);
	serd_writer_free(writer);
	serd_env_free(env);
	serd_world_free(world);
	if (file) {
		fclose(file);
	}
}

static SerdNode* getTerm(const string &str)
{
	if (str.empty()) {
		throw std::runtime_error("Empty Value on triple!");
	}

	const char* const buf = str.c_str();
	const size_t      len = str.length();
	if (str.at(0) == '"') {
		const size_t endQuote = str.rfind("\"");
		if (!strncmp(buf + endQuote, "\"^^", 3)) {
			assert(buf[endQuote + 3] == '<');

			const char*  datatypeStart = buf + endQuote + 4;
			const size_t datatypeLen   = len - endQuote - 5;

			return serd_new_literal(
				buf + 1, endQuote - 1, datatypeStart, datatypeLen, NULL, 0);
		} else if (!strncmp(buf + endQuote, "\"@", 2)) {
			const char*  langStart = buf + endQuote + 2;
			const size_t langLen   = len - endQuote - 2;

			return serd_new_literal(buf + 1, endQuote - 1, NULL, 0, langStart, langLen);
		} else {
			return serd_new_substring(buf + 1, endQuote - 1);
		}
	} else if (str.at(0) == '_') {
		return serd_new_simple_node(SERD_BLANK, buf + 2, len - 2);
	} else {
		return serd_new_simple_node(SERD_URI, buf, len);
	}

	return NULL;
}

void RDFSerializerSerd::serialize(IteratorTripleString *it,
                                  ProgressListener     *listener,
                                  size_t          totalTriples)
{
	const SerdSink* sink = serd_writer_get_sink(writer);
	for (unsigned n = 0; it->hasNext(); ++n) {
		const TripleString *ts = it->next();
		if (!ts->isEmpty()) {
			SerdNode* subject   = getTerm(ts->getSubject());
			SerdNode* predicate = getTerm(ts->getPredicate());
			SerdNode* object    = getTerm(ts->getObject());

			serd_sink_write(sink, 0, subject, predicate, object, NULL);

			serd_node_free(object);
			serd_node_free(predicate);
			serd_node_free(subject);
			NOTIFYCOND(listener, "Exporting HDT to RDF", n, totalTriples);
		}
	}
}

}

#else
int RDFSerializerSerdDummySymbol;
#endif
