/*
 * BitmapTriples.cpp
 *
 *  Created on: 11/05/2011
 *      Author: mck
 */

#include "BitmapTriples.hpp"

#include "TripleIterators.hpp"

#include <HDTVocabulary.hpp>

#include "../util/StopWatch.hpp"

namespace hdt {

#define CHECK_BITMAPTRIPLES_INITIALIZED if(bitmapY==NULL || bitmapZ==NULL){	throw "Accessing uninitialized BitmapTriples"; }

BitmapTriples::BitmapTriples() : numTriples(0), order(SPO) {
	streamY = StreamElements::getStream(spec.get("stream.y"));
	streamZ = StreamElements::getStream(spec.get("stream.z"));
	streamIndex = NULL;
	bitmapY = NULL;
	bitmapZ = NULL;
	bitmapIndex = NULL;
	waveletY = NULL;
}

BitmapTriples::BitmapTriples(HDTSpecification &specification) : numTriples(0), spec(specification) {
	std::string orderStr = spec.get("triples.component.order");
	order= parseOrder(orderStr.c_str());
	if(order==Unknown)
		order = SPO;
	streamY = StreamElements::getStream(spec.get("stream.y"));
	streamZ = StreamElements::getStream(spec.get("stream.z"));
	streamIndex = NULL;
	bitmapY = NULL;
	bitmapZ = NULL;
	bitmapIndex = NULL;
	waveletY = NULL;
}

BitmapTriples::~BitmapTriples() {
	if(bitmapY!=NULL) {
		delete bitmapY;
	}
	if(bitmapZ!=NULL) {
		delete bitmapZ;
	}
	if(waveletY != NULL && waveletY != streamY) {
		delete waveletY;
	}
	if(bitmapIndex!=NULL) {
		delete bitmapIndex;
	}
	if(streamIndex!=NULL) {
		delete streamIndex;
	}
	delete streamY;
	delete streamZ;
}

float BitmapTriples::cost(TripleID & triple)
{
	CHECK_BITMAPTRIPLES_INITIALIZED
	return 0;
}


void BitmapTriples::load(ModifiableTriples &triples, ProgressListener *listener) {
	triples.sort(order);

	IteratorTripleID *it = triples.searchAll();

	vector<unsigned int> vectorY, vectorZ;
	vector<bool> bitY, bitZ;

	unsigned int lastX, lastY, lastZ;
	unsigned int x, y, z;

	numTriples=0;

	while(it->hasNext()) {
		TripleID *triple = it->next();
		//cout << "111> " << *triple << endl;

		swapComponentOrder(triple, SPO, order);

		x = triple->getSubject();
		y = triple->getPredicate();
		z = triple->getObject();

		if(numTriples==0){
			vectorY.push_back(y);
			vectorZ.push_back(z);
		} else if(x!=lastX) {
#ifdef OLD_BITMAP
			bitY.push_back(0);
#endif
			bitY.push_back(1);
			vectorY.push_back(y);

#ifdef OLD_BITMAP
			bitZ.push_back(0);
#endif
			bitZ.push_back(1);
			vectorZ.push_back(z);
		} else if(y!=lastY) {
			bitY.push_back(0);
			vectorY.push_back(y);

#ifdef OLD_BITMAP
			bitZ.push_back(0);
#endif
			bitZ.push_back(1);
			vectorZ.push_back(z);
		} else {
			bitZ.push_back(0);
			vectorZ.push_back(z);
		}

		lastX = x;
		lastY = y;
		lastZ = z;

        NOTIFYCOND(listener, "Converting to BitmapTriples", numTriples, triples.getNumberOfElements());
		numTriples++;
	}

#ifdef OLD_BITMAP
	bitY.push_back(0);
#endif
	bitY.push_back(1);
#ifdef OLD_BITMAP
	bitZ.push_back(0);
#endif
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

	if(streamZ->getType()==HDTVocabulary::STREAM_TYPE_WAVELET) {
		waveletY = reinterpret_cast<WaveletStream *>(streamZ);
	}

#if 0
	AdjacencyList adjY(streamY, bitmapY);
	AdjacencyList adjZ(streamZ, bitmapZ);
	adjY.dump();
	adjZ.dump();
#endif

#if 0
	generateIndex(&iListener);
#endif
}

void BitmapTriples::generateIndex(ProgressListener *listener) {
	StopWatch st;

	IntermediateListener iListener(listener);

	iListener.setRange(0,40);
	vector <vector<unsigned int> > index;
	for(unsigned int i=0;i<streamZ->getNumberOfElements(); i++) {
		unsigned int val = streamZ->get(i);
		if(index.size()<val) {
			index.resize(val);
		}
		index[val-1].push_back(i);
		NOTIFYCOND(&iListener, "Generating Object lists", i, streamZ->getNumberOfElements());
	}

	BitString *indexBitmapTmp = new BitString(streamZ->getNumberOfElements());

	iListener.setRange(40, 80);
	unsigned int pos=0;
	vector<unsigned int> out;
	for(unsigned int i=0;i<index.size();i++){
		//cout << "Object " << i << " (" << index[i].size() << ") => ";
		if(index[i].size()<=0) {
			throw "Error generating index: Object should appear at least once";
		}
		for(unsigned int j=0;j<index[i].size()-1;j++){
			out.push_back(index[i][j]);
			indexBitmapTmp->setBit(pos, false);
			//cout << index[i][j] << " ";
			pos++;
		}
		//cout << "[" << index[i][index[i].size()-1] << endl;
		out.push_back(index[i][index[i].size()-1]);
		indexBitmapTmp->setBit(pos++, true);
		NOTIFYCOND(&iListener, "Serializing object lists", i, index.size());
	}
	index.clear();

	NOTIFY(listener, "Creating object bitmap", 80, 100 );
	bitmapIndex = new cds_static::BitSequenceRG(*indexBitmapTmp, 20);
	//bitmapIndex = new cds_static::BitSequenceRRR(*indexBitmapTmp,1024);

	NOTIFY(listener, "Creating object sequence", 90, 100 );
	VectorIterator it(out);
	streamIndex = StreamElements::getStream(HDTVocabulary::STREAM_TYPE_LOG);
	streamIndex->add(it);
	cout << "Index generated in " << st << endl;

	// Generate Wavelet
	NOTIFY(listener, "Generating wavelet", 90,100);
	st.reset();
	if(streamY->getType()==HDTVocabulary::STREAM_TYPE_WAVELET) {
		waveletY = reinterpret_cast<WaveletStream *>(streamY);
	} else {
		waveletY = new WaveletStream(streamY);

		if(false) { // FIXME: Substitute existing or leave both?
			delete streamY;
			streamY = waveletY;
		}
	}
	cout << "Wavelet generated in " << st << endl;

	cout << "Num triples: " << getNumberOfElements() << endl;
	cout << "Order: " << getOrderStr(order) << endl;
	cout << "Original triples size: " << size() << endl;
	cout << "Index size: " << bitmapIndex->getSize()+streamIndex->size() << " <" << ((unsigned long long)(bitmapIndex->getSize()+streamIndex->size()))*100 / size() << "%>"<< endl;
}

void BitmapTriples::populateHeader(Header &header, string rootNode) {
	header.insert(rootNode, HDTVocabulary::TRIPLES_TYPE, getType());
	header.insert(rootNode, HDTVocabulary::TRIPLES_NUM_TRIPLES, getNumberOfElements() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_ORDER, getOrderStr(order) );
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMY_TYPE, streamY->getType() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMZ_TYPE, streamZ->getType() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMY_SIZE, streamY->size() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMZ_SIZE, streamZ->size() );
	if(bitmapY!=NULL) {
		header.insert(rootNode, HDTVocabulary::TRIPLES_BITMAPY_SIZE, bitmapY->getSize() );
	}
	if(bitmapZ!=NULL) {
		header.insert(rootNode, HDTVocabulary::TRIPLES_BITMAPZ_SIZE, bitmapZ->getSize() );
	}
}

