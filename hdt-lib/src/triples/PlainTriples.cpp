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

PlainTriples::PlainTriples() : order(SPO) {
	streamX = StreamElements::getStream(spec.get("stream.x"));
	streamY = StreamElements::getStream(spec.get("stream.y"));
	streamZ = StreamElements::getStream(spec.get("stream.z"));
}

PlainTriples::PlainTriples(HDTSpecification &specification) : spec(specification) {
	std::string orderStr = spec.get("triples.component.order");
	order = parseOrder(orderStr.c_str());
	if(order==Unknown) {
			order = SPO;
	}
	streamX = StreamElements::getStream(spec.get("stream.x"));
	streamY = StreamElements::getStream(spec.get("stream.y"));
	streamZ = StreamElements::getStream(spec.get("stream.z"));
}

PlainTriples::~PlainTriples() {
	delete streamX;
	delete streamY;
	delete streamZ;
}

float PlainTriples::cost(TripleID & triple)
{
}

void PlainTriples::load(ModifiableTriples &triples, ProgressListener *listener) {
	triples.sort(order);

	IteratorTripleID *itS = triples.searchAll();
	ComponentIterator subjIt(itS, SUBJECT);
	streamX->add(subjIt);
	delete itS;

	IteratorTripleID *itP = triples.searchAll();
	ComponentIterator predIt(itS, PREDICATE);
	streamY->add(predIt);
	delete itP;

	IteratorTripleID *itO = triples.searchAll();
	ComponentIterator objIt(itS, OBJECT);
	streamZ->add(objIt);
	delete itO;
}

void PlainTriples::populateHeader(Header &header, string rootNode) {
	header.insert(rootNode, HDTVocabulary::TRIPLES_TYPE, HDTVocabulary::TRIPLES_TYPE_PLAIN);
	header.insert(rootNode, HDTVocabulary::TRIPLES_NUM_TRIPLES, getNumberOfElements() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_ORDER, getOrderStr(order) );
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMX_TYPE, streamX->getType() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMY_TYPE, streamY->getType() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMZ_TYPE, streamZ->getType() );
}

IteratorTripleID *PlainTriples::search(TripleID & pattern)
{
	if(pattern.isEmpty()) {
		return new PlainTriplesIterator(this, pattern, order);
	} else {
		return new SequentialSearchIteratorTripleID(pattern, new PlainTriplesIterator(this,pattern, order));
	}
}

bool PlainTriples::save(std::ostream & output, ControlInformation &controlInformation, ProgressListener *listener)
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

void PlainTriples::load(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener)
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

string PlainTriples::getType() {
	return HDTVocabulary::TRIPLES_TYPE_PLAIN;
}

PlainTriplesIterator::PlainTriplesIterator(PlainTriples *triples, TripleID & pattern, TripleComponentOrder order) :
		triples(triples), pattern(pattern), pos(0), order(order)
{
}

void PlainTriplesIterator::updateOutput()
{
	returnTriple.setAll(triples->streamX->get(pos), triples->streamY->get(pos), triples->streamZ->get(pos));
}

bool PlainTriplesIterator::hasNext()
{
	return pos<triples->getNumberOfElements();
}

TripleID *PlainTriplesIterator::next()
{
	updateOutput();
	pos++;
	return &returnTriple;
}

bool PlainTriplesIterator::hasPrevious()
{
	return pos>0;
}

TripleID *PlainTriplesIterator::previous()
{
	pos--;
	updateOutput();
	return &returnTriple;
}

void PlainTriplesIterator::goToStart()
{
	pos=0;
}





ComponentIterator::ComponentIterator(IteratorTripleID *iterator, TripleComponentRole component) : role(component), it(iterator)
{
}

unsigned int ComponentIterator::next()
{
	TripleID *triple = it->next();

	switch(role){
	case SUBJECT:
		return triple->getSubject();
	case PREDICATE:
		return triple->getPredicate();
	case OBJECT:
		return triple->getObject();
	}
}

bool ComponentIterator::hasNext()
{
	it->hasNext();
}


}



