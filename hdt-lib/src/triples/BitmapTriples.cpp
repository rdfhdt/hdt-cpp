/*
 * File: BitmapTriples.cpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * Copyright (C) 2012, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#include "BitmapTriples.hpp"

#include "TripleIterators.hpp"

#include <HDTVocabulary.hpp>

#include "../util/StopWatch.hpp"

#include <algorithm>

namespace hdt {

#define CHECK_BITMAPTRIPLES_INITIALIZED if(bitmapY==NULL || bitmapZ==NULL){	throw "Accessing uninitialized BitmapTriples"; }

BitmapTriples::BitmapTriples() : order(SPO) {
	arrayY = IntSequence::getArray(spec.get("stream.y"));
	arrayZ = IntSequence::getArray(spec.get("stream.z"));
	arrayIndex = NULL;
	bitmapY = NULL;
	bitmapZ = NULL;
	bitmapIndex = NULL;
	waveletY = NULL;
	predicateCount = NULL;
}

BitmapTriples::BitmapTriples(HDTSpecification &specification) : spec(specification) {
	std::string orderStr = spec.get("triples.component.order");
	order= parseOrder(orderStr.c_str());
	if(order==Unknown)
		order = SPO;
	arrayY = IntSequence::getArray(spec.get("stream.y"));
	arrayZ = IntSequence::getArray(spec.get("stream.z"));
	arrayIndex = NULL;
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
	if(waveletY != NULL && waveletY != arrayY) {
		delete waveletY;
	}
	if(bitmapIndex!=NULL) {
		delete bitmapIndex;
	}
	if(arrayIndex!=NULL) {
		delete arrayIndex;
	}
	if(predicateCount!=NULL) {
		delete predicateCount;
	}
	delete arrayY;
	delete arrayZ;
}

float BitmapTriples::cost(TripleID & triple)
{
	CHECK_BITMAPTRIPLES_INITIALIZED
	return 0;
}


void BitmapTriples::load(ModifiableTriples &triples, ProgressListener *listener) {
	triples.sort(order);

	IteratorTripleID *it = triples.searchAll();

	bitmapY = new BitSequence375(triples.getNumberOfElements()/2);
	bitmapZ = new BitSequence375(triples.getNumberOfElements());

	LogSequence2 *vectorY = new LogSequence2(bits(triples.getNumberOfElements()));
	LogSequence2 *vectorZ = new LogSequence2(bits(triples.getNumberOfElements()),triples.getNumberOfElements());

	unsigned int lastX, lastY, lastZ;
	unsigned int x, y, z;

	unsigned int numTriples=0;

	while(it->hasNext()) {
		TripleID *triple = it->next();
		//cout << "111> " << *triple << endl;

		swapComponentOrder(triple, SPO, order);

		x = triple->getSubject();
		y = triple->getPredicate();
		z = triple->getObject();
		if(x==0 || y==0 || z==0) {
			cerr << "ERROR: Triple with at least one component zero." << endl;
			continue;
		}

		if(numTriples==0){
			vectorY->push_back(y);
			vectorZ->push_back(z);
		} else if(x!=lastX) {
            if(x!=lastX+1) {
                throw "Error: The subjects must be correlative.";
            }
			bitmapY->append(true);
			vectorY->push_back(y);

			bitmapZ->append(true);
			vectorZ->push_back(z);
		} else if(y!=lastY) {
            if(y<lastY) {
                throw "Error: The predicates must be in increasing order.";
            }
            bitmapY->append(false);
			vectorY->push_back(y);

			bitmapZ->append(true);
			vectorZ->push_back(z);
		} else {
            if(z<=lastZ) {
                throw "Error, The objects must be in increasing order.";
            }
            bitmapZ->append(false);
			vectorZ->push_back(z);
		}

		lastX = x;
		lastY = y;
		lastZ = z;

        NOTIFYCOND(listener, "Converting to BitmapTriples", numTriples, triples.getNumberOfElements());
		numTriples++;
	}

	bitmapY->append(true);
	bitmapZ->append(true);

	delete it;

	vectorY->reduceBits();

	delete arrayY;
	arrayY = vectorY;

	delete arrayZ;
	arrayZ = vectorZ;

#if 0
	AdjacencyList adjY(arrayY, bitmapY);
	AdjacencyList adjZ(arrayZ, bitmapZ);
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
	if(arrayY->getType()==HDTVocabulary::SEQ_TYPE_WAVELET) {
		waveletY = reinterpret_cast<WaveletSequence *>(arrayY);
	} else {
		waveletY = new WaveletSequence(arrayY);
#if 0
        // FIXME: Substitute existing or leave both?
        delete arrayY;
        arrayY = waveletY;
#endif
	}
}

void BitmapTriples::generateIndex(ProgressListener *listener) {
	StopWatch st;

	IntermediateListener iListener(listener);
	iListener.setRange(0,40);

    cout << "Generate Object Index" << endl;
    cout << " Gather object lists..." << endl;

	// For each object, a list of (zpos, predicate)
	vector<vector<pair<unsigned int, unsigned int> > > index;
	int maxpred = 0;
	for(unsigned int i=0;i<arrayZ->getNumberOfElements(); i++) {
		unsigned int val = arrayZ->get(i);
        if(val==0) {
            cerr << "ERROR: There is a zero value in the Z level." << endl;
            continue;
        }
		if(index.size()<val) {
			index.resize(val);
		}
		unsigned int adjZlist = i>0 ?  bitmapZ->rank1(i-1) : 0;

		//cout << "Item " << i << " in adjlist " << adjZlist << endl;
		unsigned int pred = arrayY->get(adjZlist);
		maxpred = pred>maxpred ? pred : maxpred;

        index[val-1].push_back(std::make_pair(adjZlist, pred));
		NOTIFYCOND(&iListener, "Generating Object lists", i, arrayZ->getNumberOfElements());
	}

	if(predicateCount!=NULL) {
		delete predicateCount;
	}
	predicateCount = new LogSequence2(bits(arrayZ->getNumberOfElements()), maxpred);
	for(int i=0;i<maxpred;i++ ){
		predicateCount->push_back(0);
	}

	// Generate list and bitmap.
	if(bitmapIndex!=NULL) {
		delete bitmapIndex;
	}
	bitmapIndex = new BitSequence375(arrayZ->getNumberOfElements());

    cout << " Serialize object lists..." << endl;
	iListener.setRange(40, 80);
	unsigned int pos=0;
	unsigned int numBits = bits(arrayY->getNumberOfElements());
	unsigned int numElements = arrayZ->getNumberOfElements();
	LogSequence2 *stream = new LogSequence2(numBits, numElements );
	arrayIndex = stream;
	for(unsigned int i=0;i<index.size();i++){
		if(index[i].size()<=0) {
			cerr << "Error, object "<< i << " never appears" << endl;
			throw "Error generating index: Object should appear at least once";
		}

		// Sort by predicate of this object
		std::sort(index[i].begin(), index[i].end(), sort_pred());

		for(unsigned int j=0;j<index[i].size();j++){
			stream->push_back(index[i][j].first);
			predicateCount->set(index[i][j].second-1, predicateCount->get(index[i][j].second-1)+1);

			if(j==index[i].size()-1) {
				// Last element of the list
				bitmapIndex->set(pos, true);
			} else {
				// Elements inside the list
				bitmapIndex->set(pos, false);
			}
			pos++;
		}
		NOTIFYCOND(&iListener, "Serializing object lists", i, index.size());
	}
	index.clear();

	predicateCount->reduceBits();

	cout << "Index generated in " << st << endl;

	// Generate Wavelet
	st.reset();
	generateWavelet();
	cout << "Wavelet generated in " << st << endl;

	cout << "Num triples: " << getNumberOfElements() << endl;
	cout << "Order: " << getOrderStr(order) << endl;
	cout << "Original triples size: " << size() << endl;
	cout << "Stream size: " << arrayIndex->size() << " <" << ((unsigned long long)(arrayIndex->size())*100) / size() << "%>"<< endl;
	cout << "Bitmap Object size: " << bitmapIndex->getSizeBytes() << " <" << ((unsigned long long)(bitmapIndex->getSizeBytes()))*100 / size() << "%>"<< endl;

    cout << "Total Index size: " << bitmapIndex->getSizeBytes()+arrayIndex->size() << " <" << ((unsigned long long)(bitmapIndex->getSizeBytes()+arrayIndex->size()))*100 / size() << "%>"<< endl;
    cout << "Total size: " << size()+bitmapIndex->getSizeBytes()+arrayIndex->size() << endl;

	cout << "Number of lists: " << bitmapZ->countOnes() << " Bits: " << bits(bitmapZ->countOnes()) << endl;
}

void BitmapTriples::populateHeader(Header &header, string rootNode) {
	header.insert(rootNode, HDTVocabulary::TRIPLES_TYPE, getType());
	header.insert(rootNode, HDTVocabulary::TRIPLES_NUM_TRIPLES, getNumberOfElements() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_ORDER, getOrderStr(order) );
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMY_TYPE, arrayY->getType() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMZ_TYPE, arrayZ->getType() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMY_SIZE, arrayY->size() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_STREAMZ_SIZE, arrayZ->size() );
	if(bitmapY!=NULL) {
		header.insert(rootNode, HDTVocabulary::TRIPLES_BITMAPY_SIZE, bitmapY->getSizeBytes() );
	}
	if(bitmapZ!=NULL) {
		header.insert(rootNode, HDTVocabulary::TRIPLES_BITMAPZ_SIZE, bitmapZ->getSizeBytes() );
	}
}

string BitmapTriples::getType() {
    return HDTVocabulary::TRIPLES_TYPE_BITMAP;
}

TripleComponentOrder BitmapTriples::getOrder()
{
    return order;
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
	    } else if( (this->order == OPS) && (arrayIndex!=NULL)) {
		return new SequentialSearchIteratorTripleID(pattern, new ObjectIndexIterator(this, pattern));
	    }
	}

	if((arrayIndex!=NULL) && (patternString=="??O" || patternString=="?PO" )) {
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

    controlInformation.setUint("numTriples", getNumberOfElements());
	controlInformation.set("codification", getType());
	controlInformation.setUint("componentOrder", order);
	controlInformation.set("array.y", arrayY->getType());
	controlInformation.set("array.z", arrayZ->getType());
	controlInformation.save(output);

	// Fixme: Bitmap directly on istream/ostream??
	IntermediateListener iListener(listener);
	iListener.setRange(0,5);
	iListener.notifyProgress(0, "BitmapTriples saving Bitmap Y");
	bitmapY->save(output);

	iListener.setRange(5,15);
	iListener.notifyProgress(0, "BitmapTriples saving Bitmap Z");
	bitmapZ->save(output);

	iListener.setRange(15,30);
	iListener.notifyProgress(0, "BitmapTriples saving Stream Y");
	arrayY->save(output);

	iListener.setRange(30,100);
	iListener.notifyProgress(0, "BitmapTriples saving Stream Z");
	arrayZ->save(output);
}

void BitmapTriples::load(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener)
{
	order = (TripleComponentOrder) controlInformation.getUint("componentOrder");

	std::string typeY = controlInformation.get("array.y");
	std::string typeZ = controlInformation.get("array.z");

	delete arrayY;
	delete arrayZ;

	arrayY = IntSequence::getArray(typeY);
	arrayZ = IntSequence::getArray(typeZ);

	IntermediateListener iListener(listener);

	iListener.setRange(0,5);
	iListener.notifyProgress(0, "BitmapTriples loading Bitmap Y");
	bitmapY = BitSequence375::load(input);
	if(bitmapY==NULL){
		throw "Could not read bitmapY.";
	}

	iListener.setRange(5,10);
	iListener.notifyProgress(0, "BitmapTriples loading Bitmap Z");
	bitmapZ = BitSequence375::load(input);
	if(bitmapZ==NULL){
		throw "Could not read bitmapZ.";
	}

	iListener.setRange(10,20);
	iListener.notifyProgress(0, "BitmapTriples loading Stream Y");
	arrayY->load(input);

	iListener.setRange(20,50);
	iListener.notifyProgress(0, "BitmapTriples loading Stream Z");
    arrayZ->load(input);

	if(arrayZ->getType()==HDTVocabulary::SEQ_TYPE_WAVELET) {
		waveletY = reinterpret_cast<WaveletSequence *>(arrayZ);
    }
}

size_t BitmapTriples::load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener)
{
    size_t count=0;

    controlInformation.clear();
    count += controlInformation.load(&ptr[count], ptrMax);

    order = (TripleComponentOrder) controlInformation.getUint("componentOrder");

    BitSequence375 *bitY = new BitSequence375();
    BitSequence375 *bitZ = new BitSequence375();

    std::string typeY = controlInformation.get("array.y");
    std::string typeZ = controlInformation.get("array.z");

    IntSequence *aY = IntSequence::getArray(typeY);
    IntSequence *aZ = IntSequence::getArray(typeZ);

    IntermediateListener iListener(listener);

    iListener.setRange(0,5);
    iListener.notifyProgress(0, "BitmapTriples loading Bitmap Y");
    count += bitY->load(&ptr[count], ptrMax, listener);

    iListener.setRange(5,10);
    iListener.notifyProgress(0, "BitmapTriples loading Bitmap Z");
    count += bitZ->load(&ptr[count], ptrMax, listener);

    iListener.setRange(10,20);
    iListener.notifyProgress(0, "BitmapTriples loading Stream Y");
    count += aY->load(&ptr[count], ptrMax, listener);

    iListener.setRange(20,50);
    iListener.notifyProgress(0, "BitmapTriples loading Stream Z");
    count += aZ->load(&ptr[count], ptrMax, listener);

    delete bitmapY;
    delete bitmapZ;
    delete arrayY;
    delete arrayZ;

    bitmapY = bitY;
    bitmapZ = bitZ;
    arrayY = aY;
    arrayZ = aZ;

    return count;
}

void BitmapTriples::saveIndex(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener) {
	IntermediateListener iListener(listener);

    iListener.setRange(10,50);
	if(arrayIndex==NULL || bitmapIndex==NULL) {
		generateIndex(&iListener);
	}

	controlInformation.clear();
	controlInformation.setIndex(true);
	controlInformation.setUint("numTriples", getNumberOfElements());
	controlInformation.set("stream.index", arrayIndex->getType());
	controlInformation.save(output);

    iListener.setRange(50,60);
	iListener.notifyProgress(0, "BitmapTriples saving Predicate count");
	predicateCount->save(output);

    iListener.setRange(60,70);
	iListener.notifyProgress(0, "BitmapTriples saving Bitmap Index");
	bitmapIndex->save(output);

    iListener.setRange(70,100);
	iListener.notifyProgress(0, "BitmapTriples saving Stream Index");
	arrayIndex->save(output);

    waveletY->save(output);
}

void BitmapTriples::loadIndex(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener) {
	unsigned int numTriples = controlInformation.getUint("numTriples");
	std::string typeIndex = controlInformation.get("stream.index");

	if(this->getNumberOfElements()!=numTriples) {
		throw "The supplied index does not have the same number of triples as the dataset";
	}

	IntermediateListener iListener(listener);

	// Load predicate count
	if(predicateCount!=NULL) {
		delete predicateCount;
	}
	iListener.setRange(0,10);
	iListener.notifyProgress(0, "BitmapTriples loading Predicate Count");
	predicateCount = new LogSequence2();
	predicateCount->load(input);

	// LOAD BITMAP
	if(bitmapIndex!=NULL) {
		delete bitmapIndex;
	}
	iListener.setRange(10,20);
	iListener.notifyProgress(0, "BitmapTriples loading Bitmap Index");
	bitmapIndex = BitSequence375::load(input);

	// LOAD STREAM
	if(arrayIndex!=NULL) {
		delete arrayIndex;
	}
	arrayIndex = IntSequence::getArray(typeIndex);
	iListener.setRange(10,50);
	iListener.notifyProgress(0, "BitmapTriples loading Stream Index");
	arrayIndex->load(input);

	// Make sure wavelet is generated
	iListener.setRange(50,100);
    if(! input.eof() ) {
        waveletY = new WaveletSequence();
        waveletY->load(input);
    } else {
        generateWavelet(&iListener);
    }
}

size_t BitmapTriples::loadIndex(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener)
{
    IntermediateListener iListener(listener);

    size_t count=0;
    controlInformation.clear();
    count += controlInformation.load(&ptr[count], ptrMax);

    unsigned int numTriples = controlInformation.getUint("numTriples");
    std::string typeIndex = controlInformation.get("stream.index");

    if(this->getNumberOfElements()!=numTriples) {
        throw "The supplied index does not have the same number of triples as the dataset";
    }

    // LOAD PREDICATES
    iListener.setRange(0,10);
    iListener.notifyProgress(0, "BitmapTriples loading Predicate Count");
    LogSequence2 *pCount = new LogSequence2();
    count += pCount->load(&ptr[count], ptrMax, &iListener);

    if(predicateCount) {
        delete predicateCount;
    }
    predicateCount = pCount;

    // LOAD BITMAP
    iListener.setRange(10,20);
    iListener.notifyProgress(0, "BitmapTriples loading Bitmap Index");
    if(bitmapIndex!=NULL) {
        delete bitmapIndex;
    }
    BitSequence375 *bitIndex = new BitSequence375();
    count += bitIndex->load(&ptr[count], ptrMax, &iListener);
    bitmapIndex = bitIndex;

    // LOAD STREAM
    iListener.setRange(10,50);
    iListener.notifyProgress(0, "BitmapTriples loading Stream Index");
    if(arrayIndex!=NULL) {
        delete arrayIndex;
    }
    LogSequence2 *arrIndex = new LogSequence2();
    count += arrIndex->load(&ptr[count], ptrMax, &iListener);
    arrayIndex = arrIndex;

    iListener.setRange(50,100);
    if(arrayY->getType()==HDTVocabulary::SEQ_TYPE_WAVELET) {
        waveletY = reinterpret_cast<WaveletSequence *>(arrayY);
    } else if(&ptr[count]<ptrMax) {
        iListener.notifyProgress(0, "BitmapTriples loading Wavelet");

        waveletY = new WaveletSequence();
        count += waveletY->load(&ptr[count], ptrMax, &iListener);
    } else {
        iListener.notifyProgress(0, "BitmapTriples generating Wavelet");
        waveletY = new WaveletSequence(arrayY);
    }
    return count;
}

unsigned int BitmapTriples::getNumberOfElements()
{
	return arrayZ->getNumberOfElements();
}

size_t BitmapTriples::size()
{
	if(bitmapY!=NULL && bitmapZ!=NULL && predicateCount!=NULL) {
		return bitmapY->getSizeBytes()
				+bitmapZ->getSizeBytes()
				+arrayY->size()
				+arrayZ->size()
				+arrayIndex->size()
				+bitmapIndex->getSizeBytes();
				//+predicateCount->size();
	}
	return arrayY->size()+arrayZ->size()+bitmapY->getSizeBytes()+bitmapZ->getSizeBytes();
}


}
