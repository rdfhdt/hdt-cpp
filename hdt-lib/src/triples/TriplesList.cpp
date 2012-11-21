/*
 * File: TriplesList.cpp
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

#include <HDTVocabulary.hpp>

#include "TriplesList.hpp"
#include "TriplesComparator.hpp"
#include "../util/StopWatch.hpp"

#include <algorithm>

#include "../util/Histogram.h"

namespace hdt {

TriplesList::TriplesList() : order(Unknown), numValidTriples(0)
{
}

TriplesList::TriplesList(HDTSpecification &specification) : spec(specification), order(Unknown), numValidTriples(0) {
}

TriplesList::~TriplesList()
{

}

IteratorTripleID *TriplesList::search(TripleID &pattern)
{
	string patternString = pattern.getPatternString();
	if(patternString=="???") {
		return new TriplesListIterator(this, pattern);
	} else {
		return new SequentialSearchIteratorTripleID(pattern, new TriplesListIterator(this,pattern));
	}
}

IteratorTripleID *TriplesList::searchJoin(TripleID &a, TripleID &b, unsigned short conditions) {
	throw "Not implemented";
}

float TriplesList::cost(TripleID &pattern)
{
	// TODO: Theoretically define this with the team
	throw "Not implemented";
}

unsigned int TriplesList::getNumberOfElements()
{
	return numValidTriples;
}

size_t TriplesList::size()
{
	return numValidTriples*sizeof(TripleID);
}

void TriplesList::save(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener)
{
	controlInformation.clear();
	controlInformation.setUint("numTriples", numValidTriples);
	controlInformation.setFormat(HDTVocabulary::TRIPLES_TYPE_TRIPLESLIST);
	controlInformation.setUint("order", order);
	controlInformation.save(output);

	for( unsigned int i = 0; i < arrayOfTriples.size(); i++ ) {
		if ( arrayOfTriples[i].isValid() ) {
			output.write((char *)&arrayOfTriples[i], sizeof(TripleID));
			NOTIFYCOND(listener, "TriplesList saving", i, arrayOfTriples.size())
		}
	}
}

void TriplesList::load(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener)
{
	std::string format = controlInformation.getFormat();
	if(format!=getType()) {
		throw "Trying to read a TriplesList but the data is not TriplesList";
	}

	order = (TripleComponentOrder) controlInformation.getUint("order");
	unsigned int totalTriples = controlInformation.getUint("numTriples");

	unsigned int numRead=0;
	TripleID readTriple;

	while(input.good() && numRead<totalTriples) {
		input.read((char *)&readTriple, sizeof(TripleID));
		arrayOfTriples.push_back(readTriple);
		numRead++;
		numValidTriples++;
		NOTIFYCOND(listener, "TriplesList loading", numRead, totalTriples)
    }
}

size_t TriplesList::load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener)
{
    throw "Not implemented";
}

void TriplesList::load(ModifiableTriples &input, ProgressListener *listener)
{
	IteratorTripleID *it = input.searchAll();

	while(it->hasNext()) {
		TripleID *triple = it->next();

		this->insert(*triple);
	}

	delete it;
}

void TriplesList::generateIndex(ProgressListener *listener) {

}

void TriplesList::saveIndex(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener) {

}

void TriplesList::loadIndex(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener) {

}

size_t TriplesList::loadIndex(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener)
{
}


void TriplesList::populateHeader(Header &header, string rootNode)
{
	header.insert(rootNode, HDTVocabulary::TRIPLES_TYPE, HDTVocabulary::TRIPLES_TYPE_TRIPLESLIST);
	header.insert(rootNode, HDTVocabulary::TRIPLES_NUM_TRIPLES, getNumberOfElements() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_ORDER, getOrderStr(order) );
}

void TriplesList::startProcessing(ProgressListener *listener)
{
}

void TriplesList::stopProcessing(ProgressListener *listener)
{
}

string TriplesList::getType() {
    return HDTVocabulary::TRIPLES_TYPE_TRIPLESLIST;
}

TripleComponentOrder TriplesList::getOrder()
{
    return order;
}

// From ModifiableTriples

void TriplesList::insert(TripleID &triple)
{
	// Add the triple
	order = Unknown;
	arrayOfTriples.push_back(triple);
	numValidTriples++;
}

void TriplesList::insert(IteratorTripleID *triples)
{
	while( triples->hasNext() ) {
		arrayOfTriples.push_back(*triples->next());
		numValidTriples++;
	}
	order = Unknown;
}

bool TriplesList::remove(TripleID &pattern)
{
	bool removed=false;
	for(unsigned int i=0; i< arrayOfTriples.size(); i++) {
		TripleID *tid = &arrayOfTriples[i];
		if (tid->match(pattern)) {
			tid->clear();
			numValidTriples--;
			removed=true;
		}
	}
	return removed;
}

bool TriplesList::remove(IteratorTripleID *pattern)
{
	bool removed = false;
	vector<TripleID> allPat;

	while(pattern->hasNext()) {
		allPat.push_back(*pattern->next());
	}

	for(unsigned int i=0; i< arrayOfTriples.size(); i++) {
		TripleID *tid = &arrayOfTriples[i];
        for(size_t j=0; j<allPat.size(); j++) {
			if (tid->match(allPat[i])) {
				tid->clear();
				numValidTriples--;
				removed = true;
				break;
			}
		}
	}
	return removed;
}

void TriplesList::sort(TripleComponentOrder order, ProgressListener *listener)
{
	if(this->order != order) {
		//StopWatch st;
		NOTIFY(listener, "Sorting triples", 0, 100);
		std::sort(arrayOfTriples.begin(), arrayOfTriples.end(), TriplesComparator(order));
		//cout << "Sorted in " << st << endl;
		this->order = order;
	}
}


void TriplesList::setOrder(TripleComponentOrder order)
{
	this->order = order;


}

TripleID* TriplesList::getTripleID(unsigned int i)
{
	return &this->arrayOfTriples[i];
}

void TriplesList::removeDuplicates(ProgressListener *listener) {

	if(arrayOfTriples.size()<=1)
		return;

	if(order==Unknown){
		throw "Cannot remove duplicates on unordered triples";
	}

	unsigned int j = 0;
	StopWatch st;

	for(unsigned int i=1; i<arrayOfTriples.size(); i++) {
        if(!arrayOfTriples[i].isValid()) {
            cerr << "WARNING: Triple with null component: " << arrayOfTriples[i] << endl;
        }
        if(arrayOfTriples[i] != arrayOfTriples[j] && arrayOfTriples[i].isValid()) {
			j++;
			arrayOfTriples[j] = arrayOfTriples[i];
		}
		NOTIFYCOND(listener, "Removing duplicate triples", i, arrayOfTriples.size());
	}

	//cout << "Removed "<< arrayOfTriples.size()-j-1 << " duplicates in " << st << endl;

	arrayOfTriples.resize(j+1);
	numValidTriples = j+1;
}



/** Calculate Degree
 * @param path Description of the param.
 * @return void
 */
