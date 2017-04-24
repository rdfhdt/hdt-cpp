
#ifdef HAVE_SERD


#include "../util/fileUtil.hpp"
#include "RDFParserSerd.hpp"
#include "RDFParser.hpp"

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
        out.append("^^<");
        out.append((char *)dataType->buf);
        out.append(">");
    }

    //cout << out << endl;
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
    raptorParser->callback->processTriple(ts, raptorParser->numByte);

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

#ifdef HAVE_LIBZ

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

const size_t CHUNK=64*1024; 

typedef struct {
	z_stream strm;
	FILE *file;
	uint8_t *inBuffer;
        uint64_t totalRead;
} LibzSerdData;

void libz_init(LibzSerdData *d){
       d->inBuffer = new unsigned char[CHUNK];

        d->strm.zalloc = Z_NULL;
        d->strm.zfree = Z_NULL;
        d->strm.opaque = Z_NULL;
        d->strm.avail_in = 0;
        d->strm.next_in = d->inBuffer;

        d->totalRead = 0;
	int ret = inflateInit2(&d->strm, 16+MAX_WBITS);
        if (ret != Z_OK) {
		// FIXME: Error
		return;
        }
}

void libz_destroy(LibzSerdData *d){
        (void)inflateEnd(&d->strm);

        delete[] d->inBuffer;
}

int libz_serd_error(void *stream) {
	// Fixme: Notify Error
	return SERD_SUCCESS;
}

size_t libz_serd_read(void *buf, size_t size, size_t nmemb, void *stream){
	const size_t bufSize = (nmemb*size);
	uint8_t *buffer = reinterpret_cast<uint8_t *>(buf);
	LibzSerdData *d = reinterpret_cast<LibzSerdData *>(stream);

	d->strm.next_out = buffer;
	d->strm.avail_out = bufSize;

	do {
		// Fill in buffer from file if empty
		if(d->strm.avail_in == 0) {
			size_t readCount = fread(d->inBuffer, 1, CHUNK, d->file);

			if(readCount == 0){
				// FIXME: Could not read, manage error
				return 0;
			}
			d->strm.avail_in = readCount;
			d->strm.next_in = d->inBuffer;
			d->totalRead += readCount;
		}

		// Call inflate to consume in and fill out
		int ret = inflate(&d->strm, Z_NO_FLUSH);

		size_t have;
		switch (ret) {
		case Z_NEED_DICT:
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			cerr << "zlib error code: " << ret << " => "<< d->strm.msg << endl;
			// FIXME: Handle error
			return 0;
		case Z_STREAM_END:
			// Finished reading stream.
			have = bufSize-d->strm.avail_out;
			return have;
		case Z_OK:
			have = bufSize-d->strm.avail_out;
			break;
		default:
			return 0;
		}
	} while (d->strm.avail_out>0);

	size_t numRead = bufSize-d->strm.avail_out;
	return numRead;
}

#endif


RDFParserSerd::RDFParserSerd() : numByte(0)
{
}

RDFParserSerd::~RDFParserSerd() {

}

SerdSyntax RDFParserSerd::getParserType(RDFNotation notation){
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
                (SerdEndSink)hdtserd_end);

    serd_reader_set_error_sink(reader, hdtserd_error, NULL);

    const uint8_t* input=serd_uri_to_path((const uint8_t *)fileName);
    FILE *in_fd = fopen((const char*)input, "r");
    // TODO: fadvise sequential
    if(in_fd==NULL) {
        throw ParseException("Could not open input file for parsing");
    }

    if(fileUtil::str_ends_with(fileName,".gz")){

// NOTE: Requires serd 0.27.1
#ifdef HAVE_LIBZ
	LibzSerdData libzSerdData;
	libzSerdData.file = in_fd;
	libz_init(&libzSerdData);

	serd_reader_read_source(reader, libz_serd_read, libz_serd_error, &libzSerdData, input, 4096); 

	libz_destroy(&libzSerdData);
#else
	cerr << "HDT Library has not been compiled with gzip support." << end;
#endif

    } else {
	serd_reader_read_file_handle(reader, in_fd, (const uint8_t *)fileName);
    }


    fclose(in_fd);
    serd_reader_free(reader);

    serd_env_free(env);
    serd_node_free(&base);
}

}
#else
int RDFParserSerdDummySymbol;
#endif
