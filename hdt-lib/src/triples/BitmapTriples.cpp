/*
 * BitmapTriples.cpp
 *
 *  Created on: 11/05/2011
 *      Author: mck
 */

#include "BitmapTriples.hpp"

#include "TripleIterators.hpp"

#include <HDTVocabulary.hpp>

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



void BitmapTriples::load(ModifiableTriples &triples, ProgressListener *listener) {
	triples.sort(order);

	IteratorTripleID *it = triples.searchAll();

	vector<unsigned int> vectorY, vectorZ;
	vector<bool> bitY, bitZ;

	//vector<unsigned int> dif_ys, dif_zs;
	unsigned int lastX, lastY, lastZ;
	unsigned int x, y, z;
	//unsigned int ys=0, zs=0, tys=0, tzs=0;

	// First triple
	if(it->hasNext()) {
		TripleID *triple = it->next();
		//cout << "111> " << *triple << endl;

		swapComponentOrder(triple, SPO, order);

		lastX = x = triple->getSubject();
		lastY = y = triple->getPredicate();
		lastZ = z = triple->getObject();

		vectorY.push_back(y);
		vectorZ.push_back(z);

		numTriples++;
	}

	// Rest of the triples
	while(it->hasNext()) {
		TripleID *triple = it->next();
		//cout << "111> " << *triple << endl;

		swapComponentOrder(triple, SPO, order);

		x = triple->getSubject();
		y = triple->getPredicate();
		z = triple->getObject();

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

void BitmapTriples::populateHeader(Header &header, string rootNode) {
	header.insert(rootNode, HDTVocabulary::TRIPLES_TYPE, getType());
	header.insert(rootNode, HDTVocabulary::TRIPLES_NUM_TRIPLES, getNumberOfElements() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_ORDER, order );  // TODO: Convert to String
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMY_TYPE, streamY->getType() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMZ_TYPE, streamZ->getType() );
}

string BitmapTriples::getType() {
	return HDTVocabulary::TRIPLES_TYPE_BITMAP;
}

IteratorTripleID *BitmapTriples::search(TripleID & pattern)
{
	std::string patternString = pattern.getPatternString();

	if(order==SPO && patternString=="?P?" && streamY->getType() == HDTVocabulary::STREAM_TYPE_WAVELET) {
		return new MiddleWaveletIterator(this, pattern);
	} else {
		if(patternString=="???" || patternString=="S??" || patternString=="SP?"|| patternString=="SPO" ) {
			return new BitmapTriplesSearchIterator(this, pattern);
		} else {
			return new SequentialSearchIteratorTripleID(pattern, new BitmapTriplesSearchIterator(this, pattern));
		}
	}
}

bool BitmapTriples::save(std::ostream & output, ControlInformation &controlInformation, ProgressListener *listener)
{
	controlInformation.clear();
	controlInformation.setUint("numTriples", getNumberOfElements());
	controlInformation.set("codification", getType());
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

void BitmapTriples::load(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener)
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
	swapComponentOrder(&pattern, SPO, triples->order);
	patX = pattern.getSubject();
	patY = pattern.getPredicate();
	patZ = pattern.getObject();

#if 0
	cout << "AdjY: " << endl;
	adjY.dump();
	cout << "AdjZ: " << endl;
	adjZ.dump();
	cout << "Pattern: " << patX << " " << patY << " " << patZ << endl;
#endif

#if 0
	for(uint mposZ=0;mposZ<adjZ.getSize(); mposZ++) {
		uint z = adjZ.get(mposZ);

		uint posY = adjZ.findListIndex(mposZ);

		uint y = adjY.get(posY);

		uint x = adjY.findListIndex(posY)+1;

		cout << "FWD2 posZ: " << mposZ << " posY: " << posY << "\t";
		cout << "Triple: " << x << ", " << y << ", " << z << endl;
	}
#endif

	findRange();

	goToStart();
}

void BitmapTriplesSearchIterator::updateOutput() {
	// Convert local order to SPO
	returnTriple.setAll(x,y,z);
	swapComponentOrder(&returnTriple, triples->order, SPO);
}

void BitmapTriplesSearchIterator::findRange()
{
	if(patX!=0) {
		// S X X
		if(patY!=0) {
			// S P X
			minY = adjY.find(patX-1, patY);
			maxY = minY+1;
			if(patZ!=0) {
				// S P O
				minZ = adjZ.find(minY,patZ);
				maxZ = minZ+1;
			} else {
				// S P ?
				minZ = adjZ.find(minY);
				maxZ = adjZ.last(minY)+1;
			}
		} else {
			// S ? X
			minY = adjY.find(patX-1);
			minZ = adjZ.find(minY);
			maxY = adjY.last(patX-1)+1;
			maxZ = adjZ.find(maxY);
		}
		nextY = adjY.last(patX-1)+1;
		nextZ = adjZ.last(minY)+1;
		x = patX;
	} else {
		// ? X X
		x = 1;
		minY=0;
		minZ=0;
                maxY = adjY.getSize();
                maxZ = adjZ.getSize();
		nextY = adjY.last(0)+1;
		nextZ = adjZ.last(0)+1;
	}
}

bool BitmapTriplesSearchIterator::hasNext()
{
	return posZ<maxZ;
}

TripleID *BitmapTriplesSearchIterator::next()
{
#if 0
	cout << "FWD posZ: " << posZ << " posY: " << posY << endl;
	cout << "\tFWD nextZ: " << nextZ << " nextY: " << nextY << endl;
	cout << "\tTriple: " << x << ", " << y << ", " << z << endl;
#endif

	z = adjZ.get(posZ);

	if(posZ==nextZ) {
		posY++;
		y = adjY.get(posY);
		nextZ = adjZ.find(posY+1);

		if(posY==nextY) {
			x++;
			nextY = adjY.find(x);
		}
	}

	posZ++;

	updateOutput();

	return &returnTriple;
}

bool BitmapTriplesSearchIterator::hasPrevious()
{
	return posZ>minZ;
}

TripleID *BitmapTriplesSearchIterator::previous()
{
	if(posZ>adjZ.getSize()) {
		posZ=adjZ.getSize();
	}

	posZ--;
	posY = adjZ.findListIndex(posZ);

	z = adjZ.get(posZ);
	y = adjY.get(posY);
	x = adjY.findListIndex(posY)+1;

	nextY = adjY.last(x-1)+1;
	nextZ = adjZ.last(posY)+1;

#if 0
	cout << "BACK posZ: " << posZ << " posY: " << posY << endl;
	cout << "\tBack nextZ: " << nextZ << " nextY: " << nextY << endl;
	cout << "\tTriple: " << x << ", " << y << ", " << z << endl;
#endif

	updateOutput();

	return &returnTriple;
}

void BitmapTriplesSearchIterator::goToStart()
{
	posY = minY;
	posZ = minZ;
	y = adjY.get(posY);
	z = adjZ.get(posZ);
}












MiddleWaveletIterator::MiddleWaveletIterator(BitmapTriples *trip, TripleID &pat) :
		triples(trip),
		pattern(pat),
		adjY(trip->streamY, trip->bitmapY),
		adjZ(trip->streamZ, trip->bitmapZ),
		predicateOcurrence(1),
		wavelet(reinterpret_cast<WaveletStream *>(trip->streamY))
{
	// Convert pattern to local order.
	swapComponentOrder(&pattern, SPO, triples->order);
	patX = pattern.getSubject();
	patY = pattern.getPredicate();
	patZ = pattern.getObject();

	try {
		// Find position of the first matching pattern.
		findRange();

		// If first not found, go through all to find one.
		if(!nextTriple.match(pattern)) {
			getNextTriple();
		}
	} catch (char *ex) {
		// If exception thrown, the selected triple could not be found.
		hasMoreTriples = false;
	}
}

void MiddleWaveletIterator::updateOutput() {
	// Convert local order to SPO
	nextTriple.setAll(x,y,z);

	swapComponentOrder(&nextTriple, triples->order, SPO);

	// Check termination condition.
	hasMoreTriples = (posY < adjY.getSize()) && (posZ < adjZ.getSize());

	if(predicateOcurrence==numOcurrences+1 && posZ == nextZ) {
		hasMoreTriples = false;
	}
}

void MiddleWaveletIterator::findRange()
{

#if 1
	cout << "AdjY: " << endl;
	adjY.dump();
	cout << "AdjZ: " << endl;
	adjZ.dump();
	cout << "Pattern: " << patX << " " << patY << " " << patZ << endl;
#endif

	for(unsigned int i=0; i<10; i++) {
		numOcurrences = wavelet->rank(i, wavelet->getNumberOfElements());
		cout << "Predicate "<< i << " occurs " << numOcurrences << " times" << endl;
	}

	numOcurrences = wavelet->rank(patY, wavelet->getNumberOfElements());

#if 1
	cout << "Predicate ocurs " << numOcurrences << " times" << endl;

	for(unsigned int i=1;i<numOcurrences;i++){
		unsigned int pos = wavelet->select(patY, i);
		cout << "Occurrence " << i << " => " << pos << " Value: " << adjY.get(pos) << endl;

		unsigned int tx = adjY.findListIndex(pos)+1;
		unsigned int ty = adjY.get(pos);

		unsigned int iniz = adjZ.find(pos);
		unsigned int endz = adjZ.last(pos);

		cout << tx << ", " << ty << "[" << iniz << "," << endz << "]" << endl;
		for(unsigned int j=iniz; j<=endz; j++) {
			unsigned int tz = adjZ.get(j);
			cout << tx <<", " << ty << ", " << tz << endl;
		}
	}
#endif

	posY = wavelet->select(patY, predicateOcurrence++);
	posZ = adjZ.find(posY);
	nextZ = adjZ.last(posY)+1;

	x = adjY.findListIndex(posY)+1;
	y = adjY.get(posY);
	z = adjZ.get(posZ);

#if 1
	cout << "Found pos: " << posY << ", " << posZ << endl;
	cout << "NextZ: " << nextZ << endl;
	cout << "Triple: " << x << ", " << y << ", " << z << endl;
#endif

	updateOutput();
}











void MiddleWaveletIterator::getNextTriple() {

	z = adjZ.get(posZ);

	if(posZ==nextZ && predicateOcurrence<numOcurrences) {
		posY = wavelet->select(patY, predicateOcurrence++);
		posZ = adjZ.find(posY);
		nextZ = adjZ.last(posY)+1;

		x = adjY.findListIndex(posY)+1;
		y = adjY.get(posY);
		z = adjZ.get(posZ);
	} else {
		posZ++;
	}

	updateOutput();
}

void MiddleWaveletIterator::getPreviousTriple() {
}

bool MiddleWaveletIterator::hasNext()
{
	return hasMoreTriples;
}

TripleID *MiddleWaveletIterator::next()
{
	getNextTriple();
	return &nextTriple;
}

bool MiddleWaveletIterator::hasPrevious()
{
	return hasPreviousTriples;
}

TripleID *MiddleWaveletIterator::previous()
{

}

void MiddleWaveletIterator::goToStart()
{

}






}
