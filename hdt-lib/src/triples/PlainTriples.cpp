/*
 * PlainTriples.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include "PlainTriples.hpp"

namespace hdt {

PlainTriples::PlainTriples() {
	// TODO Auto-generated constructor stub

}

PlainTriples::~PlainTriples() {
	// TODO Auto-generated destructor stub
}

float PlainTriples::cost(TripleID & triple)
{
}

bool PlainTriples::insert(TripleID & triple)
{
}

void PlainTriples::load(std::istream & input, Header &header)
{
}

void PlainTriples::populateHeader(Header &header) {

}

IteratorTripleID PlainTriples::search(TripleID & triple)
{
}

bool PlainTriples::save(std::ostream & output)
{
}

unsigned int PlainTriples::getNumberOfElements()
{
}

unsigned int PlainTriples::size()
{
}

bool PlainTriples::insert(IteratorTripleID & triples)
{
}

void PlainTriples::startProcessing(){

}

void PlainTriples::stopProcessing(){

}

}