string BitmapTriples::getType() {
	return HDTVocabulary::TRIPLES_TYPE_BITMAP;
}

IteratorTripleID *BitmapTriples::search(TripleID & pattern)
{
	CHECK_BITMAPTRIPLES_INITIALIZED

	TripleID reorderedPat = pattern;
	swapComponentOrder(&reorderedPat, SPO, this->order);
	std::string patternString = reorderedPat.getPatternString();

	if((streamIndex!=NULL && bitmapIndex != NULL) && (patternString=="??O" || patternString=="?PO" )) {
		if(patternString=="??O") {
			return new ObjectIndexIterator(this, pattern);
		} else {
			return new SequentialSearchIteratorTripleID(pattern, new ObjectIndexIterator(this, pattern));
		}
	} else if( waveletY != NULL && patternString=="?P?") {
		return new MiddleWaveletIterator(this, pattern);
	} else {
		if(patternString=="???" || patternString=="S??" || patternString=="SP?"|| patternString=="SPO" ) {
			return new BitmapTriplesSearchIterator(this, pattern);
		} else {
			return new SequentialSearchIteratorTripleID(pattern, new BitmapTriplesSearchIterator(this, pattern));
		}
	}
}

void BitmapTriples::save(std::ostream & output, ControlInformation &controlInformation, ProgressListener *listener)
{
	CHECK_BITMAPTRIPLES_INITIALIZED

	controlInformation.clear();
	controlInformation.setUint("numTriples", getNumberOfElements());
	controlInformation.set("codification", getType());
	controlInformation.setUint("componentOrder", order);
	controlInformation.set("stream.y", streamY->getType());
	controlInformation.set("stream.z", streamZ->getType());
	controlInformation.save(output);

	// Fixme: Bitmap directly on istream/ostream??
	IntermediateListener iListener(listener);
	ofstream *out = dynamic_cast<ofstream *>(&output);
	//cout << "Save BitmapY " << out->tellp() << endl;
	iListener.setRange(0,5);
	iListener.notifyProgress(0, "BitmapTriples saving Bitmap Y");
	bitmapY->save(*out);

	iListener.setRange(5,15);
	iListener.notifyProgress(0, "BitmapTriples saving Bitmap Z");
	//cout << "Save BitmapZ " << out->tellp() << endl;
	bitmapZ->save(*out);

	iListener.setRange(15,30);
	iListener.notifyProgress(0, "BitmapTriples saving Stream Y");
	//cout << "Save StreamY " << out->tellp() << endl;
	streamY->save(output);

	iListener.setRange(30,100);
	iListener.notifyProgress(0, "BitmapTriples saving Stream Z");
	//cout << "Save StreamZ " << out->tellp() << endl;
	streamZ->save(output);
	//cout << "OK " << out->tellp() << endl;
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

	IntermediateListener iListener(listener);

	// Fixme: Bitmap directly on istream/ostream??
	ifstream *in = dynamic_cast<ifstream *>(&input);

	iListener.setRange(0,5);
	iListener.notifyProgress(0, "BitmapTriples loading Bitmap Y");
	//cout << "Load BitmapY " << in->tellg() << endl;
	bitmapY = cds_static::BitSequence::load(*in);

	iListener.setRange(5,10);
	iListener.notifyProgress(0, "BitmapTriples loading Bitmap Z");
	//cout << "Load BitmapZ " << in->tellg() << endl;
	bitmapZ = cds_static::BitSequence::load(*in);

	iListener.setRange(10,20);
	iListener.notifyProgress(0, "BitmapTriples loading Stream Y");
	//cout << "Load StreamY " << in->tellg() << endl;
	streamY->load(input);

	iListener.setRange(20,50);
	iListener.notifyProgress(0, "BitmapTriples loading Stream Z");
	//cout << "Load StreamZ " << in->tellg() << endl;
	streamZ->load(input);
	//cout << "OK " << in->tellg() << endl;

	if(streamZ->getType()==HDTVocabulary::STREAM_TYPE_WAVELET) {
		waveletY = reinterpret_cast<WaveletStream *>(streamZ);
	}

#if 0
	iListener.setRange(50, 100);
	generateIndex(&iListener);
#endif
}

