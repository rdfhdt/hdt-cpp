/*
 * BasicHeader.cpp
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#include <HDTFactory.hpp>

#include "BasicHeader.hpp"

namespace hdt {

BasicHeader::BasicHeader() {
	// FIXME: Specify best HDT types for header using spec.

	spec.set("noheader", "true");

	hdt = HDTFactory::createBasicHDT(spec);
}

BasicHeader::~BasicHeader() {
	delete hdt;
}

void BasicHeader::load(std::istream & input)
{
	hdt->loadFromRDF(input);
}

bool BasicHeader::save(std::ostream & output)
{
	hdt->saveToRDF(output, NTRIPLE);
}

void BasicHeader::insert(TripleString & triple)
{
	hdt->insert(triple);
}

void BasicHeader::remove(TripleString & triple)
{
	hdt->remove(triple);
}

void BasicHeader::insert(IteratorTripleString *triples)
{
	hdt->insert(triples);
}

void BasicHeader::remove(IteratorTripleString *triples)
{
	hdt->remove(triples);
}

IteratorTripleString *BasicHeader::search(const char *subject, const char *predicate, const char *object)
{
	return hdt->search(subject, predicate, object);
}

bool BasicHeader::edit(TripleString & oldTriple, TripleString & newTriple)
{
	return hdt->edit(oldTriple, newTriple);
}

}
