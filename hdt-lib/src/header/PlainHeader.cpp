/*
 * PlainHeader.cpp
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#include <HDTVocabulary.hpp>
#include <HDTFactory.hpp>
#include "../RDFParserN3.hpp"

#include "PlainHeader.hpp"

namespace hdt {

PlainHeader::PlainHeader() {
}

PlainHeader::PlainHeader(HDTSpecification &specification) : spec(specification) {
}

PlainHeader::~PlainHeader() {
}

void PlainHeader::load(std::istream & input, ControlInformation &controlInformation)
{
	std::string codification = controlInformation.get("codification");
	if(codification != HDTVocabulary::HEADER_PLAIN) {
		throw "Unexpected PlainHeader format";
	}

	RDFParserN3 parser(input);

	while(parser.hasNext()) {
		TripleString ts = parser.next();
		triples.push_back(ts);
	}
}

bool PlainHeader::save(std::ostream & output, ControlInformation &controlInformation)
{
	controlInformation.clear();
	controlInformation.set("codification", HDTVocabulary::HEADER_PLAIN);
	controlInformation.save(output);

	for(vector<TripleString>::iterator it = triples.begin(); it!=triples.end(); it++){
		output << *it << " ." << endl;
	}

	output << endl;
}

unsigned int PlainHeader::getNumberOfElements() {
	return triples.size();
}

void PlainHeader::insert(TripleString & triple)
{
	triples.push_back(triple);
}

void PlainHeader::remove(TripleString & triple)
{
	throw "Not implemented";
}

void PlainHeader::insert(IteratorTripleString *tripit)
{
	while(tripit->hasNext()){
		TripleString next = tripit->next();
		triples.push_back(next);
	}

}

void PlainHeader::remove(IteratorTripleString *triples)
{
	throw "Not implemented";
}

IteratorTripleString *PlainHeader::search(const char *subject, const char *predicate, const char *object)
{
	throw "Not implemented";
}

}
