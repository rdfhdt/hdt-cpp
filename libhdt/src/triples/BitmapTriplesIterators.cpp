/*
 * File: BitmapTriplesIterators.cpp
 * Last modified: $Date: 2011-08-21 05:35:30 +0100 (dom, 21 ago 2011) $
 * Revision: $Revision: 180 $
 * Last modified by: $Author: mario.arias $
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


namespace hdt {

#define SAVE_ADJ_LIST

/// ITERATOR
BitmapTriplesSearchIterator::BitmapTriplesSearchIterator(BitmapTriples *trip, TripleID &pat) :
    triples(trip),
    pattern(pat),
    adjY(trip->arrayY, trip->bitmapY),
    adjZ(trip->arrayZ, trip->bitmapZ)
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
    //cout << returnTriple << endl;
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
                    //maxZ = adjZ.findNext(minZ);
                }
            } catch (std::exception& e) {
                // Item not found in list, no results.
                minY = minZ = maxY = maxZ = 0;
            }
        } else {
            // S ? X
            minY = adjY.find(patX-1);
            minZ = adjZ.find(minY);
            maxY = adjY.last(patX-1)+1;
            //maxY = adjY.findNext(patX-1);
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

    //cout << "findRange: Y(" << minY <<  ", " << maxY << ") Z(" << minZ <<", " << maxZ << ")" << endl;
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
	//nextZ = adjZ.findNext(nextZ)+1;

        if(posY==nextY) {
	    x++;
	    nextY = adjY.find(x);
	    //nextY = adjY.findNext(nextY)+1;
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
    goToY();
}

void BitmapTriplesSearchIterator::goToY(){
	//we assume posZ is positioned
	if(posZ<maxZ) {
	    	posY = adjZ.findListIndex(posZ);

	    	z = adjZ.get(posZ);
	    	y = adjY.get(posY);
	    	x = adjY.findListIndex(posY)+1;

	    	nextY = adjY.last(x-1)+1;
	    	nextZ = adjZ.last(posY)+1;
	    }
}

size_t BitmapTriplesSearchIterator::estimatedNumResults()
{
    return maxZ-minZ;
}

ResultEstimationType BitmapTriplesSearchIterator::numResultEstimation()
{
    if(patX!=0 && patY==0 && patZ!=0) {
        return UP_TO;
    }
    return EXACT;
}

bool BitmapTriplesSearchIterator::canGoTo() {
    return true;
}

void BitmapTriplesSearchIterator::goTo(unsigned int pos) {
	if ((posZ + pos) >= maxZ) {
			throw std::runtime_error("Cannot goTo on this pattern.");
	}
	posZ += pos; // move the position of Z
	goToY(); // go to the correct Y
}

TripleComponentOrder BitmapTriplesSearchIterator::getOrder() {
    return triples->order;
}

bool BitmapTriplesSearchIterator::findNextOccurrence(unsigned int value, unsigned char component) {
    if(triples->order==SPO) {
	if(component==3 && patY!=0) {

	}

	if(component==2 && patX!=0) {

	}
    } else if(triples->order==OPS) {
	if(component==1 && patY!=0) {

	}

	if(component==2 && patX!=0) {

	}

    }
    return false;
}

bool BitmapTriplesSearchIterator::isSorted(TripleComponentRole role) {
    if(triples->order==SPO) {
	switch(role) {
	case SUBJECT:
	    return true;
	case PREDICATE:
	    return patX!=0;
	case OBJECT:
	    return patX!=0 && patY!=0;
	}
    } else if(triples->order==OPS) {
	switch(role) {
	case OBJECT:
	    return true;
	case PREDICATE:
	    return patX!=0;
	case SUBJECT:
	    return patX!=0 && patY!=0;
	}
    }


    throw std::runtime_error("Order not supported");
}



MiddleWaveletIterator::MiddleWaveletIterator(BitmapTriples *trip, TripleID &pat) :
    triples(trip),
    pattern(pat),
    adjY(trip->arrayY, trip->bitmapY),
    adjZ(trip->arrayZ, trip->bitmapZ),
    predicateIndex(trip->predicateIndex),
    predicateOcurrence(1)
{
    // Convert pattern to local order.
    swapComponentOrder(&pattern, SPO, triples->order);
    patX = pattern.getSubject();
    patY = pattern.getPredicate();
    patZ = pattern.getObject();

    if(patY==0) {
        throw std::runtime_error("This iterator is not suitable for this pattern");
    }

#if 0
    cout << "AdjY: " << endl;
    adjY.dump();
    cout << "AdjZ: " << endl;
    adjZ.dump();
    cout << "Pattern: " << patX << " " << patY << " " << patZ << endl;
#endif

    // Find largest position of Z
    maxZ = trip->arrayZ->getNumberOfElements();

    // Find position of the first matching pattern.
    numOcurrences = predicateIndex->getNumAppearances(patY);

    goToStart();
}

void MiddleWaveletIterator::updateOutput() {
    // Convert local order to SPO
    returnTriple.setAll(x,y,z);

    swapComponentOrder(&returnTriple, triples->order, SPO);
}

bool MiddleWaveletIterator::hasNext()
{
    return posZ<maxZ && (predicateOcurrence<numOcurrences || posZ <= nextZ);
}

TripleID *MiddleWaveletIterator::next()
{
    //cout << "nextTriple: " << predicateOcurrence << ", " << prevZ << ", " << posZ << ", " << nextZ << endl;
    if(posZ>nextZ) {
        predicateOcurrence++;
        posY = predicateIndex->getAppearance(patY, predicateOcurrence);

        posZ = prevZ = adjZ.find(posY);
        nextZ = adjZ.last(posY);
        //nextZ = adjZ.findNext(prevZ)-1;

        x = adjY.findListIndex(posY)+1;
        y = adjY.get(posY);
        z = adjZ.get(posZ);
    } else {
        z = adjZ.get(posZ);
    }
    posZ++;
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
        posY = predicateIndex->getAppearance(patY, predicateOcurrence);

        prevZ = adjZ.find(posY);
        posZ = nextZ = adjZ.last(posY);
        //nextZ = adjZ.findNext(prevZ)-1;

        x = adjY.findListIndex(posY)+1;
        y = adjY.get(posY);
        z = adjZ.get(posZ);
    } else {
        z = adjZ.get(posZ);
        posZ--;
    }
    updateOutput();
    return &returnTriple;
}

void MiddleWaveletIterator::goToStart()
{
    predicateOcurrence = 1;
    posY = predicateIndex->getAppearance(patY, predicateOcurrence);

    posZ = prevZ = adjZ.find(posY);
    nextZ = adjZ.last(posY);
    //nextZ = adjZ.findNext(prevZ)-1;

    x = adjY.findListIndex(posY)+1;
    y = adjY.get(posY);
    z = adjZ.get(posZ);
}

size_t MiddleWaveletIterator::estimatedNumResults()
{
	if (triples->predicateCount!=NULL){
		return triples->predicateCount->get(patY-1);
	}
    return predicateIndex->getNumAppearances(patY);
}

ResultEstimationType MiddleWaveletIterator::numResultEstimation()
{
    if(triples->predicateIndex!=NULL && triples->predicateCount!=NULL) {
	return EXACT;
    }
    return APPROXIMATE;
}

TripleComponentOrder MiddleWaveletIterator::getOrder() {
    return triples->order;
}

bool MiddleWaveletIterator::findNextOccurrence(unsigned int value, unsigned char component) {
    if(component==1) {
        // Search subject, check each predicate ocurrence.
        while(x!=value) {
            predicateOcurrence++;
            if(predicateOcurrence>numOcurrences) {  // FIXME CHECK COMP
                return false;
            }
            posY = predicateIndex->getAppearance(patY, predicateOcurrence);
            x = adjY.findListIndex(posY)+1;
        }

        prevZ = adjZ.find(posY);
        nextZ = adjZ.last(posY);

        y = adjY.get(posY);
        z = adjZ.get(posZ);
    } else if(component==3) {
        // Search Object
        while(z!=value) {
            predicateOcurrence++;
            if(predicateOcurrence>numOcurrences) {  // FIXME CHECK COMP
                return false;
            }
            posY = predicateIndex->getAppearance(patY, predicateOcurrence);
            try {
                posZ = adjZ.find(posY, value);

                // If found:
                posY = adjZ.findListIndex(posZ);

                prevZ = adjZ.find(posY);
                nextZ = adjZ.last(posY);

                y = adjY.get(posY);
                x = adjY.findListIndex(posY)+1;
                return true;
            } catch (std::exception& e) {
                // Not found
            }
        }
    }
    throw std::runtime_error("Cannot search component");
}

bool MiddleWaveletIterator::isSorted(TripleComponentRole role) {
    if(triples->order==SPO) {
	switch(role) {
	case SUBJECT:
	case PREDICATE:
	    return true;
	case OBJECT:
	    return false;
	}
    } else if(triples->order==OPS) {
	switch(role) {
	case OBJECT:
	case PREDICATE:
	    return true;
	case SUBJECT:
	    return false;
	}
    }

    throw std::runtime_error("Order not supported");
}







IteratorY::IteratorY(BitmapTriples *trip, TripleID &pat) :
    triples(trip),
    pattern(pat),
    adjY(trip->arrayY, trip->bitmapY),
    adjZ(trip->arrayZ, trip->bitmapZ)
{
    // Convert pattern to local order.
    swapComponentOrder(&pattern, SPO, triples->order);
    patX = pattern.getSubject();
    patY = pattern.getPredicate();
    patZ = pattern.getObject();

    if(patY==0) {
        throw std::runtime_error("This iterator is not suitable for this pattern");
    }

#if 0
    cout << "AdjY: " << endl;
    adjY.dump();
    cout << "AdjZ: " << endl;
    adjZ.dump();
    cout << "Pattern: " << patX << " " << patY << " " << patZ << endl;
#endif

    goToStart();
}

void IteratorY::updateOutput() {
    // Convert local order to SPO
    returnTriple.setAll(x,y,z);

    swapComponentOrder(&returnTriple, triples->order, SPO);
}

bool IteratorY::hasNext()
{
    return nextY!=-1 || posZ<=nextZ;
}

TripleID *IteratorY::next()
{
	if(posZ>nextZ) {
		prevY = posY;
		posY = nextY;
		nextY = adjY.findNextAppearance(nextY+1, patY);

		posZ = prevZ = adjZ.find(posY);
		nextZ = adjZ.last(posY);

		x = adjY.findListIndex(posY)+1;
		y = adjY.get(posY);
		z = adjZ.get(posZ);
	} else {
		z = adjZ.get(posZ);
	}
	posZ++;

	updateOutput();

	return &returnTriple;
}

bool IteratorY::hasPrevious()
{
	return prevY!=-1 || posZ>=prevZ;
}

TripleID *IteratorY::previous()
{
	if(posZ<=prevZ) {
		nextY = posY;
		posY = prevY;
		prevY = adjY.findPreviousAppearance(prevY-1, patY);

		posZ = prevZ = adjZ.find(posY);
		nextZ = adjZ.last(posY);

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

void IteratorY::goToStart()
{
	prevY = -1;
	posY = adjY.findNextAppearance(0, patY);
	nextY = adjY.findNextAppearance(posY+1, patY);

	posZ = prevZ = adjZ.find(posY);
	nextZ = adjZ.last(posY);

	x = adjY.findListIndex(posY)+1;
	y = adjY.get(posY);
    z = adjZ.get(posZ);
}

size_t IteratorY::estimatedNumResults()
{
	return adjZ.getSize();
}

ResultEstimationType IteratorY::numResultEstimation()
{
    return UNKNOWN;
}

TripleComponentOrder IteratorY::getOrder() {
    return triples->order;
}

bool IteratorY::findNextOccurrence(unsigned int value, unsigned char component) {
    throw std::logic_error("Not Implemented");
}

bool IteratorY::isSorted(TripleComponentRole role) {
    throw std::logic_error("Not Implemented");
}







ObjectIndexIterator::ObjectIndexIterator(BitmapTriples *trip, TripleID &pat) :
    triples(trip),
    pattern(pat),
    adjY(trip->arrayY, trip->bitmapY),
    adjZ(trip->arrayZ, trip->bitmapZ),
    adjIndex(trip->arrayIndex, trip->bitmapIndex)
{
    // Convert pattern to local order.
    swapComponentOrder(&pattern, SPO, triples->order);
    patX = pattern.getSubject();
    patY = pattern.getPredicate();
    patZ = pattern.getObject();

    if(patZ==0) {
    	throw std::runtime_error("This iterator is not suitable for this pattern");
    }

#if 0
    cout << "Pattern: " << patX << " " << patY << " " << patZ << endl;
    cout << "AdjY: " << endl;
    adjY.dump();
    cout << "AdjZ: " << endl;
    adjZ.dump();
    cout << "AdjIndexObjects: " << endl;
    adjIndex.dump();
#endif

    calculateRange();
    goToStart();
}

unsigned int ObjectIndexIterator::getPosZ(unsigned int indexObjectPos) {
#ifdef SAVE_ADJ_LIST
    unsigned int posZ=0;
    unsigned int posAdjList = adjIndex.get(indexObjectPos);

    try {
        posZ = adjZ.find(posAdjList, patZ);
//        z = adjZ.get(posZ);
    } catch (std::exception& e) {
	cerr << "posZ not found in Index!!!!" << endl;
        posZ = adjZ.find(posAdjList);
    }

#else
    unsigned int posZ = adjIndex.get(index);

    while( (z=adjZ.get(posZ))!=patZ) {
        posZ++;
    }
#endif
    return posZ;
}

unsigned int ObjectIndexIterator::getY(unsigned int index) {

#ifdef SAVE_ADJ_LIST
    unsigned int posAdjList = adjIndex.get(index);
    unsigned int myY = adjY.get(posAdjList);
    return myY;
#else
    unsigned int posZ = adjIndex.get(index);
    unsigned int posY = adjZ.findListIndex(posZ);
    unsigned int myY = adjY.get(posY);
    return myY;
#endif

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
    unsigned int posY = adjIndex.get(posIndex);

    z = patZ;
    y = patY!=0 ? patY : adjY.get(posY);
    x = adjY.findListIndex(posY)+1;

    posIndex++;

    updateOutput();
    return &returnTriple;
}

bool ObjectIndexIterator::hasPrevious()
{
    return posIndex>minIndex;
}

TripleID *ObjectIndexIterator::previous()
{
    posIndex--;

    unsigned int posY = adjIndex.get(posIndex);

    z = patZ;
    y = patY!=0 ? patY : adjY.get(posY);
    x = adjY.findListIndex(posY)+1;

    updateOutput();
    return &returnTriple;
}

void ObjectIndexIterator::calculateRange() {

    minIndex=adjIndex.find(patZ-1);
    maxIndex=adjIndex.last(patZ-1);
    //maxIndex=adjIndex.findNext(minIndex)-1;


    if(patY!=0) {
        //cout << endl << endl << "binsearch " << patY <<  endl;
        while (minIndex <= maxIndex) {
            //cout << "binSearch range: " << minIndex << ", " << maxIndex << endl;
            long long mid = (minIndex + maxIndex) >> 1;
            unsigned int predicate=getY(mid);

            if (patY > predicate) {
                minIndex = mid + 1;
            } else if (patY < predicate) {
                maxIndex = mid - 1;
            } else {
#if 0
                cout << "Middle found at " << mid << " => " << predicate << " Y=" << getY(mid) << endl;
                cout << "At maxIndex: " << maxIndex << " => Y=" << getY(maxIndex) << endl;
                cout << "At minIndex: " << minIndex << " => Y=" << getY(minIndex) << endl;

                // Do Sequential search
                unsigned int left = mid;
                while(left>minIndex && predicate==patY) {
                    left--;
                    predicate = getY(left);
                    cout << "Check left: " << minIndex << "/" << left << "=>" << predicate << " Z=" << adjZ.get(getPosZ(left))<< endl;
                }
                minIndex= predicate==patY ? left : left+1;

                unsigned int right = mid;
                predicate = patY;
                while(right<maxIndex && predicate==patY) {
                    right++;
                    predicate = getY(right);
                    cout << "Check right: " << right << "/" << maxIndex << "=>" << predicate << " Z=" << adjZ.get(getPosZ(right)) << endl;
                }
                maxIndex = predicate==patY ? right :  right-1;

#else
                // Binary Search to find left boundary
                long long left=minIndex;
                long long right=mid;
                long long pos=0;

                while(left<=right) {
                    pos = (left+right) >> 1;

                    predicate = getY(pos);

                    if(predicate!=patY) {
                        left = pos+1;
                    } else {
                        right = pos-1;
                    }
                }
                minIndex = predicate==patY ? pos : pos+1;

                // Binary Search to find right boundary
                left = mid;
                right= maxIndex;

                while(left<=right) {
                    pos = (left+right) >> 1;
                    predicate = getY(pos);

                    if(predicate!=patY) {
                        right = pos-1;
                    } else {
                        left = pos+1;
                    }
                }
                maxIndex = predicate==patY ? pos : pos-1;
#endif

//                cout << "Left bound" << endl;
//                for(unsigned int i=minIndex-2; i<=minIndex+2; i++) {
//                    if(i>=0) {
//                        cout << "Found: " << i << "=>" << getY(i) << " " << ((i>= minIndex && i<=maxIndex) ? '*': ' ') << " Z=" << adjZ.get(getPosZ(i))<< endl;
//                    }
//                }

//                cout << "Right bound" << endl;
//                for(unsigned int i=maxIndex-2; i<=maxIndex+2; i++) {
//                    if(i>=0) {
//                        cout << "Found: " << i << "=>" << getY(i) << " " << ((i>= minIndex && i<=maxIndex) ? '*': ' ') << " Z=" << adjZ.get(getPosZ(i))<< endl;
//                    }
//                }

                break;
            }
        }
    }
}

void ObjectIndexIterator::goToStart()
{
    posIndex=minIndex;
}

size_t ObjectIndexIterator::estimatedNumResults()
{
    return maxIndex-minIndex+1;
}

ResultEstimationType ObjectIndexIterator::numResultEstimation()
{
    return EXACT;
}

TripleComponentOrder ObjectIndexIterator::getOrder() {
    return invertOrder(triples->order);
}

bool ObjectIndexIterator::canGoTo()
{
    return true;
}

void ObjectIndexIterator::goTo(unsigned int pos)
{
    if(minIndex+pos>maxIndex) {
	throw std::runtime_error("Cannot goto beyond last element");
    }
    posIndex = minIndex+pos;
}

bool ObjectIndexIterator::findNextOccurrence(unsigned int value, unsigned char component) {
    if(component==1) {
        if(patY!=0) {
            unsigned int posZ=0, posY=0;
            while(x!=value) {
                posZ = getPosZ(posIndex);
                posY = adjZ.findListIndex(posZ);
                x = adjY.findListIndex(posY)+1;

                posIndex++;
            }
            z = adjZ.get(posZ);
            y = adjY.get(posY);
        } else {

        }
    } else if(component==2) {
        // Binary search the predicate within the object list.

    }
    return true;
}


bool ObjectIndexIterator::isSorted(TripleComponentRole role) {
    if(triples->order==SPO) {
	switch(role) {
	case SUBJECT:
	    return false;
	case PREDICATE:
	    return true;
	case OBJECT:
	    return true;
	}
    } else if(triples->order==OPS) {
	switch(role) {
	case OBJECT:
	    return false;
	case PREDICATE:
	    return true;
	case SUBJECT:
	    return true;
	}
    }

    throw std::runtime_error("Order not supported");
}


BTInterleavedIterator::BTInterleavedIterator(BitmapTriples *triples, size_t skip) :
            triples(triples),
            adjY(triples->arrayY, triples->bitmapY),
            adjZ(triples->arrayZ, triples->bitmapZ),
            posZ(0),
            skip(skip)
{
}

bool BTInterleavedIterator::hasNext()
{
    return posZ<triples->getNumberOfElements();
}

TripleID *BTInterleavedIterator::next()
{
    size_t posY = adjZ.findListIndex(posZ);

    unsigned int z = adjZ.get(posZ);
    unsigned int y = adjY.get(posY);
    unsigned int x = adjY.findListIndex(posY)+1;

    posZ+=skip;

    returnTriple.setAll(x,y,z);

    return &returnTriple;
}



}
