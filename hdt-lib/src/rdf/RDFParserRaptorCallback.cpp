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

void raptor_callback_process_triple(void *user_data, raptor_statement *triple) {
	 //raptor_statement_print_as_ntriples(triple, stdout);

	const char *s = (const char*)raptor_term_to_string(triple->subject);
	const char *p = (const char*)raptor_term_to_string(triple->predicate);
	const char *o = (const char*)raptor_term_to_string(triple->object);

	TripleString ts(s, p, o);

	raptor_free_memory((void*)s);
	raptor_free_memory((void*)p);
	raptor_free_memory((void*)o);
	//cout << "***" << ts << endl;

	RDFParserRaptorCallback *raptorParser = reinterpret_cast<RDFParserRaptorCallback *>(user_data);

	raptor_locator *locator = raptor_parser_get_locator(raptorParser->rdf_parser);
	//cout << "RaptorCallback Pos: " << raptor_locator_byte(locator) << endl;

	int pos = raptor_locator_byte(locator);
	raptorParser->callback->processTriple(ts, pos==-1 ? 0 : pos);
}

void raptor_callback_log_handler(void *user_data, raptor_log_message *message) {
	RDFParserRaptorCallback *raptorParser = reinterpret_cast<RDFParserRaptorCallback *>(user_data);

	cout << "Parser message: => " << message->text << " at " << message->locator->line << endl;
	if(message->level>=RAPTOR_LOG_LEVEL_ERROR) {
		throw message->text;
	}
}

RDFParserRaptorCallback::RDFParserRaptorCallback()
{
}

RDFParserRaptorCallback::~RDFParserRaptorCallback() {

}

const char *RDFParserRaptorCallback::getParserType(RDFNotation notation){
	switch(notation){
	case N3:
		return "n3";
	case NTRIPLES:
		return "ntriples";
	case TURTLE:
		return "turtle";
	case XML:
		return "rdfxml";
	}
}

void RDFParserRaptorCallback::doParse(const char *fileName, RDFNotation notation, RDFCallback *callback) {
	this->callback = callback;

	raptor_world *world = raptor_new_world();
	raptor_world_set_log_handler(world, (void *)this, raptor_callback_log_handler);

	rdf_parser = raptor_new_parser(world, getParserType(notation));
	raptor_parser_set_statement_handler(rdf_parser, (void *)this, raptor_callback_process_triple);

	string file(fileName);
	raptor_uri *fileUri;
	if(file.substr(0,7)=="http://") {
		fileUri = raptor_new_uri(world, (const unsigned char *) fileName);
	} else {
		unsigned char *fileUri_string = raptor_uri_filename_to_uri_string(fileName);
		fileUri = raptor_new_uri(world, fileUri_string);
		raptor_free_memory(fileUri_string);
	}

	raptor_uri *base_uri = raptor_uri_copy(fileUri);

	raptor_parser_parse_uri(rdf_parser, fileUri, base_uri);

	raptor_free_parser(rdf_parser);
	raptor_free_uri(base_uri);
	raptor_free_uri(fileUri);

	raptor_free_world(world);
}

}
#endif 
