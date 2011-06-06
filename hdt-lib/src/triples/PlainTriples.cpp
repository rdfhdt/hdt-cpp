/*
 * PlainTriples.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDTVocabulary.hpp>

#include "PlainTriples.hpp"
#include "TripleOrderConvert.hpp"

namespace hdt {

PlainTriples::PlainTriples() : order(Unknown) {
	streamX = StreamElements::getStream(spec.get("stream.x"));
	streamY = StreamElements::getStream(spec.get("stream.y"));
	streamZ = StreamElements::getStream(spec.get("stream.z"));
}

PlainTriples::PlainTriples(HDTSpecification &specification) : spec(specification) {
	streamX = StreamElements::getStream(spec.get("stream.x"));
	streamY = StreamElements::getStream(spec.get("stream.y"));
	streamZ = StreamElements::getStream(spec.get("stream.z"));

	std::string orderStr = spec.get("triples.component.order");
	order = parseOrder(orderStr.c_str());
}

PlainTriples::~PlainTriples() {
	delete streamX;
	delete streamY;
	delete streamZ;
}

float PlainTriples::cost(TripleID & triple)
{
}

void PlainTriples::load(ModifiableTriples &triples) {
	TripleID all(0,0,0);

	IteratorTripleID *itS = triples.search(all);
	ComponentIterator subjIt(itS, SUBJECT);
	streamX->add(subjIt);
	delete itS;

	IteratorTripleID *itP = triples.search(all);
	ComponentIterator predIt(itS, PREDICATE);
	streamY->add(predIt);
	delete itP;

	IteratorTripleID *itO = triples.search(all);
	ComponentIterator objIt(itS, OBJECT);
	streamZ->add(objIt);
	delete itO;
}

void PlainTriples::populateHeader(Header &header, string rootNode) {
	header.insert(rootNode, HDTVocabulary::TRIPLES_TYPE, HDTVocabulary::TRIPLES_TYPE_PLAIN);
	header.insert(rootNode, HDTVocabulary::TRIPLES_NUM_TRIPLES, getNumberOfElements() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_ORDER, order );  // TODO: Convert to String
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMX_TYPE, streamX->getType() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMY_TYPE, streamY->getType() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMZ_TYPE, streamZ->getType() );
}

IteratorTripleID *PlainTriples::search(TripleID & pattern)
{
	return new PlainTriplesIterator(this, pattern);
}

bool PlainTriples::save(std::ostream & output, ControlInformation &controlInformation)
{
	controlInformation.clear();
	controlInformation.setUint("numTriples", getNumberOfElements());
	controlInformation.set("codification", HDTVocabulary::TRIPLES_TYPE_PLAIN);
	controlInformation.setUint("triples.component.order", order);
	controlInformation.set("stream.x", streamX->getType());
	controlInformation.set("stream.y", streamY->getType());
	controlInformation.set("stream.z", streamZ->getType());
	controlInformation.save(output);

	streamX->save(output);
	streamY->save(output);
	streamZ->save(output);
}

void PlainTriples::load(std::istream &input, ControlInformation &controlInformation)
{
	unsigned int numTriples = controlInformation.getUint("numTriples");
	order = (TripleComponentOrder) controlInformation.getUint("triples.component.order");

	std::string typeSubjects = controlInformation.get("stream.x");
	std::string typePredicates = controlInformation.get("stream.y");
	std::string typeObjects = controlInformation.get("stream.z");

	delete streamX;
	delete streamY;
	delete streamZ;

	streamX = StreamElements::getStream(typeSubjects);
	streamY = StreamElements::getStream(typePredicates);
	streamZ = StreamElements::getStream(typeObjects);

	streamX->load(input);
	streamY->load(input);
	streamZ->load(input);
}

unsigned int PlainTriples::getNumberOfElements()
{
	return streamX->getNumberOfElements();
}

unsigned int PlainTriples::size()
{
	return streamX->size()+streamY->size()+streamZ->size();
}

TripleID PlainTriples::getTripleID(unsigned int pos) {
	TripleID triple(streamX->get(pos), streamY->get(pos), streamZ->get(pos));
	return triple;
}


/// ITERATOR

PlainTriplesIterator::PlainTriplesIterator(PlainTriples *pt, TripleID &pat) : triples(pt), pos(0), pattern(pat) {
	doFetch();
}

PlainTriplesIterator::~PlainTriplesIterator() {

}

void PlainTriplesIterator::doFetch() {
	do {
		nextv = triples->getTripleID(pos);
		pos++;
	} while(pos<=triples->getNumberOfElements() && (!nextv.isValid() || !nextv.match(pattern)));

	cout << nextv << endl;

	hasNextv= pos<=triples->getNumberOfElements();
}

bool PlainTriplesIterator::hasNext() {
	return hasNextv;
}

TripleID PlainTriplesIterator::next() {
	TripleID ret = nextv;
	doFetch();
	return ret;
}


ComponentIterator::ComponentIterator(IteratorTripleID *iterator, TripleComponentRole component) : role(component), it(iterator)
{
}

unsigned int ComponentIterator::next()
{
	TripleID triple = it->next();

	switch(role){
	case SUBJECT:
		return triple.getSubject();
	case PREDICATE:
		return triple.getPredicate();
	case OBJECT:
		return triple.getObject();
	}
}

bool ComponentIterator::hasNext()
{
	it->hasNext();
}


}



