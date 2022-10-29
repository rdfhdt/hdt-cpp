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
#include <stdexcept>
#include "BitmapTriples.hpp"

#include "TripleIterators.hpp"

#include <HDTVocabulary.hpp>

#include "../util/StopWatch.hpp"

#include <algorithm>

namespace hdt {

#define CHECK_BITMAPTRIPLES_INITIALIZED if(bitmapY==NULL || bitmapZ==NULL){	throw std::runtime_error("Accessing uninitialized BitmapTriples"); }

BitmapTriples::BitmapTriples() : order(SPO) {
	string typey=spec.getOrEmpty("stream.y");
	string typez=spec.getOrEmpty("stream.z");

	arrayY = IntSequence::getArray(typey);
	arrayZ = IntSequence::getArray(typez);
	arrayIndex = NULL;
	bitmapY = NULL;
	bitmapZ = NULL;
	bitmapIndex = NULL;
    predicateIndex = NULL;
    predicateCount = NULL;
}

BitmapTriples::BitmapTriples(HDTSpecification &specification) : spec(specification) {
	std::string orderStr = spec.getOrEmpty("triplesOrder");

	order= parseOrder(orderStr.c_str());
	if(order==Unknown)
		order = SPO;
	string typey= spec.getOrEmpty("stream.y");
	string typez= spec.getOrEmpty("stream.z");

	arrayY = IntSequence::getArray(typey);
	arrayZ = IntSequence::getArray(typez);
	arrayIndex = NULL;
	bitmapY = NULL;
	bitmapZ = NULL;
	bitmapIndex = NULL;
    predicateIndex = NULL;
   	predicateCount = NULL;
}

BitmapTriples::~BitmapTriples() {
	if(bitmapY!=NULL) {
		delete bitmapY;
	}
	if(bitmapZ!=NULL) {
		delete bitmapZ;
	}
    if(predicateIndex != NULL) {
        delete predicateIndex;
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

float BitmapTriples::cost(TripleID & triple) const
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

	size_t lastX=0, lastY=0, lastZ=0;
	size_t x, y, z;

    size_t numTriples=0;

	while(it->hasNext()) {
		TripleID *triple = it->next();

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
                throw std::runtime_error("Error: The subjects must be correlative.");
            }
			bitmapY->append(true);
			vectorY->push_back(y);

			bitmapZ->append(true);
			vectorZ->push_back(z);
		} else if(y!=lastY) {
            if(y<lastY) {
                throw std::runtime_error("Error: The predicates must be in increasing order.");
            }
            bitmapY->append(false);
			vectorY->push_back(y);

			bitmapZ->append(true);
			vectorZ->push_back(z);
		} else {
            if(z<=lastZ) {
                throw std::runtime_error("Error, The objects must be in increasing order.");
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
    	if(left.second==right.second) {
    		return left.first < right.first;
    	}
        return left.second < right.second;
    }
};

// function object
class PredicateComparator : public std::binary_function<unsigned int,unsigned int,bool>
{
private:
	IntSequence *array;

public:
	PredicateComparator(IntSequence *array) : array(array) {

	}
	~PredicateComparator() {

	}

	inline bool operator()(const unsigned int a, const unsigned int b)
	{
		size_t x =  array->get(a);
		size_t y =  array->get(b);

		if(x==y) {
			return a<b;
		}
		return x<y;
	}
};

void BitmapTriples::generateIndex(ProgressListener *listener) {
	StopWatch global;

    IntermediateListener iListener(listener);
    iListener.setRange(0,30);

    predicateIndex = new PredicateIndexArray(this);
    predicateIndex->generate(&iListener);

    iListener.setRange(30,100);
    generateIndexMemory(&iListener);
   	//generateIndexMemoryFast(listener);

	cerr << "Index generated in "<< global << endl;
}

void BitmapTriples::generateIndexMemory(ProgressListener *listener) {
	StopWatch st;

	IntermediateListener iListener(listener);
	iListener.setRange(0,20);

	// Count the number of appearances of each object
	LogSequence2 *objectCount = new LogSequence2(bits(arrayZ->getNumberOfElements()));
    size_t maxCount = 0;
    for(size_t i=0;i<arrayZ->getNumberOfElements(); i++) {
        size_t val = arrayZ->get(i);
		if(val==0) {
			cerr << "ERROR: There is a zero value in the Z level." << endl;
			continue;
		}
		if(objectCount->getNumberOfElements()<val) {
			objectCount->resize(val);
		}
        size_t count = objectCount->get(val-1)+1;
		maxCount = count>maxCount ? count : maxCount;
		objectCount->set(val-1, count);

		NOTIFYCOND3(&iListener, "Counting appearances of objects", i, arrayZ->getNumberOfElements(), 1000000);
	}
	cerr << "Count Objects in " << st << " Max was: " << maxCount << endl;
	st.reset();

#if 0
    for(size_t i=0;i<objectCount->getNumberOfElements();i++) {
		cerr << "Object " << (i+1) << " appears " << objectCount->get(i) << " times." << endl;
	}
#endif

	iListener.setRange(20, 25);
	// Calculate bitmap that separates each object sublist.
	bitmapIndex = new BitSequence375(arrayZ->getNumberOfElements());
    size_t tmpCount=0;
    for(size_t i=0;i<objectCount->getNumberOfElements();i++) {
		tmpCount += objectCount->get(i);
		bitmapIndex->set(tmpCount-1, true);
		NOTIFYCOND3(&iListener, "Creating Index bitmap", i, objectCount->getNumberOfElements(), 1000000);
	}
	if (arrayZ->getNumberOfElements())
	  bitmapIndex->set(arrayZ->getNumberOfElements()-1, true);
	delete objectCount;
	objectCount=NULL;
	cerr << "Bitmap in " << st << endl;
	st.reset();

    cerr << "Bitmap bits: " << bitmapIndex->getNumBits() << " Ones: " << bitmapIndex->countOnes() << endl;
#if 0
    for(size_t i=0;i<bitmapIndex->getNumBits();i++) {
		if(bitmapIndex->access(i)){
			cerr << "1";
		} else {
			cerr << "0";
		}
	}
	cerr << endl;

	for(int i=0;i<bitmapIndex->getNumBits();i++) {
		unsigned int pos = 0;
		if(i>0) {
			pos = bitmapIndex->select1(i)+1;
		}
		cerr << "Object " << (i+1) << " is stored at " << pos << endl;
	}
#endif

	if(predicateCount!=NULL) {
		delete predicateCount;
	}
	size_t maxpred = 0;
	LogSequence2 *predCount = new LogSequence2(bits(arrayZ->getNumberOfElements()));

	iListener.setRange(25,60);
	// Copy each object reference to its position
	LogSequence2 *objectInsertedCount = new LogSequence2(bits(maxCount), bitmapIndex->countOnes());
	objectInsertedCount->resize(bitmapIndex->countOnes());

	LogSequence2 *objectArray = new LogSequence2(bits(arrayY->getNumberOfElements()), arrayZ->getNumberOfElements());
	objectArray->resize(arrayZ->getNumberOfElements());

    for(size_t i=0;i<arrayZ->getNumberOfElements(); i++) {
			unsigned int objectValue = arrayZ->get(i);
            size_t posY = i>0 ?  bitmapZ->rank1(i-1) : 0;

            size_t insertBase = objectValue==1 ? 0 : bitmapIndex->select1(objectValue-1)+1;
            size_t insertOffset = objectInsertedCount->get(objectValue-1);
			objectInsertedCount->set(objectValue-1, insertOffset+1);

			objectArray->set(insertBase+insertOffset, posY);

            size_t yval = arrayY->get(posY)-1;
            if(yval>=maxpred) {
            	predCount->resize(yval+1);
            	maxpred = yval+1;
            	predCount->set(yval, 1);
            } else {
            	predCount->set(yval, predCount->get(yval)+1);
            }

			NOTIFYCOND3(&iListener, "Generating object references", i, arrayZ->getNumberOfElements(), 1000000);
	}
	delete objectInsertedCount;
	objectInsertedCount=NULL;
	cerr << "Object references in " << st << endl;
	st.reset();

	predCount->reduceBits();
	if(this->predicateCount!=NULL) delete this->predicateCount;
	this->predicateCount = predCount;

#if 0
    for(size_t i=0;i<objectArray->getNumberOfElements();i++) {
		cerr << "Position " << (i) << " references " << objectArray->get(i) << " ." << endl;
		if(bitmapIndex->access(i)) {
			cerr << endl;
		}
	}
#endif

    iListener.setRange(60,100);
	unsigned int object=1;
	do {
		size_t first = object==1 ? 0 : bitmapIndex->select1(object-1)+1;
		size_t last = bitmapIndex->select1(object)+1;
		size_t listLen = last-first;

		// Sublists of one element do not need to be sorted.

		// Hard-coded size-2 for speed (They are quite common).
        if(listLen==2) {
            size_t aPos = objectArray->get(first);
            size_t a = arrayY->get(aPos);
            size_t bPos = objectArray->get(last-1);
            size_t b = arrayY->get(bPos);
			if(a>b) {
				objectArray->set(first, bPos);
                objectArray->set(last-1, aPos);
			}
        } else if(listLen>2) {
			// FIXME: Sort directly without copying?

#if 0
			vector<pair<unsigned int, unsigned int> > tempList;
			tempList.reserve(listLen);

			// Create temporary list of (position, predicate)
			for(size_t i=first; i<last;i++) {
				unsigned int adjZlist = (unsigned int)objectArray->get(i);
				unsigned int pred = arrayY->get(adjZlist);
				tempList.push_back(std::make_pair(adjZlist, pred));
			}

			// Sort
			std::sort(tempList.begin(), tempList.end(), sort_pred());

			// Copy back
			for(size_t i=first; i<last;i++) {
				objectArray->set(i, tempList[i-first].first);
			}
#else
            vector<size_t> tempList;
			tempList.reserve(listLen);

			// Create temporary list of (position, predicate)
			for(size_t i=first; i<last;i++) {
                size_t adjZlist = objectArray->get(i);
				tempList.push_back(adjZlist);
			}

			// Sort
			PredicateComparator comparator(arrayY);
			std::sort(tempList.begin(), tempList.end(), comparator);

			// Copy back
			for(size_t i=first; i<last;i++) {
				objectArray->set(i, tempList[i-first]);
			}
#endif
		}

        object++;
		NOTIFYCOND3(&iListener, "Sorting object sublists", first, arrayZ->getNumberOfElements(), 10000);
	} while(object<=bitmapIndex->countOnes());

	cerr << "Sort lists in " << st << endl;
	st.reset();

	// Save Object Index
    arrayIndex = objectArray;

#if 0
    for(size_t i=0;i<arrayIndex->getNumberOfElements();i++) {
        size_t indexPtr = arrayIndex->get(i);
        size_t pred = arrayY->get(indexPtr);

        size_t cobject = i==0 ? 1 : bitmapIndex->rank1(i-1)+1;
        size_t subject = indexPtr==0 ? 1 : bitmapY->rank1(indexPtr-1)+1;
		cerr << "\tFinal: " << i << " (" << indexPtr << " > " <<  cobject << "-" << pred << "-"<<subject<<")" << endl;

        if(bitmapIndex->access(i)) {
            cerr << endl;
        }
    }
#endif
}

void BitmapTriples::generateIndexFast(ProgressListener *listener) {
	StopWatch st;

	IntermediateListener iListener(listener);
	iListener.setRange(0,40);

    cerr << "Generate Object Index" << endl;
    cerr << " Gather object lists..." << endl;

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

		//cerr << "Item " << i << " in adjlist " << adjZlist << endl;
		unsigned int pred = arrayY->get(adjZlist);
		maxpred = ((maxpred<0) || (pred>static_cast<size_t>(maxpred))) ? pred : maxpred;

        index[val-1].push_back(std::make_pair(adjZlist, pred));
		NOTIFYCOND(&iListener, "Generating Object lists", i, arrayZ->getNumberOfElements());
	}

	if(predicateCount!=NULL) {
		delete predicateCount;
	}
	LogSequence2 *predCount = new LogSequence2(bits(arrayZ->getNumberOfElements()), maxpred);
	for(int i=0;i<maxpred;i++ ){
		predCount->push_back(0);
	}

	// Generate list and bitmap.
	if(bitmapIndex!=NULL) {
		delete bitmapIndex;
	}
	bitmapIndex = new BitSequence375(arrayZ->getNumberOfElements());

    cerr << " Serialize object lists..." << endl;
	iListener.setRange(40, 80);
	unsigned int pos=0;
	unsigned int numBits = bits(arrayY->getNumberOfElements());
	unsigned int numElements = arrayZ->getNumberOfElements();
	LogSequence2 *stream = new LogSequence2(numBits, numElements );
	arrayIndex = stream;
	for(unsigned int i=0;i<index.size();i++){
		if(index[i].size()<=0) {
			cerr << "Error, object "<< i << " never appears" << endl;
			throw std::runtime_error("Error generating index: Object should appear at least once");
		}

		// Sort by predicate of this object
		std::sort(index[i].begin(), index[i].end(), sort_pred());

		for(unsigned int j=0;j<index[i].size();j++){
			stream->push_back(index[i][j].first);
			predCount->set(index[i][j].second-1, predCount->get(index[i][j].second-1)+1);

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

	predCount->reduceBits();
	if(this->predicateCount!=NULL) delete this->predicateCount;
	this->predicateCount = predCount;

#if 1
    for(size_t i=0;i<arrayIndex->getNumberOfElements();i++) {
		unsigned int indexPtr = (unsigned int)arrayIndex->get(i);
		unsigned int pred = arrayY->get(indexPtr);

		unsigned int cobject = i==0 ? 1 : bitmapIndex->rank1(i-1)+1;
		unsigned int subject = indexPtr==0 ? 1 : bitmapY->rank1(indexPtr-1)+1;
		cerr << "\tFinal: " << i << " (" << indexPtr << " > " <<  cobject << "-" << pred << "-"<<subject<<")" << endl;

        if(bitmapIndex->access(i)) {
            cerr << endl;
        }
    }
#endif

	cerr << "Index generated in " << st << endl;

	cerr << "Num triples: " << getNumberOfElements() << endl;
	cerr << "Order: " << getOrderStr(order) << endl;
	cerr << "Original triples size: " << size() << endl;
	cerr << "Stream size: " << arrayIndex->size() << " <" << ((unsigned long long)(arrayIndex->size())*100) / size() << "%>"<< endl;
	cerr << "Bitmap Object size: " << bitmapIndex->getSizeBytes() << " <" << ((unsigned long long)(bitmapIndex->getSizeBytes()))*100 / size() << "%>"<< endl;

    cerr << "Total Index size: " << bitmapIndex->getSizeBytes()+arrayIndex->size() << " <" << ((unsigned long long)(bitmapIndex->getSizeBytes()+arrayIndex->size()))*100 / size() << "%>"<< endl;
    cerr << "Total size: " << size()+bitmapIndex->getSizeBytes()+arrayIndex->size() << endl;

	cerr << "Number of lists: " << bitmapZ->countOnes() << " Bits: " << bits(bitmapZ->countOnes()) << endl;
}

void BitmapTriples::populateHeader(Header &header, string rootNode) {
	header.insert(rootNode, HDTVocabulary::TRIPLES_TYPE, getType());
	header.insert(rootNode, HDTVocabulary::TRIPLES_NUM_TRIPLES, getNumberOfElements() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_ORDER, getOrderStr(order) );
#if 0
	header.insert(rootNode, HDTVocabulary::TRIPLES_SEQY_TYPE, arrayY->getType() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_SEQZ_TYPE, arrayZ->getType() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_SEQY_SIZE, arrayY->size() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_SEQZ_SIZE, arrayZ->size() );
	if(bitmapY!=NULL) {
		header.insert(rootNode, HDTVocabulary::TRIPLES_BITMAPY_SIZE, bitmapY->getSizeBytes() );
	}
	if(bitmapZ!=NULL) {
		header.insert(rootNode, HDTVocabulary::TRIPLES_BITMAPZ_SIZE, bitmapZ->getSizeBytes() );
	}
#endif
}

string BitmapTriples::getType() const
{
    return HDTVocabulary::TRIPLES_TYPE_BITMAP;
}

TripleComponentOrder BitmapTriples::getOrder() const
{
    return order;
}

IteratorTripleID *BitmapTriples::search(TripleID & pattern)
{
	CHECK_BITMAPTRIPLES_INITIALIZED

	TripleID reorderedPat = pattern;
	swapComponentOrder(&reorderedPat, SPO, this->order);
	std::string patternString = reorderedPat.getPatternString();

	if(patternString=="?P?") {
		if(predicateIndex!=NULL) {
			return new MiddleWaveletIterator(this, pattern);
		} else {
			return new IteratorY(this, pattern);
		}
	}

	if(patternString=="S?O") {
	    if(this->order == SPO) {
		return new SequentialSearchIteratorTripleID(pattern, new BitmapTriplesSearchIterator(this, pattern));
	    } else if( (this->order == OPS) && (arrayIndex!=NULL)) {
		return new SequentialSearchIteratorTripleID(pattern, new ObjectIndexIterator(this, pattern));
	    }
	}

	if((arrayIndex!=NULL) && (patternString=="??O" || patternString=="?PO" )) {
		return new ObjectIndexIterator(this, pattern);
    } else if( predicateIndex != NULL && patternString=="?P?") {
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

	controlInformation.setFormat(getType());
	controlInformation.setUint("order", order);
	controlInformation.save(output);

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
	std::string format = controlInformation.getFormat();
	if(format!=getType()) {
		throw std::runtime_error("Trying to read a BitmapTriples but the data is not BitmapTriples");
	}

	order = (TripleComponentOrder) controlInformation.getUint("order");

	IntermediateListener iListener(listener);

	iListener.setRange(0,5);
	iListener.notifyProgress(0, "BitmapTriples loading Bitmap Y");
	bitmapY = BitSequence375::load(input);
	if(bitmapY==NULL){
		throw std::runtime_error("Could not read bitmapY.");
	}

	iListener.setRange(5,10);
	iListener.notifyProgress(0, "BitmapTriples loading Bitmap Z");
	bitmapZ = BitSequence375::load(input);
	if(bitmapZ==NULL){
		throw std::runtime_error("Could not read bitmapZ.");
	}

	iListener.setRange(10,20);
	iListener.notifyProgress(0, "BitmapTriples loading Array Y");
	delete arrayY;
	arrayY = IntSequence::getArray(input);
	arrayY->load(input);

	iListener.setRange(20,50);
	iListener.notifyProgress(0, "BitmapTriples loading Array Z");
	delete arrayZ;
	arrayZ = IntSequence::getArray(input);
    arrayZ->load(input);
}

size_t BitmapTriples::load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener)
{
    size_t count=0;

    controlInformation.clear();
    count += controlInformation.load(&ptr[count], ptrMax);

	std::string format = controlInformation.getFormat();
	if(format!=getType()) {
		throw std::runtime_error("Trying to read a FourSectionDictionary but the data is not FourSectionDictionary");
	}

    order = (TripleComponentOrder) controlInformation.getUint("order");

    BitSequence375 *bitY = new BitSequence375();
    BitSequence375 *bitZ = new BitSequence375();

    IntermediateListener iListener(listener);

    iListener.setRange(0,5);
    iListener.notifyProgress(0, "BitmapTriples loading Bitmap Y");
    count += bitY->load(&ptr[count], ptrMax, listener);

    iListener.setRange(5,10);
    iListener.notifyProgress(0, "BitmapTriples loading Bitmap Z");
    count += bitZ->load(&ptr[count], ptrMax, listener);

    iListener.setRange(10,20);
    iListener.notifyProgress(0, "BitmapTriples loading Sequence Y");
    IntSequence *aY = IntSequence::getArray(ptr[count]);
    count += aY->load(&ptr[count], ptrMax, listener);

    iListener.setRange(20,50);
    iListener.notifyProgress(0, "BitmapTriples loading Sequence Z");
    IntSequence *aZ = IntSequence::getArray(ptr[count]);
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
    controlInformation.setType(INDEX);
	controlInformation.setUint("numTriples", getNumberOfElements());
	controlInformation.setUint("order", getOrder());
	controlInformation.setFormat(HDTVocabulary::INDEX_TYPE_FOQ);
	controlInformation.save(output);
    iListener.setRange(60,70);
	iListener.notifyProgress(0, "BitmapTriples saving Bitmap Index");
	bitmapIndex->save(output);

    iListener.setRange(70,90);
	iListener.notifyProgress(0, "BitmapTriples saving Array Index");
	arrayIndex->save(output);

    iListener.setRange(90,100);
	iListener.notifyProgress(0, "BitmapTriples saving Predicate Index");
    predicateIndex->save(output);

    iListener.setRange(50,60);
	iListener.notifyProgress(0, "BitmapTriples saving Predicate count");
	predicateCount->save(output);
}

void BitmapTriples::loadIndex(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener) {
    size_t numTriples = controlInformation.getUint("numTriples");

	if(controlInformation.getType()!=INDEX) {
		throw std::runtime_error("Trying to read Index but data is not index.");
	}

	if(controlInformation.getFormat()!=HDTVocabulary::INDEX_TYPE_FOQ) {
		throw std::runtime_error("Error reading index. Please delete .hdt.index and let application generate it again.");
	}

	if(this->getNumberOfElements()!=numTriples) {
		// FIXME: Force index regeneration instead of error.
		throw std::runtime_error("The supplied index does not have the same number of triples as the dataset");
	}

	if(this->getOrder()!=controlInformation.getUint("order")) {
		throw std::runtime_error("The order of the triples is different than the index.");
	}

	IntermediateListener iListener(listener);

	// LOAD BITMAP
	if(bitmapIndex!=NULL) {
		delete bitmapIndex;
	}
	iListener.setRange(10,20);
	iListener.notifyProgress(0, "BitmapTriples loading Bitmap Index");
	bitmapIndex = BitSequence375::load(input);

	// LOAD SEQ
	if(arrayIndex!=NULL) {
		delete arrayIndex;
	}
	arrayIndex = IntSequence::getArray(input);
	iListener.setRange(10,50);
	iListener.notifyProgress(0, "BitmapTriples loading Array Index");
	arrayIndex->load(input);

    // TODO: Guess type from file
	iListener.setRange(10,50);
	iListener.notifyProgress(0, "BitmapTriples loading Predicate Index");
    predicateIndex = new PredicateIndexArray(this);
    predicateIndex->load(input,&iListener);

	// Load predicate count
	if(predicateCount!=NULL) {
		delete predicateCount;
	}
	iListener.setRange(0,10);
	iListener.notifyProgress(0, "BitmapTriples loading Predicate Count");
	predicateCount = IntSequence::getArray(input);
	predicateCount->load(input);
}

size_t BitmapTriples::loadIndex(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener)
{
    IntermediateListener iListener(listener);

    size_t count=0;
    controlInformation.clear();
    count += controlInformation.load(&ptr[count], ptrMax);

    if(controlInformation.getType()!=INDEX) {
        throw std::runtime_error("Trying to load an HDT Index, but the ControlInformation states that it's not an index.");
    }

    size_t numTriples = controlInformation.getUint("numTriples");
    std::string typeIndex = spec.getOrEmpty("stream.index");

    if(this->getNumberOfElements()!=numTriples) {
    	// FIXME: Force index regeneration instead of error.
        throw std::runtime_error("The supplied index does not have the same number of triples as the dataset");
    }

    // Load Bitmap
    iListener.setRange(10,20);
    iListener.notifyProgress(0, "BitmapTriples loading Bitmap Index");
    if(bitmapIndex!=NULL) {
        delete bitmapIndex;
    }
    BitSequence375 *bitIndex = new BitSequence375();
    count += bitIndex->load(&ptr[count], ptrMax, &iListener);
    bitmapIndex = bitIndex;

    // Load Seq
    iListener.setRange(20,50);
    iListener.notifyProgress(0, "BitmapTriples loading Array Index");
    if(arrayIndex!=NULL) {
        delete arrayIndex;
    }
    LogSequence2 *arrIndex = new LogSequence2();
    count += arrIndex->load(&ptr[count], ptrMax, &iListener);
    arrayIndex = arrIndex;

    // Load Predicate Index. TODO: Guess type from file
    iListener.setRange(50,100);
    iListener.notifyProgress(0, "BitmapTriples loading Predicate Index");
    predicateIndex = new PredicateIndexArray(this);
    count += predicateIndex->load(&ptr[count], ptrMax, &iListener);

    // Load predicate counts
    iListener.setRange(0,10);
    iListener.notifyProgress(0, "BitmapTriples loading Predicate Count");
    LogSequence2 *pCount = new LogSequence2();
    count += pCount->load(&ptr[count], ptrMax, &iListener);

    if(predicateCount) {
        delete predicateCount;
    }
    predicateCount = pCount;

    return count;
}

size_t BitmapTriples::getNumberOfElements() const
{
	return arrayZ->getNumberOfElements();
}

size_t BitmapTriples::size() const
{
    if(bitmapY!=NULL && bitmapZ!=NULL) {
		return bitmapY->getSizeBytes()
				+bitmapZ->getSizeBytes()
				+arrayY->size()
                +arrayZ->size();
                //+arrayIndex->size()
                //+bitmapIndex->getSizeBytes();
	}
	return arrayY->size()+arrayZ->size()+bitmapY->getSizeBytes()+bitmapZ->getSizeBytes();
}

} // namespace hdt
