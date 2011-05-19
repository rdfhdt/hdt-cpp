/*
 * PlainTriples.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include "PlainTriples.hpp"
#include "TripleOrderConvert.hpp"

namespace hdt {

PlainTriples::PlainTriples() : order(Unknown) {
	subjects = StreamElements::getStream(spec.get("stream.x"));
	predicates = StreamElements::getStream(spec.get("stream.y"));
	objects = StreamElements::getStream(spec.get("stream.z"));
}

PlainTriples::PlainTriples(HDTSpecification &specification) : spec(specification) {
	subjects = StreamElements::getStream(spec.get("stream.x"));
	predicates = StreamElements::getStream(spec.get("stream.y"));
	objects = StreamElements::getStream(spec.get("stream.z"));

	std::string orderStr = spec.get("triples.component.order");
	order = parseOrder(orderStr.c_str());
}

PlainTriples::~PlainTriples() {
	delete subjects;
	delete predicates;
	delete objects;
}

float PlainTriples::cost(TripleID & triple)
{
}

void PlainTriples::load(ModifiableTriples &triples) {
	TripleID all(0,0,0);

	IteratorTripleID *itS = triples.search(all);
	ComponentIterator subjIt(itS, SUBJECT);
	subjects->add(subjIt);
	delete itS;

	IteratorTripleID *itP = triples.search(all);
	ComponentIterator predIt(itS, PREDICATE);
	predicates->add(predIt);
	delete itP;

	IteratorTripleID *itO = triples.search(all);
	ComponentIterator objIt(itS, OBJECT);
	objects->add(objIt);
	delete itO;
}

void PlainTriples::populateHeader(Header &header) {

}

IteratorTripleID *PlainTriples::search(TripleID & pattern)
{
	return new PlainTriplesIterator(this, pattern);
}

bool PlainTriples::save(std::ostream & output, ControlInformation &controlInformation)
{
	controlInformation.clear();
	controlInformation.setUint("numTriples", getNumberOfElements());
	controlInformation.set("codification", "http://purl.org/HDT/hdt#triplesPlain");
	controlInformation.setUint("triples.component.order", order);
	controlInformation.set("stream.x", subjects->getType());
	controlInformation.set("stream.y", predicates->getType());
	controlInformation.set("stream.z", objects->getType());
	controlInformation.save(output);

	subjects->save(output);
	predicates->save(output);
	objects->save(output);
}

void PlainTriples::load(std::istream &input, ControlInformation &controlInformation)
{
	unsigned int numTriples = controlInformation.getUint("numTriples");
	order = (TripleComponentOrder) controlInformation.getUint("triples.component.order");

	std::string typeSubjects = controlInformation.get("stream.x");
	std::string typePredicates = controlInformation.get("stream.y");
	std::string typeObjects = controlInformation.get("stream.z");

	delete subjects;
	delete predicates;
	delete objects;

	subjects = StreamElements::getStream(typeSubjects);
	predicates = StreamElements::getStream(typePredicates);
	objects = StreamElements::getStream(typeObjects);

	subjects->load(input);
	predicates->load(input);
	objects->load(input);
}

unsigned int PlainTriples::getNumberOfElements()
{
	return subjects->getNumberOfElements();
}

unsigned int PlainTriples::size()
{
	return subjects->size()+predicates->size()+objects->size();
}

TripleID PlainTriples::getTripleID(unsigned int pos) {
	TripleID triple(subjects->get(pos), predicates->get(pos), objects->get(pos));
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
		//cout << nextv << endl;
		pos++;
	} while(pos<=triples->getNumberOfElements() && (!nextv.isValid() || !nextv.match(pattern)));

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



