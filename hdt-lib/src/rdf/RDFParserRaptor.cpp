/*
 * RDFParserRaptor.cpp
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifdef USE_RAPTOR
#include "RDFParserRaptor.hpp"
#include "../util/fileUtil.hpp"

namespace hdt {

void raptor_process_triple(void *user_data, raptor_statement *triple) {
	 //raptor_statement_print_as_ntriples(triple, stdout);

	const char *s = (const char*)raptor_term_to_string(triple->subject);
	const char *p = (const char*)raptor_term_to_string(triple->predicate);
	const char *o = (const char*)raptor_term_to_string(triple->object);

	TripleString ts(s, p, o);

	delete s;
	delete p;
	delete o;
	//cout << "***" << ts << endl;

	RDFParserRaptor *raptorParser = reinterpret_cast<RDFParserRaptor *>(user_data);
	raptorParser->vectorOutput.push_back(ts);

#if 0
	raptor_locator *locator = raptor_parser_get_locator(raptorParser->rdf_parser);
	raptorParser->size = raptor_locator_byte(locator);
#endif
}

void raptor_log_handler(void *user_data, raptor_log_message *message) {
	RDFParserRaptor *raptorParser = reinterpret_cast<RDFParserRaptor *>(user_data);

	cout << "LOG: " << message->code << " => " << message->text << endl;
    throw message->text;
}

RDFParserRaptor::RDFParserRaptor(std::istream &in, RDFNotation notation)
	: RDFParser(notation),
	  pos(0)
{
	size = fileUtil::getSize(in);

	buf.resize(2048, '\0');

	raptor_world *world = raptor_new_world();
	raptor_world_set_log_handler(world, (void *)this, raptor_log_handler);

	raptor_uri *base_uri = raptor_new_uri(world, (const unsigned char*)"http://www.rdfhdt.org/");
	rdf_parser = raptor_new_parser(world, getParserType(notation));

	raptor_parser_set_statement_handler(rdf_parser, (void *)this, raptor_process_triple);

	raptor_parser_parse_start(rdf_parser, base_uri);

	while(!in.eof()) {
		//cout << "Buffer pos: " << in.tellg() << endl;
		in.read((char *)&buf[0], buf.size() );
		raptor_parser_parse_chunk(rdf_parser, (const unsigned char *)&buf[0], buf.size(), 0);
	}
	raptor_parser_parse_chunk(rdf_parser, NULL, 0, 1);

        raptor_free_parser(rdf_parser);
        raptor_free_uri(base_uri);
        raptor_free_world(world);
}

RDFParserRaptor::RDFParserRaptor(const char *fileName, RDFNotation notation) : RDFParser(notation), pos(0) {
	raptor_world *world = raptor_new_world();
	raptor_world_set_log_handler(world, (void *)this, raptor_log_handler);

	rdf_parser = raptor_new_parser(world, getParserType(notation));
	raptor_parser_set_statement_handler(rdf_parser, (void *)this, raptor_process_triple);

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

RDFParserRaptor::~RDFParserRaptor() {

}

const char *RDFParserRaptor::getParserType(RDFNotation notation){
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

bool RDFParserRaptor::hasNext() {
	return pos<vectorOutput.size();
}

TripleString *RDFParserRaptor::next() {
	return &vectorOutput[pos++];
}

void RDFParserRaptor::reset() {
	pos = 0;
}

uint64_t RDFParserRaptor::getPos(){
	return pos;
}

uint64_t RDFParserRaptor::getSize() {
        return vectorOutput.size();
}

}
#endif 
