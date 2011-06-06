/*
 * CompactTriples.cpp
 *
 *  Created on: 11/05/2011
 *      Author: mck
 */

#include <HDTVocabulary.hpp>

#include "CompactTriples.hpp"
#include "TripleOrderConvert.hpp"

namespace hdt {


CompactTriples::CompactTriples() : numTriples(0), order(SPO) {
	streamY = StreamElements::getStream(spec.get("stream.y"));
	streamZ = StreamElements::getStream(spec.get("stream.z"));
}

CompactTriples::CompactTriples(HDTSpecification &specification) : numTriples(0), spec(specification) {
	std::string orderStr = spec.get("triples.component.order");
	order= parseOrder(orderStr.c_str());
	if(order==Unknown)
		order = SPO;

	streamY = StreamElements::getStream(spec.get("stream.y"));
	streamZ = StreamElements::getStream(spec.get("stream.z"));
}

CompactTriples::~CompactTriples() {
	delete streamY;
	delete streamZ;
}


float CompactTriples::cost(TripleID & triple)
{
}



void CompactTriples::load(ModifiableTriples &triples) {
	TripleID all(0,0,0);

	IteratorTripleID *it = triples.search(all);

	vector<unsigned int> vectorY, vectorZ;
	unsigned int lastX, lastY, lastZ;
	unsigned int x, y, z;

	// First triple
	if(it->hasNext()) {
		TripleID triple = it->next();
		//cout << "111> " << triple << endl;

		UnorderedTriple *ut = reinterpret_cast<UnorderedTriple *>(&triple);
		swapComponentOrder(ut, SPO, order);
		//cout << "222> " << triple << endl;

		lastX = x = ut->x;
		lastY = y = ut->y;
		lastZ = z = ut->z;

		vectorY.push_back(y);
		vectorZ.push_back(z);

		numTriples++;
	}

	// Rest of the triples
	while(it->hasNext()) {
		TripleID triple = it->next();
		//cout << "111> " << triple << endl;

		UnorderedTriple *ut = reinterpret_cast<UnorderedTriple *>(&triple);
		swapComponentOrder(ut, SPO, order);
		//cout << "222> " << triple << endl;

		x = ut->x;
		y = ut->y;
		z = ut->z;

		if(x!=lastX) {
			vectorY.push_back(0);
			vectorY.push_back(y);

			vectorZ.push_back(0);
			vectorZ.push_back(z);
		} else if(y!=lastY) {
			vectorY.push_back(y);
			vectorZ.push_back(0);
			vectorZ.push_back(z);
		} else {
			vectorZ.push_back(z);
		}

		lastX = x;
		lastY = y;
		lastZ = z;

		numTriples++;
	}

	delete it;

	VectorIterator itY(vectorY);
	VectorIterator itZ(vectorZ);

	streamY->add(itY);
	streamZ->add(itZ);

#if 0
	// Debug Adjacency Lists
	cout << "Y" << vectorY.size() << "): ";
	for(unsigned int i=0;i<streamY->getNumberOfElements();i++){
		cout << streamY->get(i) << " ";
	}
	cout << endl;

	cout << "Z" << vectorZ.size() << "): ";
	for(unsigned int i=0;i<streamZ->getNumberOfElements();i++){
		cout << streamZ->get(i) << " ";
	}
	cout << endl;
#endif

}

void CompactTriples::populateHeader(Header &header, string rootNode) {
	header.insert(rootNode, HDTVocabulary::TRIPLES_TYPE, HDTVocabulary::TRIPLES_TYPE_COMPACT);
	header.insert(rootNode, HDTVocabulary::TRIPLES_NUM_TRIPLES, getNumberOfElements() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_ORDER, order );  // TODO: Convert to String
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMY_TYPE, streamY->getType() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMZ_TYPE, streamZ->getType() );
}

IteratorTripleID *CompactTriples::search(TripleID & pattern)
{
	return new CompactTriplesIterator(this, pattern);
}

bool CompactTriples::save(std::ostream & output, ControlInformation &controlInformation)
{
	controlInformation.clear();
	controlInformation.setUint("numTriples", getNumberOfElements());
	controlInformation.set("codification", HDTVocabulary::TRIPLES_TYPE_COMPACT);
	controlInformation.setUint("triples.component.order", order);
	controlInformation.set("stream.y", streamY->getType());
	controlInformation.set("stream.z", streamZ->getType());
	controlInformation.save(output);

	streamY->save(output);
	streamZ->save(output);
}

void CompactTriples::load(std::istream &input, ControlInformation &controlInformation)
{
	std::string codification = controlInformation.get("codification");
	if(codification != HDTVocabulary::TRIPLES_TYPE_COMPACT) {
		throw "Unexpected CompactTriples format";
	}

	numTriples = controlInformation.getUint("numTriples");
	order = (TripleComponentOrder) controlInformation.getUint("triples.component.order");

	std::string typeY = controlInformation.get("stream.y");
	std::string typeZ = controlInformation.get("stream.z");

	delete streamY;
	delete streamZ;

	streamY = StreamElements::getStream(typeY);
	streamZ = StreamElements::getStream(typeZ);

	streamY->load(input);
	streamZ->load(input);
}

unsigned int CompactTriples::getNumberOfElements()
{
	return numTriples;
}

unsigned int CompactTriples::size()
{
	return streamY->size()+streamZ->size();
}

/// ITERATOR
CompactTriplesIterator::CompactTriplesIterator(CompactTriples *pt, TripleID &pat)
		: triples(pt), numTriple(0), masterPos(0), slavePos(0), pattern(pat) {

	doFetch();
}

CompactTriplesIterator::~CompactTriplesIterator() {

}

void CompactTriplesIterator::readTriple() {
	if(numTriple==0) {
		x = 1;
		y = triples->streamY->get(masterPos++);
		z = triples->streamZ->get(slavePos++);
	} else {
		z = triples->streamZ->get(slavePos++);

		if(z==0) {
			z = triples->streamZ->get(slavePos++);

			y = triples->streamY->get(masterPos++);

			if(y==0) {
				y = triples->streamY->get(masterPos++);
				x++;
			}
		}
	}

	//cout << numTriple << "/" << triples->numTriples << "  "<< x << ", " << y << ", " << z << endl;

	nextv.setSubject(x);
	nextv.setPredicate(y);
	nextv.setObject(z);

	UnorderedTriple *trip = reinterpret_cast<UnorderedTriple *>(&nextv);
	swapComponentOrder(trip, triples->order, SPO);

	numTriple++;

	hasNextv = (numTriple<=triples->numTriples);
}

void CompactTriplesIterator::doFetch() {
	do {
		readTriple();
	} while(hasNextv && (!nextv.isValid() || !nextv.match(pattern)));
}

bool CompactTriplesIterator::hasNext() {
	return hasNextv;
}

TripleID CompactTriplesIterator::next() {
	TripleID ret = nextv;
	doFetch();
	return ret;
}


}
