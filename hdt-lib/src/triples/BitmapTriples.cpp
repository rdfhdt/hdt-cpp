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

#include "../stream/LogStream2.hpp"

#include <algorithm>

namespace hdt {

#define SAVE_ADJ_LIST

#define CHECK_BITMAPTRIPLES_INITIALIZED if(bitmapY==NULL || bitmapZ==NULL){	throw "Accessing uninitialized BitmapTriples"; }

BitmapTriples::BitmapTriples() : numTriples(0), order(SPO) {
	streamY = StreamElements::getStream(spec.get("stream.y"));
	streamZ = StreamElements::getStream(spec.get("stream.z"));
	streamIndex = NULL;
	bitmapY = NULL;
	bitmapZ = NULL;
	bitmapIndex = NULL;
	waveletY = NULL;
	predicateCount = NULL;
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
	predicateCount = NULL;
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
	if(predicateCount!=NULL) {
		delete predicateCount;
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

	LogStream2 *vectorY = new LogStream2(bits(triples.getNumberOfElements()));
	LogStream2 *vectorZ = new LogStream2(bits(triples.getNumberOfElements()),triples.getNumberOfElements());

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
			vectorY->push_back(y);
			vectorZ->push_back(z);
		} else if(x!=lastX) {
#ifdef OLD_BITMAP
			bitY.push_back(0);
#endif
			bitY.push_back(1);
			vectorY->push_back(y);

#ifdef OLD_BITMAP
			bitZ.push_back(0);
#endif
			bitZ.push_back(1);
			vectorZ->push_back(z);
		} else if(y!=lastY) {
			bitY.push_back(0);
			vectorY->push_back(y);

#ifdef OLD_BITMAP
			bitZ.push_back(0);
#endif
			bitZ.push_back(1);
			vectorZ->push_back(z);
		} else {
			bitZ.push_back(0);
			vectorZ->push_back(z);
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

	vectorY->reduceBits();

	delete streamY;
	streamY = vectorY;

	delete streamZ;
	streamZ = vectorZ;

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

	//bitmapY = new cds_static::BitSequenceRG(*bsy, 20);
	//bitmapZ = new cds_static::BitSequenceRG(*bsz, 20);
	bitmapY = new cds_static::BitSequence375(*bsy);
	bitmapZ = new cds_static::BitSequence375(*bsz);

#if 0
	AdjacencyList adjY(streamY, bitmapY);
	AdjacencyList adjZ(streamZ, bitmapZ);
	adjY.dump();
	adjZ.dump();
#endif
}

// Sort by predicate
struct sort_pred {
    bool operator()(const std::pair<unsigned int,unsigned int> &left, const std::pair<unsigned int, unsigned int> &right) {
        return left.second < right.second;
    }
};

void BitmapTriples::generateWavelet(ProgressListener *listener) {
	NOTIFY(listener, "Generating wavelet", 0,100);
	if(streamY->getType()==HDTVocabulary::STREAM_TYPE_WAVELET) {
		waveletY = reinterpret_cast<WaveletStream *>(streamY);
	} else {
		waveletY = new WaveletStream(streamY);

                if(true) { // FIXME: Substitute existing or leave both?
			delete streamY;
			streamY = waveletY;
		}
	}
}

void BitmapTriples::generateIndex(ProgressListener *listener) {
	StopWatch st;

	IntermediateListener iListener(listener);
	iListener.setRange(0,40);

	cout << "Object Lists" << endl;

	// For each object, a list of (zpos, predicate)
	vector<vector<pair<unsigned int, unsigned int> > > index;
	int maxpred = 0;
	for(unsigned int i=0;i<streamZ->getNumberOfElements(); i++) {
		unsigned int val = streamZ->get(i);
		if(index.size()<val) {
			index.resize(val);
		}
		unsigned int adjZlist = i>0 ?  bitmapZ->rank1(i-1) : 0;
#ifdef SAVE_ADJ_LIST
		unsigned int savedPos = adjZlist;
#else
		unsigned int savedPos = i;
#endif
		//cout << "Item " << i << " in adjlist " << adjZlist << endl;
		unsigned int pred = streamY->get(adjZlist);
		maxpred = pred>maxpred ? pred : maxpred;

		index[val-1].push_back(std::make_pair(savedPos, (unsigned short)pred));
		NOTIFYCOND(&iListener, "Generating Object lists", i, streamZ->getNumberOfElements());
	}

	if(predicateCount!=NULL) {
		delete predicateCount;
	}
	predicateCount = new LogStream2(bits(streamZ->getNumberOfElements()), maxpred);
	for(int i=0;i<maxpred;i++ ){
		predicateCount->push_back(0);
	}

	// Generate list and bitmap.
	BitString *indexBitmapObjectTmp = new BitString(streamZ->getNumberOfElements());

#ifdef USE_BITMAP_PREDICATES
	BitString *indexBitmapPredicateTmp = new BitString(streamZ->getNumberOfElements());
#endif

	cout << "Serialize" << endl;
	iListener.setRange(40, 80);
	unsigned int pos=0;
#ifdef SAVE_ADJ_LIST
	unsigned int numBits = bits(streamY->getNumberOfElements());
#else
	unsigned int numBits = bits(streamZ->getNumberOfElements());
#endif
	unsigned int numElements = streamZ->getNumberOfElements();
	LogStream2 *stream = new LogStream2(numBits, numElements );
	streamIndex = stream;
	for(unsigned int i=0;i<index.size();i++){
		if(index[i].size()<=0) {
			throw "Error generating index: Object should appear at least once";
		}

		// Sort by predicate of this object
		std::sort(index[i].begin(), index[i].end(), sort_pred());

		for(unsigned int j=0;j<index[i].size();j++){
			stream->push_back(index[i][j].first);
			predicateCount->set(index[i][j].second-1, predicateCount->get(index[i][j].second-1)+1);

			if(j==index[i].size()-1) {
				// Last element of the list
				indexBitmapObjectTmp->setBit(pos, true);
			} else {
				// Elements inside the list
				indexBitmapObjectTmp->setBit(pos, false);
			}
			pos++;
		}
		NOTIFYCOND(&iListener, "Serializing object lists", i, index.size());
	}
	index.clear();

	predicateCount->reduceBits();

	NOTIFY(listener, "Creating object bitmap", 80, 85 );
	//bitmapIndex = new cds_static::BitSequenceRG(*indexBitmapObjectTmp, 20);
	bitmapIndex = new cds_static::BitSequence375(*indexBitmapObjectTmp);
	delete indexBitmapObjectTmp;

	cout << "Index generated in " << st << endl;

	// Generate Wavelet
	st.reset();
	generateWavelet();
	cout << "Wavelet generated in " << st << endl;

	cout << "Num triples: " << getNumberOfElements() << endl;
	cout << "Order: " << getOrderStr(order) << endl;
	cout << "Original triples size: " << size() << endl;
	cout << "Stream size: " << streamIndex->size() << " <" << ((unsigned long long)(streamIndex->size())*100) / size() << "%>"<< endl;
	cout << "Bitmap Object size: " << bitmapIndex->getSize() << " <" << ((unsigned long long)(bitmapIndex->getSize()))*100 / size() << "%>"<< endl;
#ifdef USE_BITMAP_PREDICATES
	cout << "Bitmap Predicate size: " << bitmapIndexPredicates->getSize() << " <" << ((unsigned long long)(bitmapIndexPredicates->getSize()))*100 / size() << "%>"<< endl;
#endif

        cout << "Total Index size: " << bitmapIndex->getSize()+streamIndex->size() << " <" << ((unsigned long long)(bitmapIndex->getSize()+streamIndex->size()))*100 / size() << "%>"<< endl;
        cout << "Total size: " << size()+bitmapIndex->getSize()+streamIndex->size() << endl;
#ifdef SAVE_ADJ_LIST
	cout << "Number of lists: " << bitmapZ->countOnes() << " Bits: " << bits(bitmapZ->countOnes()) << endl;
#else
	cout << "Number of elements: " << bitmapZ->getLength() << " Bits: " << bits(bitmapZ->getLength()) << endl;
#endif
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

	if(patternString=="S?O") {
	    if(this->order == SPO) {
		return new SequentialSearchIteratorTripleID(pattern, new BitmapTriplesSearchIterator(this, pattern));
	    } else if( (this->order == OPS) && (streamIndex!=NULL)) {
		return new SequentialSearchIteratorTripleID(pattern, new ObjectIndexIterator(this, pattern));
	    }
	}

	if((streamIndex!=NULL) && (patternString=="??O" || patternString=="?PO" )) {
		//if(patternString=="??O") {
			return new ObjectIndexIterator(this, pattern);
		//} else {
		//	return new SequentialSearchIteratorTripleID(pattern, new ObjectIndexIterator(this, pattern));
		//}
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
}

void BitmapTriples::saveIndex(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener) {
	if(streamIndex==NULL || bitmapIndex==NULL) {
		generateIndex(listener);
	}

	controlInformation.clear();
	controlInformation.setIndex(true);
	controlInformation.setUint("numTriples", getNumberOfElements());
	controlInformation.set("stream.index", streamIndex->getType());
	controlInformation.save(output);

	IntermediateListener iListener(listener);
	ofstream *out = dynamic_cast<ofstream *>(&output);

	iListener.setRange(0,10);
	iListener.notifyProgress(0, "BitmapTriples saving Predicate count");
	predicateCount->save(*out);

	iListener.setRange(10,20);
	iListener.notifyProgress(0, "BitmapTriples saving Bitmap Index");
	bitmapIndex->save(*out);

	iListener.setRange(10,100);
	iListener.notifyProgress(0, "BitmapTriples saving Stream Index");
	streamIndex->save(*out);
}

void BitmapTriples::loadIndex(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener) {
	unsigned int numTriples = controlInformation.getUint("numTriples");
	std::string typeIndex = controlInformation.get("stream.index");

	if(this->getNumberOfElements()!=numTriples) {
		throw "The supplied index does not have the same number of triples as the dataset";
	}

	ifstream *in = dynamic_cast<ifstream *>(&input);
	IntermediateListener iListener(listener);

	// Load predicate count
	if(predicateCount!=NULL) {
		delete predicateCount;
	}
	iListener.setRange(0,10);
	iListener.notifyProgress(0, "BitmapTriples loading Predicate Count");
	predicateCount = new LogStream2();
	predicateCount->load(input);

	// LOAD BITMAP
	if(bitmapIndex!=NULL) {
		delete bitmapIndex;
	}
	iListener.setRange(10,20);
	iListener.notifyProgress(0, "BitmapTriples loading Bitmap Index");
	bitmapIndex = cds_static::BitSequence::load(*in);

	// LOAD STREAM
	if(streamIndex!=NULL) {
		delete streamIndex;
	}
	streamIndex = StreamElements::getStream(typeIndex);
	iListener.setRange(10,50);
	iListener.notifyProgress(0, "BitmapTriples loading Stream Index");
	streamIndex->load(input);

	// Make sure wavelet is generated
	iListener.setRange(50,100);
	generateWavelet(&iListener);
}

unsigned int BitmapTriples::getNumberOfElements()
{
	return numTriples;
}

unsigned int BitmapTriples::size()
{
	if(bitmapY!=NULL && bitmapZ!=NULL && predicateCount!=NULL) {
		return bitmapY->getSize()+bitmapZ->getSize()+
			streamY->size()+streamZ->size()+
			streamIndex->size()+bitmapIndex->getSize()+
			predicateCount->size();
	}
	return streamY->size()+streamZ->size()+bitmapY->getSize()+bitmapZ->getSize();
}


}



