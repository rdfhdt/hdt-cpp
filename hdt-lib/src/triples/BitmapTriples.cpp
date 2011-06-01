/*
 * BitmapTriples.cpp
 *
 *  Created on: 11/05/2011
 *      Author: mck
 */

#include "BitmapTriples.hpp"

namespace hdt {


BitmapTriples::BitmapTriples() : numTriples(0), order(SPO) {
	streamY = StreamElements::getStream(spec.get("stream.y"));
	streamZ = StreamElements::getStream(spec.get("stream.z"));
}

BitmapTriples::BitmapTriples(HDTSpecification &specification) : numTriples(0), spec(specification) {
	std::string orderStr = spec.get("triples.component.order");
	order= parseOrder(orderStr.c_str());
	if(order==Unknown)
		order = SPO;

	streamY = StreamElements::getStream(spec.get("stream.y"));
	streamZ = StreamElements::getStream(spec.get("stream.z"));
}




BitmapTriples::~BitmapTriples() {
	delete streamY;
	delete streamZ;
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

	streamY->add(itY);
	streamZ->add(itZ);

#if 0
	// Debug Adjacency Lists
	cout << "Y" << vectorY.size() << "): ";
	for(unsigned int i=0;i<20 && i<streamY->getNumberOfElements();i++){
		cout << streamY->get(i) << " ";
	}
	cout << endl;

	cout << "Z" << vectorZ.size() << "): ";
	for(unsigned int i=0;i<20 && i<streamZ->getNumberOfElements();i++){
		cout << streamZ->get(i) << " ";
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
	return new BitmapTriplesSearchIterator(this, pattern);
}

bool BitmapTriples::save(std::ostream & output, ControlInformation &controlInformation)
{
	controlInformation.clear();
	controlInformation.setUint("numTriples", getNumberOfElements());
	controlInformation.set("codification", "http://purl.org/HDT/hdt#triplesBitmap");
	controlInformation.setUint("componentOrder", order);
	controlInformation.set("stream.y", streamY->getType());
	controlInformation.set("stream.z", streamZ->getType());
	controlInformation.save(output);

	// Fixme: Bitmap directly on istream/ostream??
	ofstream *out = dynamic_cast<ofstream *>(&output);
	bitmapY->save(*out);
	bitmapZ->save(*out);

	streamY->save(output);
	streamZ->save(output);
}

void BitmapTriples::load(std::istream &input, ControlInformation &controlInformation)
{
	numTriples = controlInformation.getUint("numTriples");
	order = (TripleComponentOrder) controlInformation.getUint("componentOrder");

	std::string typeY = controlInformation.get("stream.y");
	std::string typeZ = controlInformation.get("stream.z");

	delete streamY;
	delete streamZ;

	streamY = StreamElements::getStream(typeY);
	streamZ = StreamElements::getStream(typeZ);

	// Fixme: Bitmap directly on istream/ostream??
	ifstream *in = dynamic_cast<ifstream *>(&input);
	bitmapY = cds_static::BitSequence::load(*in);
	bitmapZ = cds_static::BitSequence::load(*in);

	streamY->load(input);
	streamZ->load(input);
}

unsigned int BitmapTriples::getNumberOfElements()
{
	return numTriples;
}

unsigned int BitmapTriples::size()
{
	return bitmapY->getSize()+bitmapZ->getSize()+streamY->size()+streamZ->size();
}

/// ITERATOR
BitmapTriplesSearchIterator::BitmapTriplesSearchIterator(BitmapTriples *trip, TripleID &pat) :
		triples(trip),
		pattern(pat),
		adjY(trip->streamY, trip->bitmapY),
		adjZ(trip->streamZ, trip->bitmapZ)
{
	// Convert pattern to local order.
	UnorderedTriple *unorderedPattern = reinterpret_cast<UnorderedTriple *>(&pattern);
	swapComponentOrder(unorderedPattern, SPO, triples->order);
	patX = unorderedPattern->x;
	patY = unorderedPattern->y;
	patZ = unorderedPattern->z;

	try {
		// Find position of the first matching pattern.
		findFirst();

		// If first not found, go through all to find one.
		if(!nextv.match(pattern)) {
			doFetch();
		}
	} catch (char *ex) {
		// If exception thrown, the selected triple could not be found.
		hasNextv = false;
	}
}

void BitmapTriplesSearchIterator::updateOutput() {
	// Convert local order to SPO
	nextv.setSubject(x);
	nextv.setPredicate(y);
	nextv.setObject(z);
	UnorderedTriple *trip = reinterpret_cast<UnorderedTriple *>(&nextv);
	swapComponentOrder(trip, triples->order, SPO);

	// Check termination condition.
	hasNextv = (posY <= adjY.getSize()) && (posZ <= adjZ.getSize());

	if(!goThroughAll) {
		hasNextv = hasNextv && nextv.match(pattern);
	}
}

void BitmapTriplesSearchIterator::findFirst()
{

#if 0
	cout << "AdjY: " << endl;
	adjY.dump();
	cout << "AdjZ: " << endl;
	adjZ.dump();
	cout << "Pattern: " << patX << " " << patY << " " << patZ << endl;
#endif

	goThroughAll = false;

	if(patX!=0) {
		// S X X
		if(patY!=0) {
			// S P X
			posY = adjY.find(patX-1, patY);
			if(patZ!=0) {
				// S P O
				posZ = adjZ.find(posY,patZ);
			} else {
				// S P ?
				posZ = adjZ.find(posY);
			}
		} else {
			// S ? X
			goThroughAll = patZ!=0;

			posY = adjY.find(patX-1);
			posZ = adjZ.find(posY);
		}
		nextY = adjY.last(patX-1)+1;
		nextZ = adjZ.last(posY)+1;
		x = patX;
	} else {
		// ? X X
		x = 1;
		posY=0;
		posZ=0;
		nextY = adjY.last(0)+1;
		nextZ = adjZ.last(0)+1;
		goThroughAll = true;
	}

#if 0
	cout << "Found pos: " << posY << ", " << posZ << endl;
	cout << "Next: " << nextY << ", " << nextZ << endl;
#endif

	y = adjY.get(posY++);
	z = adjZ.get(posZ++);

	updateOutput();
}

void BitmapTriplesSearchIterator::readTriple() {
    z = adjZ.get(posZ++);

	if(posZ==nextZ+1) {
		y = adjY.get(posY++);
		nextZ = adjZ.find(posY);

		if(posY==nextY+1) {
			x++;
			nextY = adjY.find(x);
		}
	}

	updateOutput();
}

void BitmapTriplesSearchIterator::doFetch() {
	do {
		readTriple();
	} while(hasNextv && (!nextv.isValid() || !nextv.match(pattern)));
}

bool BitmapTriplesSearchIterator::hasNext()
{
	return hasNextv;
}

TripleID BitmapTriplesSearchIterator::next()
{
	TripleID ret = nextv;
	//cout << "POS: " << posY-1 << "," << posZ-1 << " => " << nextv << endl;
	doFetch();

	return ret;
}

}
