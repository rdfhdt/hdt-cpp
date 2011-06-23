/*
 * RDFParserRaptor.cpp
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifdef USE_RAPTOR
#include "RDFParserRaptor.hpp"

namespace hdt {

void process_triple(void *user_data, raptor_statement *triple) {
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

//	cout << "Triples appended: " << raptorParser->vectorOutput.size() << endl;
}

RDFParserRaptor::RDFParserRaptor(std::istream &in, RDFNotation notation) : RDFParser(in), notation(notation), pos(0) {
	buf.resize(2048, '\0');

	world = raptor_new_world();
	base_uri = raptor_new_uri(world, (const unsigned char*)"http://www.rdfhdt.org/");
	rdf_parser = raptor_new_parser(world, getParserType(notation));

	raptor_parser_set_statement_handler(rdf_parser, (void *)this, process_triple);

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

RDFParserRaptor::~RDFParserRaptor() {

}

const char *RDFParserRaptor::getParserType(RDFNotation notation){
	switch(notation){
	case N3:
		return "n3";
	case NTRIPLE:
		return "ntriple";
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

}
#endif 
