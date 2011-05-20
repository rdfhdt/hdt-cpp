/*
 * BitmapTriples.cpp
 *
 *  Created on: 11/05/2011
 *      Author: mck
 */

#include "BitmapTriples.hpp"
#include "TripleOrderConvert.hpp"

namespace hdt {


BitmapTriples::BitmapTriples() : numTriples(0), order(SPO) {
	masterStream = StreamElements::getStream(spec.get("stream.y"));
	slaveStream = StreamElements::getStream(spec.get("stream.z"));
}

BitmapTriples::BitmapTriples(HDTSpecification &specification) : numTriples(0), spec(specification) {
	std::string orderStr = spec.get("triples.component.order");
	order= parseOrder(orderStr.c_str());
	if(order==Unknown)
		order = SPO;

	masterStream = StreamElements::getStream(spec.get("stream.y"));
	slaveStream = StreamElements::getStream(spec.get("stream.z"));
}




BitmapTriples::~BitmapTriples() {
	delete masterStream;
	delete slaveStream;
}


float BitmapTriples::cost(TripleID & triple)
{
}



void BitmapTriples::load(ModifiableTriples &triples) {
	TripleID all(0,0,0);

	IteratorTripleID *it = triples.search(all);

	vector<unsigned int> vectorY, vectorZ;
	vector<bool> bitY, bitZ;

	//vector<unsigned int> dif_ys, dif_zs;
	unsigned int lastX, lastY, lastZ;
	unsigned int x, y, z;
	//unsigned int ys=0, zs=0, tys=0, tzs=0;

	// First triple
	if(it->hasNext()) {
		TripleID triple = it->next();
		//cout << "111> " << triple << endl;

		UnorderedTriple *ut = reinterpret_cast<UnorderedTriple *>(&triple);
		swapComponentOrder(ut, SPO, order);

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
		//if(numTriples<20) cout << "111> " << triple << endl;

		UnorderedTriple *ut = reinterpret_cast<UnorderedTriple *>(&triple);
		swapComponentOrder(ut, SPO, order);

		x = ut->x;
		y = ut->y;
		z = ut->z;

		if(x!=lastX) {
			bitY.push_back(0);
			bitY.push_back(1);
			vectorY.push_back(y);

			bitZ.push_back(0);
			bitZ.push_back(1);
			vectorZ.push_back(z);
		} else if(y!=lastY) {
			bitY.push_back(0);
			vectorY.push_back(y);

			bitZ.push_back(0);
			bitZ.push_back(1);
			vectorZ.push_back(z);
		} else {
			bitZ.push_back(0);
			vectorZ.push_back(z);
		}

		lastX = x;
		lastY = y;
		lastZ = z;

		numTriples++;
	}

	bitY.push_back(0);
	bitY.push_back(1);
	bitZ.push_back(0);
	bitZ.push_back(1);

	delete it;

	VectorIterator itY(vectorY);
	VectorIterator itZ(vectorZ);

	masterStream->add(itY);
	slaveStream->add(itZ);

#if 0
	// Debug Adjacency Lists
	cout << "Y" << vectorY.size() << "): ";
	for(unsigned int i=0;i<20 && i<masterStream->getNumberOfElements();i++){
		cout << masterStream->get(i) << " ";
	}
	cout << endl;

	cout << "Z" << vectorZ.size() << "): ";
	for(unsigned int i=0;i<20 && i<slaveStream->getNumberOfElements();i++){
		cout << slaveStream->get(i) << " ";
	}
	cout << endl;

	cout << "BY: ";
	for(unsigned int i=0;i<20 && i<bitY.size();i++){
		cout << bitY[i] << " ";
	}
	cout << endl;

	cout << "BZ: ";
	for(unsigned int i=0;i<20 && i<bitZ.size();i++){
		cout << bitZ[i] << " ";
	}
	cout << endl;
#endif

	BitString *bsy = new BitString(bitY.size());
	for(unsigned int i=0;i<bitY.size();i++) {
		bsy->setBit(i, bitY[i]);
	}

	BitString *bsz = new BitString(bitZ.size());
	for(unsigned int i=0;i<bitZ.size();i++) {
		bsz->setBit(i, bitZ[i]);
	}

	bitY.clear();
	bitZ.clear();

	bitmapY = new cds_static::BitSequenceRG(*bsy, 20);
	bitmapZ = new cds_static::BitSequenceRG(*bsz, 20);
}

void BitmapTriples::populateHeader(Header &header) {

}

IteratorTripleID *BitmapTriples::search(TripleID & pattern)
{
	return new BitmapTriplesIterator(this, pattern);
}

bool BitmapTriples::save(std::ostream & output, ControlInformation &controlInformation)
{
	controlInformation.clear();
	controlInformation.setUint("numTriples", getNumberOfElements());
	controlInformation.set("codification", "http://purl.org/HDT/hdt#triplesBitmap");
	controlInformation.setUint("componentOrder", order);
	controlInformation.set("stream.y", masterStream->getType());
	controlInformation.set("stream.z", slaveStream->getType());
	controlInformation.save(output);

	// Fixme: Bitmap directly on istream/ostream??
	ofstream *out = dynamic_cast<ofstream *>(&output);
	bitmapY->save(*out);
	bitmapZ->save(*out);

	masterStream->save(output);
	slaveStream->save(output);
}

void BitmapTriples::load(std::istream &input, ControlInformation &controlInformation)
{
	numTriples = controlInformation.getUint("numTriples");
	order = (TripleComponentOrder) controlInformation.getUint("componentOrder");

	std::string typeY = controlInformation.get("stream.y");
	std::string typeZ = controlInformation.get("stream.z");

	delete masterStream;
	delete slaveStream;

	masterStream = StreamElements::getStream(typeY);
	slaveStream = StreamElements::getStream(typeZ);

	// Fixme: Bitmap directly on istream/ostream??
	ifstream *in = dynamic_cast<ifstream *>(&input);
	bitmapY = cds_static::BitSequence::load(*in);
	bitmapZ = cds_static::BitSequence::load(*in);

	masterStream->load(input);
	slaveStream->load(input);
}

unsigned int BitmapTriples::getNumberOfElements()
{
	return numTriples;
}

unsigned int BitmapTriples::size()
{
	return bitmapY->getSize()+bitmapZ->getSize()+masterStream->size()+slaveStream->size();
}

/// ITERATOR
BitmapTriplesIterator::BitmapTriplesIterator(BitmapTriples *pt, TripleID &pat)
		: triples(pt), numTriple(0), masterPos(0), slavePos(0), bitYpos(0), bitZpos(0), pattern(pat) {

	doFetch();
}

BitmapTriplesIterator::~BitmapTriplesIterator() {

}

void BitmapTriplesIterator::readTriple() {
	bool bitY, bitZ;

	if(numTriple==0) {
		x = 1;
		y = triples->masterStream->get(masterPos++);
		z = triples->slaveStream->get(slavePos++);

		bool bitY = triples->bitmapY->access(bitYpos++);
		bool bitZ = triples->bitmapZ->access(bitZpos++);
	} else {
		z = triples->slaveStream->get(slavePos++);
		bool bitZ = triples->bitmapZ->access(bitZpos++);

		if(bitZ) {
			bitZ = triples->bitmapZ->access(bitZpos++);

			y = triples->masterStream->get(masterPos++);
			bool bitY = triples->bitmapY->access(bitYpos++);

			if(bitY) {
				bitY = triples->bitmapY->access(bitYpos++);

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

void BitmapTriplesIterator::doFetch() {
	do {
		readTriple();
	} while(hasNextv && (!nextv.isValid() || !nextv.match(pattern)));
}

bool BitmapTriplesIterator::hasNext() {
	return hasNextv;
}

TripleID BitmapTriplesIterator::next() {
	TripleID ret = nextv;
	doFetch();
	return ret;
}


}
