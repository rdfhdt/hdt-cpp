/*
 * RDFParserRaptorLine.cpp
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifdef USE_RAPTOR
#include "RDFParserRaptorLine.hpp"

namespace hdt {

void raptor_line_process_triple(void *user_data, raptor_statement *triple) {
	//raptor_statement_print_as_ntriples(triple, stdout);

	const char *s = (const char*)raptor_term_to_string(triple->subject);
	const char *p = (const char*)raptor_term_to_string(triple->predicate);
	const char *o = (const char*)raptor_term_to_string(triple->object);

	TripleString ts(s, p, o);

	delete s;
	delete p;
	delete o;
	//cout << "***" << ts << endl;

	RDFParserRaptorLine *raptorParser = reinterpret_cast<RDFParserRaptorLine *>(user_data);
	raptorParser->vectorOutput.push_back(ts);

	//	cout << "Triples appended: " << raptorParser->vectorOutput.size() << endl;
}

RDFParserRaptorLine::RDFParserRaptorLine(std::istream &in, RDFNotation notation) :
		RDFParser(in, notation), pos(0), globalLine(0) {
	world = raptor_new_world();
	base_uri = raptor_new_uri(world, (const unsigned char*)"http://www.rdfhdt.org/");
	rdf_parser = raptor_new_parser(world, getParserType(notation));

	raptor_parser_set_statement_handler(rdf_parser, (void *)this, raptor_line_process_triple);

	readBlock();
}


void RDFParserRaptorLine::readBlock() {
	vectorOutput.clear();
	pos = 0;

	raptor_parser_parse_start(rdf_parser, base_uri);

	string line;
	unsigned int lineCount = 0;
	while(!input.eof() && lineCount < 1000) {
		getline(input, line);
		raptor_parser_parse_chunk(rdf_parser, (const unsigned char *)line.c_str(), line.length(), 0);
		lineCount++;
		globalLine++;
	}

	raptor_parser_parse_chunk(rdf_parser, NULL, 0, 1);
}

RDFParserRaptorLine::~RDFParserRaptorLine() {
	raptor_free_parser(rdf_parser);
	raptor_free_uri(base_uri);
	raptor_free_world(world);
}

const char *RDFParserRaptorLine::getParserType(RDFNotation notation){
	switch(notation){
	case N3:
		return "n3";
	case NTRIPLES:
		return "n3";
	case TURTLE:
		return "turtle";
	case XML:
		return "rdfxml";
	}
}

bool RDFParserRaptorLine::hasNext() {
	return !input.eof() || pos<vectorOutput.size();
}

TripleString *RDFParserRaptorLine::next() {
	ts = vectorOutput[pos++];

	if(pos==vectorOutput.size() && !input.eof()) {
		readBlock();
	}
	return &ts;
}

void RDFParserRaptorLine::reset() {
	pos = 0;
	vectorOutput.clear();
	input.clear(); // Resets EOF
	input.seekg(0, std::ios::beg);
	readBlock();
}

}
#endif 
