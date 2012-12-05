/*
 * RDFParserRaptorCallback.cpp
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifdef USE_RAPTOR
#include "RDFParserRaptorCallback.hpp"
#include "../util/fileUtil.hpp"

namespace hdt {

string getString(raptor_term *term) {
	string out;

	if(term->type==RAPTOR_TERM_TYPE_URI) {
		out.append((char *)raptor_uri_as_string(term->value.uri));
	} else if(term->type==RAPTOR_TERM_TYPE_LITERAL) {
		out.append("\"");
		out.append((char *)term->value.literal.string);
		if(term->value.literal.language_len>0){
			out.append("\"@");
			out.append((char *)term->value.literal.language);
		} else {
			out.append("\"");
		}
		if(term->value.literal.datatype) {
			out.append("^^");
			out.append((char *)raptor_uri_as_string(term->value.literal.datatype));
		}
	} else if(term->type==RAPTOR_TERM_TYPE_BLANK) {
		out.append((char *)term->value.blank.string);
	}
	//cout << out << endl;
	return out;
}

void raptor_callback_process_triple(void *user_data, raptor_statement *triple) {
	 //raptor_statement_print_as_ntriples(triple, stdout);

	TripleString ts( getString(triple->subject), getString(triple->predicate), getString(triple->object));

	RDFParserRaptorCallback *raptorParser = reinterpret_cast<RDFParserRaptorCallback *>(user_data);
	raptorParser->callback->processTriple(ts, 0);
}

void raptor_callback_log_handler(void *user_data, raptor_log_message *message) {
	RDFParserRaptorCallback *raptorParser = reinterpret_cast<RDFParserRaptorCallback *>(user_data);

	if(message!=NULL) {
#if 0
		if(message->level>=RAPTOR_LOG_LEVEL_ERROR) {
			size_t len = strlen(message->text)+1;
			raptorParser->error = new char[len];
			strncpy(raptorParser->error, message->text, len);
			raptor_parser_parse_abort(raptorParser->rdf_parser);
		}
#endif

		cerr << "Parser message: => " << message->text;

		if(message->locator) {
			int line = raptor_locator_line(message->locator);
			int column = raptor_locator_column(message->locator);
			int byte = raptor_locator_byte(message->locator);
			if(line>=0) {
				cerr << " at line " << 	line;
			}
			if(column>=0) {
				cerr << " at column " << column;
			}
			if(byte>=0) {
				cerr << " at byte " << byte;
			}
			cerr << endl;
		}
	}
}

RDFParserRaptorCallback::RDFParserRaptorCallback()
{
}

RDFParserRaptorCallback::~RDFParserRaptorCallback() {

}

const char *RDFParserRaptorCallback::getParserType(RDFNotation notation){
	switch(notation){
	case NQUAD:
		return "nquads";
	case N3:
		return "n3";
	case NTRIPLES:
		return "ntriples";
	case TURTLE:
		return "turtle";
	case XML:
		return "rdfxml";
	default:
		return "ntriples";
	}
}

void RDFParserRaptorCallback::doParse(const char *fileName, const char *baseUri, RDFNotation notation, RDFCallback *callback) {
	if(callback==NULL) {
		return;
	}
	this->callback = callback;

	raptor_world *world = raptor_new_world();
	raptor_world_set_log_handler(world, (void *)this, raptor_callback_log_handler);

	rdf_parser = raptor_new_parser(world, getParserType(notation));
	raptor_parser_set_statement_handler(rdf_parser, (void *)this, raptor_callback_process_triple);

	string file(fileName);
	raptor_uri *fileUri;

	raptor_uri *base_uri = raptor_new_uri(world, (const unsigned char *)baseUri);
	error = NULL;

	if(file.substr(0,7)=="http://") {
		fileUri = raptor_new_uri(world, (const unsigned char *) fileName);
		raptor_parser_parse_uri(rdf_parser, fileUri, base_uri);
	} else {
		unsigned char *fileUri_string = raptor_uri_filename_to_uri_string(fileName);
		fileUri = raptor_new_uri(world, fileUri_string);
		raptor_free_memory(fileUri_string);

		raptor_parser_parse_file(rdf_parser, fileUri, base_uri);
	}

	raptor_free_parser(rdf_parser);
	raptor_free_uri(base_uri);
	raptor_free_uri(fileUri);

	raptor_free_world(world);

	if(error) {
		throw error;
	}
}

}
#endif 