void TriplesList::calculateDegree(string path) {
        const int maxval = 50000;
        const int nbins = 50000;

        Histogram hDegree(0, maxval, nbins);
        Histogram hDegreePartial(0, maxval, nbins);
        Histogram hDegreeLabeled(0, maxval, nbins);

        int xcount = 1, ycount = 1, ychanged = 1;

        TripleID currentTriple;

        currentTriple = arrayOfTriples[0];
        swapComponentOrder(&currentTriple, SPO, order);

        unsigned int x = currentTriple.getSubject();
        unsigned int y = currentTriple.getPredicate();
        unsigned int z = currentTriple.getObject();

        //cout << arrayOfTriples[0].getSubject() << " " << arrayOfTriples[0].getPredicate() << " " << arrayOfTriples[0].getObject() << endl;
        for (unsigned int i = 1; i < getNumberOfElements(); i++) {
            currentTriple = arrayOfTriples[i];
            swapComponentOrder(&currentTriple, SPO, order);
                //cout<<currentTriple.getSubject()<< " " << currentTriple.getPredicate() << " " << currentTriple.getObject()<<"\n";
                // Ignore duplicate triples
                if ((x == currentTriple.getSubject()) && (y == currentTriple.getPredicate()) && (z == currentTriple.getObject())) {
                        continue;
                }

                if (x != currentTriple.getSubject()) {
                        //cout << "\tdegree: " << xcount <<endl;
                        hDegree.add(xcount);

                        //cout << "\tpartial degree: " << ycount << endl;
                        hDegreePartial.add(ycount);

                        //cout << "\tlabeled degree: " << ychanged << endl;
                        hDegreeLabeled.add(ychanged);

                        xcount = ycount = 1;
                        ychanged = 1;
                } else {
                        xcount++;

                        if (y != currentTriple.getPredicate()) {
                                ychanged++;

                                //cout << "\tpartial degree: " << ycount << endl;
                                hDegreePartial.add(ycount);
                                ;

                                ycount = 1;
                        } else {
                                ycount++;
                        }
                }



                //cout << currentTriple.getSubject() << " " << currentTriple.getPredicate() << " " << currentTriple.getObject() << endl;

                x = currentTriple.getSubject();
                y = currentTriple.getPredicate();
                z = currentTriple.getObject();
        }

                //cout << "\tdegree: " << xcount <<endl;
                hDegree.add(xcount);

                //cout << "\tpartial degree: " << ycount << endl;
                hDegreePartial.add(ycount);

                //cout << "\tlabeled degree: " << ychanged << endl;
                hDegreeLabeled.add(ychanged);

        hDegree.end();
        hDegreePartial.end();
        hDegreeLabeled.end();

#if 0
        ofstream out;

        string direcc=""; //predicate total degree is neither in nor out
        if (parsing == SPO) direcc="out";
        else if (parsing == OPS) direcc="in";
        else direcc="predicate";



        //cout << endl << "Partial degree" << endl;

        if (parsing==PSO){
                out.open((path + "_Predicate").c_str(), ios::out);
                out << "# Predicate degree" << endl;
                hDegree.dumpStr(out);
                out.close();

                out.open((path + "_inPredicate").c_str(), ios::out);
                out << "# Predicate_in degree" << endl;
                hDegreeLabeled.dumpStr(out);
                out.close();

        }
        else if (parsing==POS){
                out.open((path + "_outPredicate").c_str(), ios::out);
                out << "# Predicate_out degree" << endl;
                hDegreeLabeled.dumpStr(out);
                out.close();
        }
        else if (parsing==SOP){
                out.open((path + "_DirectOut").c_str(), ios::out);
                out << "# Direct_out degree" << endl;
                hDegreeLabeled.dumpStr(out);
                out.close();
        }
        else if (parsing==OSP){
                out.open((path + "_DirectIn").c_str(), ios::out);
                out << "# Direct_in degree" << endl;
                hDegreeLabeled.dumpStr(out);
                out.close();
        }
        else{
                string direcc = (parsing == SPO) ? "out" : "in";
                //	cout << endl << "Degree" << endl;
                out.open((path + "_" + direcc).c_str(), ios::out);
                out << "# " << direcc << " degree" << endl;
                hDegree.dumpStr(out);
                out.close();

                out.open((path + "_p" + direcc).c_str(), ios::out);
                out << "# Partial " << direcc << " degree" << endl;
                hDegreePartial.dumpStr(out);
                out.close();
                //cout << endl << "Labeled degree" << endl;
                out.open((path + "_l" + direcc).c_str(), ios::out);
                out << "# Labeled" << direcc << " degree" << endl;
                hDegreeLabeled.dumpStr(out);
                out.close();
        }
#endif

}