unsigned int BitmapTriples::getNumberOfElements()
{
	return numTriples;
}

unsigned int BitmapTriples::size()
{
	if(bitmapY!=NULL && bitmapZ!=NULL) {
		return bitmapY->getSize()+bitmapZ->getSize()+streamY->size()+streamZ->size();
	}
	return streamY->size()+streamZ->size();
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
	cout << "Pattern: " << patX << " " << patY << " " << patZ << endl;
	cout << "AdjY: " << endl;
	adjY.dump();
	cout << "AdjZ: " << endl;
	adjZ.dump();
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
			try {
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
			} catch (const char *ex) {
				// Item not found in list, no results.
				minY = minZ = maxY = maxZ = 0;
			}
		} else {
			// S ? X
			minY = adjY.find(patX-1);
			minZ = adjZ.find(minY);
			maxY = adjY.last(patX-1)+1;
			maxZ = adjZ.find(maxY);
		}
		x = patX;
	} else {
		// ? X X
		minY=0;
		minZ=0;
		maxY = adjY.getSize();
		maxZ = adjZ.getSize();
	}

	//cout << "findRange: Y(" << minY << "," << nextY << ", " << maxY << ") Z(" << minZ <<", " << nextZ << ", " << maxZ << ")" << endl;
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

	posZ--;

#if 0
        // TODO: Keep prevZ updated to save bitmap accesses.
        z = adjZ.get(posZ);
        if(posZ==prevZ) {
            posY--;
            y = adjY.get(posY);
            prevZ = adjZ.find(posY);

            if(posY==prevY) {
                x--;
                prevY = adjY.find(x);
            }
        }
 #else
	posY = adjZ.findListIndex(posZ);

	z = adjZ.get(posZ);
	y = adjY.get(posY);
	x = adjY.findListIndex(posY)+1;

	nextY = adjY.last(x-1)+1;
	nextZ = adjZ.last(posY)+1;
