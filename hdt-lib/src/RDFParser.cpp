/*
 * RDFParser.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include "RDFParser.hpp"

namespace hdt {

RDFParser::RDFParser(std::istream *in) {
	this->in = in;
}

RDFParser::~RDFParser() {
	// TODO Auto-generated destructor stub
}

bool RDFParser::hasNext() {
	return getline(*in, line);
}

TripleString RDFParser::next() {
	TripleString ts;
	ts.readN3Line(line);

	return ts;
}

}