/** Calculate Degrees
 * @param path Description of the param.
 * @return void
 */
void TriplesList::calculateDegrees(string path) {

    StopWatch st;
        cout << "Calculate OUT Degree" << endl;
        sort(SPO);
        calculateDegree(path);

        cout << "Calculate IN Degree" << endl;
        sort(OPS);
        calculateDegree(path);

        cout << "Calculate Direct OUT Degree" << endl;
        sort(SOP);
        calculateDegree(path);

        cout << "Calculate Direct IN Degree" << endl;
        sort(OSP);
        calculateDegree(path);

        cout << "Calculate Predicate IN Degree" << endl;
        sort(PSO);
        calculateDegree(path);

        cout << "Calculate Predicate OUT Degree" << endl;
        sort(POS);
        calculateDegree(path);
        cout << "Degrees calculated in " << st << endl;
}


// ITERATOR

TriplesListIterator::TriplesListIterator(TriplesList *triples, TripleID & pattern) :
         pattern(pattern), triples(triples), pos(0)
{
}

bool TriplesListIterator::hasNext()
{
	return pos<triples->getNumberOfElements();
}

TripleID *TriplesListIterator::next()
{
	returnTriple = *triples->getTripleID(pos++);
	return &returnTriple;
}

bool TriplesListIterator::hasPrevious()
{
	return pos>0;
}

TripleID *TriplesListIterator::previous()
{
	returnTriple = *triples->getTripleID(--pos);
	return &returnTriple;
}

void TriplesListIterator::goToStart()
{
	pos=0;
}




}