#endif

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
	posZ = minZ;
	posY = adjZ.findListIndex(posZ);

	z = adjZ.get(posZ);
	y = adjY.get(posY);
	x = adjY.findListIndex(posY)+1;

	nextY = adjY.last(x-1)+1;
	nextZ = adjZ.last(posY)+1;
}












MiddleWaveletIterator::MiddleWaveletIterator(BitmapTriples *trip, TripleID &pat) :
		triples(trip),
		pattern(pat),
		adjY(trip->streamY, trip->bitmapY),
		adjZ(trip->streamZ, trip->bitmapZ),
		predicateOcurrence(1),
		wavelet(trip->waveletY)
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

	// Find position of the first matching pattern.
	numOcurrences = wavelet->rank(patY, wavelet->getNumberOfElements());

	goToStart();
}

void MiddleWaveletIterator::updateOutput() {
	// Convert local order to SPO
	returnTriple.setAll(x,y,z);

	swapComponentOrder(&returnTriple, triples->order, SPO);
}

bool MiddleWaveletIterator::hasNext()
{
	return predicateOcurrence<numOcurrences || posZ <= nextZ;
}

TripleID *MiddleWaveletIterator::next()
{
	//cout << "nextTriple: " << predicateOcurrence << ", " << prevZ << ", " << posZ << ", " << nextZ << endl;
	if(posZ>nextZ) {
		predicateOcurrence++;
		posY = wavelet->select(patY, predicateOcurrence);
		prevZ = adjZ.find(posY);
		nextZ = adjZ.last(posY);
		posZ = prevZ;

		x = adjY.findListIndex(posY)+1;
		y = adjY.get(posY);
		z = adjZ.get(posZ);
		posZ++;
	} else {
		z = adjZ.get(posZ);
		posZ++;
	}
	updateOutput();
	return &returnTriple;
}

bool MiddleWaveletIterator::hasPrevious()
{
	return predicateOcurrence>1 || posZ>=prevZ;
}

TripleID *MiddleWaveletIterator::previous()
{
	//cout << "previousTriple: " << predicateOcurrence << ", " << prevZ << ", " << posZ << ", " << nextZ << endl;
	if(posZ<=prevZ) {
		predicateOcurrence--;
		posY = wavelet->select(patY, predicateOcurrence);
		nextZ = adjZ.last(posY);
		prevZ = adjZ.find(posY);
		posZ = nextZ;

		x = adjY.findListIndex(posY)+1;
		y = adjY.get(posY);
		z = adjZ.get(posZ);
	} else {
		posZ--;
		z = adjZ.get(posZ);

	}
	updateOutput();
	return &returnTriple;
}

void MiddleWaveletIterator::goToStart()
{
	predicateOcurrence = 1;
	posY = wavelet->select(patY, predicateOcurrence);
	prevZ = adjZ.find(posY);
	nextZ = adjZ.last(posY);

	posZ = prevZ;

	x = adjY.findListIndex(posY)+1;
	y = adjY.get(posY);
	z = adjZ.get(posZ);
}











ObjectIndexIterator::ObjectIndexIterator(BitmapTriples *trip, TripleID &pat) :
		triples(trip),
		pattern(pat),
		adjY(trip->streamY, trip->bitmapY),
		adjZ(trip->streamZ, trip->bitmapZ),
		adjIndex(trip->streamIndex, trip->bitmapIndex)
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
	cout << "AdjIndex: " << endl;
	adjIndex.dump();
	cout << "Pattern: " << patX << " " << patY << " " << patZ << endl;
#endif

	goToStart();
}

void ObjectIndexIterator::updateOutput() {
	// Convert local order to SPO
	returnTriple.setAll(x,y,z);

	swapComponentOrder(&returnTriple, triples->order, SPO);
}

bool ObjectIndexIterator::hasNext()
{
	return posIndex <= maxIndex;
}

TripleID *ObjectIndexIterator::next()
{
	unsigned int posZ = adjIndex.get(posIndex);
	unsigned int posY = adjZ.findListIndex(posZ);

	z = adjZ.get(posZ);
	y = adjY.get(posY);
	x = adjY.findListIndex(posY)+1;

	posIndex++;

	updateOutput();
	return &returnTriple;
}

bool ObjectIndexIterator::hasPrevious()
{
	return posIndex>=minIndex;
}

TripleID *ObjectIndexIterator::previous()
{
	posIndex--;

	unsigned int posZ = adjIndex.get(posIndex);
	unsigned int posY = adjZ.findListIndex(posZ);

	z = adjZ.get(posZ);
	y = adjY.get(posY);
	x = adjY.findListIndex(posY)+1;

	updateOutput();
	return &returnTriple;
}

void ObjectIndexIterator::goToStart()
{
	minIndex=adjIndex.find(patZ-1);
	maxIndex=adjIndex.last(patZ-1);

	posIndex=minIndex;
}





}
