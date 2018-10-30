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
#include <stdexcept>
#include <HDTVocabulary.hpp>

#include "TriplesList.hpp"
#include "TriplesComparator.hpp"
#include "../util/StopWatch.hpp"

#include <algorithm>

#include "../util/Histogram.h"

namespace hdt {

TriplesList::TriplesList() : ptr(NULL), order(Unknown), numValidTriples(0)
{
}

TriplesList::TriplesList(HDTSpecification &specification) : spec(specification), ptr(NULL), order(Unknown), numValidTriples(0) {
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
	throw std::logic_error("Not Implemented");
}

float TriplesList::cost(TripleID &pattern) const
{
	// TODO: Theoretically define this with the team
	throw std::logic_error("Not Implemented");
}

size_t TriplesList::getNumberOfElements() const
{
	return numValidTriples;
}

size_t TriplesList::size() const
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

    for( size_t i = 0; i < arrayOfTriples.size(); i++ ) {
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
		throw std::runtime_error("Trying to read a TriplesList but the data is not TriplesList");
	}

	order = (TripleComponentOrder) controlInformation.getUint("order");
    size_t totalTriples = controlInformation.getUint("numTriples");

    size_t numRead=0;
	TripleID readTriple;

	while(input.good() && numRead<totalTriples) {
		input.read((char *)&readTriple, sizeof(TripleID));
		arrayOfTriples.push_back(readTriple);
        ptr = &arrayOfTriples[0];
		numRead++;
		numValidTriples++;
		NOTIFYCOND(listener, "TriplesList loading", numRead, totalTriples)
    }
}

#define CHECKPTR(base, max, size) if(((base)+(size))>(max)) throw std::runtime_error("Could not read completely the HDT from the file.");

size_t TriplesList::load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener)
{
    size_t count=0;

    ControlInformation controlInformation;
    count+=controlInformation.load(&ptr[count], ptrMax);

    std::string format = controlInformation.getFormat();
    if(format!=getType()) {
        throw std::runtime_error("Trying to read a TriplesList but the data is not TriplesList");
    }

    order = (TripleComponentOrder) controlInformation.getUint("order");
    //unsigned long long totalTriples = controlInformation.getUint("numTriples");
    this->numValidTriples = 100000000;

    //CHECKPTR(&ptr[count],ptrMax,numValidTriples*12);

    this->ptr = (TripleID*)&ptr[count];

    return count;
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
	return 0;
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

string TriplesList::getType() const
{
    return HDTVocabulary::TRIPLES_TYPE_TRIPLESLIST;
}

TripleComponentOrder TriplesList::getOrder() const
{
    return order;
}

// From ModifiableTriples

void TriplesList::insert(TripleID &triple)
{
	// Add the triple
	order = Unknown;
	arrayOfTriples.push_back(triple);
    ptr = (TripleID*)&arrayOfTriples[0];
	numValidTriples++;
}

void TriplesList::insert(IteratorTripleID *triples)
{
    size_t numres = triples->estimatedNumResults();

    arrayOfTriples.resize(numres);
    numValidTriples=0;

	while( triples->hasNext() ) {
        arrayOfTriples[numValidTriples++] = *triples->next();
        ptr = (TripleID*)&arrayOfTriples[0];
	}
	order = Unknown;
}

bool TriplesList::remove(TripleID &pattern)
{
	bool removed=false;
    for(size_t i=0; i< arrayOfTriples.size(); i++) {
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

    for(size_t i=0; i< arrayOfTriples.size(); i++) {
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

TripleID* TriplesList::getTripleID(size_t i)
{
    return &ptr[i];
    //return &this->arrayOfTriples[i];
}

void TriplesList::removeDuplicates(ProgressListener *listener) {

	if(arrayOfTriples.size()<=1)
		return;

	if(order==Unknown){
		throw std::runtime_error("Cannot remove duplicates on unordered triples");
	}

    size_t j = 0;
	StopWatch st;

    for(size_t i=1; i<arrayOfTriples.size(); i++) {
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


const string vocabSubject = "_:statistics";
const string vocabPredicate = "http://purl.org/HDT/hdt#";

/** Calculate Degree
 * @param path Description of the param.
 * @param maxSO Maximum SO in the dictionary.
 * @return void
 */
void TriplesList::calculateDegree(string path, size_t numPredicates,size_t maxSO) {
	const int maxval = 1000000;
	const int nbins = 1000000;
	map<int, Histogram*> hDegreePartialPerPredicate;

	for (size_t i=1;i<=numPredicates;i++){
		 Histogram* hDegreePart = new Histogram(0, maxval, nbins);
		 hDegreePartialPerPredicate[i]=hDegreePart;
	}

	map<string, int> listsofPredicates; //compute the different lists of Predicates;
	map<int, int> predicateinlists; //compute the number of lists per predicate;
	string listPredicates = ""; //currentlist;
	size_t numberofYs = 0;

	Histogram hDegree(0, maxval, nbins);
	Histogram hDegreePartial(0, maxval, nbins);
	Histogram hDegreeLabeled(0, maxval, nbins);

	size_t xcount = 1, ycount = 1, ychanged = 1;

	TripleID currentTriple;

	currentTriple = arrayOfTriples[0];
	swapComponentOrder(&currentTriple, SPO, order);

	size_t x = currentTriple.getSubject();
	size_t y = currentTriple.getPredicate();
	size_t z = currentTriple.getObject();

	string prefixSO = "";
	if (maxSO != 0) {
		//numiterations=maxSO+1;
		prefixSO = "_SO";
	}

	std::stringstream ss;
    ss << (size_t) (currentTriple.getPredicate());
	listPredicates = ss.str(); // The resulting string
	numberofYs++;

	//cout << arrayOfTriples[0].getSubject() << " " << arrayOfTriples[0].getPredicate() << " " << arrayOfTriples[0].getObject() << endl;
	cout << "Numberof Elements:" << getNumberOfElements() << endl;
	fflush(stdout);
	unsigned long numiterations = getNumberOfElements();
	//size_t numiterations = 4953033043;
	bool stopSO = false;
	for (size_t i = 1; i < numiterations; i++) {
		if (i % 1000000 == 0) {
			cout << i << " triples" << endl;
		}
		if (!stopSO) {

			currentTriple = arrayOfTriples[i];
			swapComponentOrder(&currentTriple, SPO, order);
			//cout<<currentTriple.getSubject()<< " " << currentTriple.getPredicate() << " " << currentTriple.getObject()<<"\n";
			// Ignore duplicate triples
			if ((x == currentTriple.getSubject())
					&& (y == currentTriple.getPredicate())
					&& (z == currentTriple.getObject())) {
				continue;
			}

			if (x != currentTriple.getSubject()) {
				//cout << "\tdegree: " << xcount <<endl;
				hDegree.add(xcount);

				//cout << "\tpartial degree: " << ycount << endl;
				hDegreePartial.add(ycount);

				//update hDegreePartialPerPredicate
				std::map<int,Histogram*>::iterator it = hDegreePartialPerPredicate.find(y); //search some previous degree of the predicate
				  if (it != hDegreePartialPerPredicate.end()){
					  it->second->add(ycount);
				  }
				  /*else{
					   Histogram hDegreePartialPred(0, maxval, nbins);
					   hDegreePartialPred.add(ycount);
					   cout<<"saving hDegreePartialPred of pred "<<y<< " with value="<<ycount<<endl;
					   hDegreePartialPerPredicate[y]=hDegreePartialPred;
				  }*/

				//cout << "\tlabeled degree: " << ychanged << endl;
				hDegreeLabeled.add(ychanged);

				xcount = ycount = 1;
				ychanged = 1;
				if (order == SPO) { //save list of Predicates

					//register the number of lists per predicate
					if (listsofPredicates[listPredicates] == 0) {
#ifdef WIN32
                        char *copylist = new char[listPredicates.size()+1];
#else
						char copylist[listPredicates.size()+1];
#endif
                        
						//listPredicates.copy((char*) copylist.c_str(),
						listPredicates.copy(copylist,
								listPredicates.size(), 0);
						copylist[listPredicates.size()]='\0';
						//char *part = strtok((char*) copylist.c_str(), "+"); // passing a string starts a new iteration
						char *part = strtok(copylist, "+"); // passing a string starts a new iteration
						while (part) {
							predicateinlists[atoi(part)] =
									predicateinlists[atoi(part)] + 1;
							part = strtok(NULL, "+");
						}
#ifdef WIN32
						delete copylist;
#endif
					}
					// register the number of repetitions per lists
					listsofPredicates[listPredicates] =
							listsofPredicates[listPredicates] + 1;

				}
				if (maxSO == 0 || currentTriple.getSubject() <= maxSO) {
					std::stringstream ss;
                    ss << (size_t) (currentTriple.getPredicate());
					listPredicates = ss.str(); // The resulting string
					numberofYs++;
				} else {
					stopSO = true;
				}
			} else {
				xcount++;

				if (y != currentTriple.getPredicate()) {

					if (order == SPO) { //save list of Predicates
						std::stringstream ss;
                        ss << (size_t) (currentTriple.getPredicate());
						listPredicates = listPredicates + "+" + ss.str(); // The resulting string
					}
					numberofYs++;

					ychanged++;

					//cout << "\tpartial degree: " << ycount << endl;
					hDegreePartial.add(ycount);
					;

					//update hDegreePartialPerPredicate
					std::map<int,Histogram*>::iterator it = hDegreePartialPerPredicate.find(y); //search some previous degree of the predicate
					  if (it != hDegreePartialPerPredicate.end()){
						  it->second->add(ycount);
					  }
					 /* else{
						   Histogram hDegreePartialPred(0, maxval, nbins);
						   hDegreePartialPred.add(ycount);
						   hDegreePartialPerPredicate[y]=hDegreePartialPred;
						   cout<<"saving hDegreePartialPred of pred "<<y<< " with value="<<ycount<<endl;
					  }*/

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
	}

	//cout << "Fin listPredicates:" << listPredicates << endl;
	fflush(stdout);

	if (maxSO == 0 || currentTriple.getSubject() <= maxSO) {
		// save list of predicates
		if (order == SPO) {
			//register the number of lists per predicate
			if (listsofPredicates[listPredicates] == 0) {
#ifdef WIN32
                char *copylist=new char[listPredicates.size()+1];
#else
                char copylist[listPredicates.size()+1];
#endif
                //listPredicates.copy((char*) copylist.c_str(),
                 listPredicates.copy(copylist,listPredicates.size(), 0);
                 copylist[listPredicates.size()]='\0';

				char *part = strtok(copylist, "+"); // passing a string starts a new iteration
				while (part) {
					predicateinlists[atoi(part)] = predicateinlists[atoi(part)]
							+ 1;
					part = strtok(NULL, "+");
				}
#ifdef WIN32
                delete copylist;
#endif
			}
			// register the number of repetitions per lists
			listsofPredicates[listPredicates] =
					listsofPredicates[listPredicates] + 1;
		}
	}

	if (maxSO == 0 || currentTriple.getSubject() <= maxSO) {

		//cout << "\tdegree: " << xcount <<endl;
		hDegree.add(xcount);

		//cout << "\tpartial degree: " << ycount << endl;
		hDegreePartial.add(ycount);

		//update hDegreePartialPerPredicate
		std::map<int,Histogram*>::iterator it = hDegreePartialPerPredicate.find(y); //search some previous degree of the predicate
		  if (it != hDegreePartialPerPredicate.end()){
			  it->second->add(ycount);
		  }
		 /* else{
			   Histogram hDegreePartialPred(0, maxval, nbins);
			   hDegreePartialPred.add(ycount);
			   hDegreePartialPerPredicate[y]=hDegreePartialPred;
			   cout<<"saving hDegreePartialPred of pred "<<y<< " with value="<<ycount<<endl;
		  }*/

		//cout << "\tlabeled degree: " << ychanged << endl;
		hDegreeLabeled.add(ychanged);
	}
	hDegree.end();
	hDegreePartial.end();
	hDegreeLabeled.end();

//#if 0
	ofstream out;
	ofstream out_summary;
	ofstream out_header_stats;

	out_summary.open((path + prefixSO + "_Summary").c_str(), ios::app); //append to the end
	out_header_stats.open((path + prefixSO + "_HeaderStats").c_str(), ios::app); //append to the end

	string direcc = ""; //predicate total degree is neither in nor out
	if (order == SPO)
		direcc = "out";
	else if (order == OPS)
		direcc = "in";
	else
		direcc = "predicate";

//cout << endl << "Partial degree" << endl;

	if (order == PSO) {
		out.open((path + prefixSO + "_Predicate").c_str(), ios::out);
		out << "# Predicate degree" << endl;
		out_summary << "* Predicate degree" << endl;
		hDegree.dumpStr(out);
		hDegree.dumpStr(out_summary, false);
		out.close();
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateDegree_min> "<<hDegree.getMinValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateDegree_max> "<<hDegree.getMaxValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateDegree_average> "<<hDegree.getMean()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateDegree_deviation> "<<hDegree.getDeviation()<< endl;

		out.open((path + prefixSO + "_inPredicate").c_str(), ios::out);
		out << "# Predicate_in degree" << endl;
		out_summary << "* Predicate_in degree" << endl;
		hDegreeLabeled.dumpStr(out);
		hDegreeLabeled.dumpStr(out_summary, false);
		out.close();
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateInDegree_min> "<<hDegreeLabeled.getMinValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateInDegree_max> "<<hDegreeLabeled.getMaxValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateInDegree_average> "<<hDegreeLabeled.getMean()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateInDegree_deviation> "<<hDegreeLabeled.getDeviation()<< endl;

	} else if (order == POS) {
		out.open((path + prefixSO + "_outPredicate").c_str(), ios::out);
		out << "# Predicate_out degree" << endl;
		out_summary << "* Predicate_out degree" << endl;
		hDegreeLabeled.dumpStr(out);
		hDegreeLabeled.dumpStr(out_summary, false);
		out.close();
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateOutDegree_min> "<<hDegreeLabeled.getMinValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateOutDegree_max> "<<hDegreeLabeled.getMaxValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateOutDegree_average> "<<hDegreeLabeled.getMean()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateOutDegree_deviation> "<<hDegreeLabeled.getDeviation()<< endl;
	} else if (order == SOP) {
		out.open((path + prefixSO + "_DirectOut").c_str(), ios::out);
		out << "# Direct_out degree" << endl;
		out_summary << "* Direct_out degree" << endl;
		hDegreeLabeled.dumpStr(out);
		hDegreeLabeled.dumpStr(out_summary, false);
		out.close();
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"DirectOutDegree_min> "<<hDegreeLabeled.getMinValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"DirectOutDegree_max> "<<hDegreeLabeled.getMaxValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"DirectOutDegree_average> "<<hDegreeLabeled.getMean()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"DirectOutDegree_deviation> "<<hDegreeLabeled.getDeviation()<< endl;
	} else if (order == OSP) {
		out.open((path + prefixSO + "_DirectIn").c_str(), ios::out);
		out << "# Direct_in degree" << endl;
		out_summary << "* Direct_in degree" << endl;
		hDegreeLabeled.dumpStr(out);
		hDegreeLabeled.dumpStr(out_summary, false);
		out.close();
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"DirectInDegree_min> "<<hDegreeLabeled.getMinValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"DirectInDegree_max> "<<hDegreeLabeled.getMaxValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"DirectInDegree_average> "<<hDegreeLabeled.getMean()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"DirectInDegree_deviation> "<<hDegreeLabeled.getDeviation()<< endl;
	} else {
		string direcc = (order == SPO) ? "out" : "in";
		//	cout << endl << "Degree" << endl;
		out.open((path + prefixSO + "_" + direcc).c_str(), ios::out);
		out << "# " << direcc << " degree" << endl;
		out_summary << "* " << direcc << " degree" << endl;
		hDegree.dumpStr(out);
		hDegree.dumpStr(out_summary, false);
		out.close();
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< direcc <<"Degree_min> "<<hDegree.getMinValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< direcc <<"Degree_max> "<<hDegree.getMaxValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< direcc <<"Degree_average> "<<hDegree.getMean()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< direcc <<"Degree_deviation> "<<hDegree.getDeviation()<< endl;

		out.open((path + prefixSO + "_p" + direcc).c_str(), ios::out);
		out << "# Partial " << direcc << " degree" << endl;
		out_summary << "* Partial " << direcc << " degree" << endl;
		hDegreePartial.dumpStr(out);
		hDegreePartial.dumpStr(out_summary, false);
		out.close();
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Partial"<< direcc <<"Degree_min> "<<hDegreePartial.getMinValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Partial"<<direcc <<"Degree_max> "<<hDegreePartial.getMaxValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Partial"<<direcc <<"Degree_average> "<<hDegreePartial.getMean()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Partial"<<direcc <<"Degree_deviation> "<<hDegreePartial.getDeviation()<< endl;

		//cout << endl << "Labeled degree" << endl;
		out.open((path + prefixSO + "_l" + direcc).c_str(), ios::out);
		out << "# Labeled" << direcc << " degree" << endl;
		out_summary << "* Labeled" << direcc << " degree" << endl;
		hDegreeLabeled.dumpStr(out);
		hDegreeLabeled.dumpStr(out_summary, false);
		out.close();
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Labeled"<< direcc <<"Degree_min> "<<hDegreeLabeled.getMinValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Labeled"<<direcc <<"Degree_max> "<<hDegreeLabeled.getMaxValue()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Labeled"<<direcc <<"Degree_average> "<<hDegreeLabeled.getMean()<< endl;
		out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Labeled"<<direcc <<"Degree_deviation> "<<hDegreeLabeled.getDeviation()<< endl;

		if (direcc == "out") { //Calculate listsofPredicates values:

			Histogram predicateFreqs(0, maxval, nbins);
			for (std::map<string, int>::iterator it = listsofPredicates.begin();
					it != listsofPredicates.end(); ++it) {
				//std::cout << it->first << " => " << it->second << '\n';
				predicateFreqs.add(it->second);
				//print lists:
				//cout<<"it->second:"<<it->second<<endl;
			}
			predicateFreqs.end();
			out.open((path + prefixSO + "_ListPredicates").c_str(), ios::out);
			out << "# Predicate list degree" << endl;
			out_summary << "* Predicate Lists" << endl;
			predicateFreqs.dumpStr(out);
			predicateFreqs.dumpStr(out_summary, false);
			out.close();
			out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateLists> "<<predicateFreqs.getNumber()<< endl;
			out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateLists_max> "<<predicateFreqs.getMaxValue()<< endl;
			out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateLists_average> "<<predicateFreqs.getMean()<< endl;
			out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"PredicateLists_deviation> "<<predicateFreqs.getDeviation()<< endl;

			out_summary << "* Number of Predicates in P List" << endl;
			out_summary << "# Total: " << numberofYs << endl;


			Histogram predicateHist(0, maxval, nbins);
			for (std::map<int, int>::iterator it = predicateinlists.begin();
					it != predicateinlists.end(); ++it) {
				//std::cout << it->first << " => " << it->second << '\n';
				predicateHist.add(it->second);
				//print lists:
				//cout<<"it->second:"<<it->second<<endl;
			}
			predicateHist.end();
			out_summary << "* Lists per Predicate" << endl;
			predicateHist.dumpStr(out_summary, false);
			out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"ListsperPredicate_max> "<<predicateHist.getMaxValue()<< endl;
			out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"ListsperPredicate_average> "<<predicateHist.getMean()<< endl;
			out_header_stats<<vocabSubject<<" <"<<vocabPredicate<<"ListsperPredicate_deviation> "<<predicateHist.getDeviation()<< endl;



		}
		// dump the partial degree of each predicate
		for (std::map<int, Histogram*>::iterator it = hDegreePartialPerPredicate.begin();
							it != hDegreePartialPerPredicate.end(); ++it) {

			it->second->end();
			int predicateID = it->first;
			if (direcc == "out"){ // just plot this metadata once
				out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "StatsofPredicate> " << "_:Predicate"<<predicateID<< endl;
				out_header_stats<<"_:Predicate"<<predicateID<<" <http://purl.org/dc/terms/identifier> "<<predicateID<< endl;
			}

			out_header_stats<<"_:Predicate"<<predicateID<<" <"<<vocabPredicate<< "Partial"<<direcc <<"Degree_average> "<<it->second->getMean()<< endl;

		}

	}
	out_summary.close();
	out_header_stats.close();
//#endif

}
/** Calculate Degree
 * @param path Description of the param.
 * @param maxSO Maximum SO in the dictionary.
 * @return void
 */
void TriplesList::calculateMinStats(string path, size_t numPredicates) {
	const int maxval = 1000000;
	const int nbins = 1000000;
	map<int, Histogram*> hDegreePartialPerPredicate;

	for (size_t i=1;i<=numPredicates;i++){
		 Histogram* hDegreePart = new Histogram(0, maxval, nbins);
		 hDegreePartialPerPredicate[i]=hDegreePart;
	}

	map<string, int> listsofPredicates; //compute the different lists of Predicates;
	map<int, int> predicateinlists; //compute the number of lists per predicate;
	string listPredicates = ""; //currentlist;
	size_t numberofYs = 0;

	Histogram hDegree(0, maxval, nbins);
	Histogram hDegreePartial(0, maxval, nbins);
	Histogram hDegreeLabeled(0, maxval, nbins);

	size_t xcount = 1, ycount = 1, ychanged = 1;

	TripleID currentTriple;

	currentTriple = arrayOfTriples[0];
	swapComponentOrder(&currentTriple, SPO, order);

	size_t x = currentTriple.getSubject();
	size_t y = currentTriple.getPredicate();
	size_t z = currentTriple.getObject();



	std::stringstream ss;
    ss << (size_t) (currentTriple.getPredicate());
	listPredicates = ss.str(); // The resulting string
	numberofYs++;

	//cout << arrayOfTriples[0].getSubject() << " " << arrayOfTriples[0].getPredicate() << " " << arrayOfTriples[0].getObject() << endl;
	cout << "Numberof Elements:" << getNumberOfElements() << endl;
	fflush(stdout);
	unsigned long numiterations = getNumberOfElements();
	//size_t numiterations = 4953033043;
	bool stopSO = false;
	for (size_t i = 1; i < numiterations; i++) {
		if (i % 1000000 == 0) {
			cout << i << " triples" << endl;
		}
		if (!stopSO) {

			currentTriple = arrayOfTriples[i];
			swapComponentOrder(&currentTriple, SPO, order);
			//cout<<currentTriple.getSubject()<< " " << currentTriple.getPredicate() << " " << currentTriple.getObject()<<"\n";
			// Ignore duplicate triples
			if ((x == currentTriple.getSubject())
					&& (y == currentTriple.getPredicate())
					&& (z == currentTriple.getObject())) {
				continue;
			}

			if (x != currentTriple.getSubject()) {
				//cout << "\tdegree: " << xcount <<endl;
				hDegree.add(xcount);

				//cout << "\tpartial degree: " << ycount << endl;
				hDegreePartial.add(ycount);

				//update hDegreePartialPerPredicate
				std::map<int,Histogram*>::iterator it = hDegreePartialPerPredicate.find(y); //search some previous degree of the predicate
				  if (it != hDegreePartialPerPredicate.end()){
					  it->second->add(ycount);
				  }
				  /*else{
					   Histogram hDegreePartialPred(0, maxval, nbins);
					   hDegreePartialPred.add(ycount);
					   cout<<"saving hDegreePartialPred of pred "<<y<< " with value="<<ycount<<endl;
					   hDegreePartialPerPredicate[y]=hDegreePartialPred;
				  }*/

				//cout << "\tlabeled degree: " << ychanged << endl;
				hDegreeLabeled.add(ychanged);

				xcount = ycount = 1;
				ychanged = 1;


			} else {
				xcount++;

				if (y != currentTriple.getPredicate()) {


					numberofYs++;

					ychanged++;

					//cout << "\tpartial degree: " << ycount << endl;
					hDegreePartial.add(ycount);
					;

					//update hDegreePartialPerPredicate
					std::map<int,Histogram*>::iterator it = hDegreePartialPerPredicate.find(y); //search some previous degree of the predicate
					  if (it != hDegreePartialPerPredicate.end()){
						  it->second->add(ycount);
					  }
					 /* else{
						   Histogram hDegreePartialPred(0, maxval, nbins);
						   hDegreePartialPred.add(ycount);
						   hDegreePartialPerPredicate[y]=hDegreePartialPred;
						   cout<<"saving hDegreePartialPred of pred "<<y<< " with value="<<ycount<<endl;
					  }*/

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
	}

	//cout << "Fin listPredicates:" << listPredicates << endl;
	fflush(stdout);





		//cout << "\tdegree: " << xcount <<endl;
		hDegree.add(xcount);

		//cout << "\tpartial degree: " << ycount << endl;
		hDegreePartial.add(ycount);

		//update hDegreePartialPerPredicate
		std::map<int,Histogram*>::iterator it = hDegreePartialPerPredicate.find(y); //search some previous degree of the predicate
		  if (it != hDegreePartialPerPredicate.end()){
			  it->second->add(ycount);
		  }
		 /* else{
			   Histogram hDegreePartialPred(0, maxval, nbins);
			   hDegreePartialPred.add(ycount);
			   hDegreePartialPerPredicate[y]=hDegreePartialPred;
			   cout<<"saving hDegreePartialPred of pred "<<y<< " with value="<<ycount<<endl;
		  }*/

		//cout << "\tlabeled degree: " << ychanged << endl;
		hDegreeLabeled.add(ychanged);

	hDegree.end();
	hDegreePartial.end();
	hDegreeLabeled.end();

//#if 0
	ofstream out;
	ofstream out_summary;
	ofstream out_header_stats;

	out_summary.open((path + "_Summary").c_str(), ios::app); //append to the end
	out_header_stats.open((path  + "_HeaderStats").c_str(), ios::app); //append to the end

	string direcc = ""; //predicate total degree is neither in nor out

//cout << endl << "Partial degree" << endl;


	direcc = (order == SPO) ? "out" : "in";
	//	cout << endl << "Degree" << endl;
	out.open((path  + "_" + direcc).c_str(), ios::out);
	out << "# " << direcc << " degree" << endl;
	out_summary << "* " << direcc << " degree" << endl;
	hDegree.dumpStr(out);
	hDegree.dumpStr(out_summary, false);
	out.close();
	out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< direcc <<"Degree_min> "<<hDegree.getMinValue()<< endl;
	out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< direcc <<"Degree_max> "<<hDegree.getMaxValue()<< endl;
	out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< direcc <<"Degree_average> "<<hDegree.getMean()<< endl;
	out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< direcc <<"Degree_deviation> "<<hDegree.getDeviation()<< endl;

	out.open((path  + "_p" + direcc).c_str(), ios::out);
	out << "# Partial " << direcc << " degree" << endl;
	out_summary << "* Partial " << direcc << " degree" << endl;
	hDegreePartial.dumpStr(out);
	hDegreePartial.dumpStr(out_summary, false);
	out.close();
	out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Partial"<< direcc <<"Degree_min> "<<hDegreePartial.getMinValue()<< endl;
	out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Partial"<<direcc <<"Degree_max> "<<hDegreePartial.getMaxValue()<< endl;
	out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Partial"<<direcc <<"Degree_average> "<<hDegreePartial.getMean()<< endl;
	out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Partial"<<direcc <<"Degree_deviation> "<<hDegreePartial.getDeviation()<< endl;

	//cout << endl << "Labeled degree" << endl;
	out.open((path  + "_l" + direcc).c_str(), ios::out);
	out << "# Labeled" << direcc << " degree" << endl;
	out_summary << "* Labeled" << direcc << " degree" << endl;
	hDegreeLabeled.dumpStr(out);
	hDegreeLabeled.dumpStr(out_summary, false);
	out.close();
	out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Labeled"<< direcc <<"Degree_min> "<<hDegreeLabeled.getMinValue()<< endl;
	out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Labeled"<<direcc <<"Degree_max> "<<hDegreeLabeled.getMaxValue()<< endl;
	out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Labeled"<<direcc <<"Degree_average> "<<hDegreeLabeled.getMean()<< endl;
	out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "Labeled"<<direcc <<"Degree_deviation> "<<hDegreeLabeled.getDeviation()<< endl;


	// dump the partial degree of each predicate
	for (std::map<int, Histogram*>::iterator it = hDegreePartialPerPredicate.begin();
						it != hDegreePartialPerPredicate.end(); ++it) {

		it->second->end();
		int predicateID = it->first;
		if (direcc == "out"){ // just plot this metadata once
			out_header_stats<<vocabSubject<<" <"<<vocabPredicate<< "StatsofPredicate> " << "_:Predicate"<<predicateID<< endl;
			out_header_stats<<"_:Predicate"<<predicateID<<" <http://purl.org/dc/terms/identifier> "<<predicateID<< endl;
		}

		out_header_stats<<"_:Predicate"<<predicateID<<" <"<<vocabPredicate<< "Partial"<<direcc <<"Degree_average> "<<it->second->getMean()<< endl;

	}

	out_summary.close();
	out_header_stats.close();
//#endif

}

/** Calculate Degree only for typed subjects
 * @param path Description of the param.
 * @rdftypeID The ID of the rdf:type predicate
 * @return void
 */
void TriplesList::calculateDegreeType(string path, size_t rdftypeID) {
	const int maxval = 1000000;
	const int nbins = 1000000;

	map<string, int> listsofPredicates; //compute the different lists of Predicates;
	map<int, int> predicateinlists; //compute the number of lists per predicate;
	map<string, int> listofClassesPredicates; //compute the number of repetitions per each concatenation 0IdObject+predicateList
	map<int, int> classesinlists; //compute the number of lists per classs

	string listPredicates = ""; //currentlist of Predicates;
	vector<int> listClasses; //currentlist of Classes;
	int numberofYs = 0;

	Histogram hDegree(0, maxval, nbins);
	Histogram hDegreePartial(0, maxval, nbins);
	Histogram hDegreeLabeled(0, maxval, nbins);

	int xcount = 1, ycount = 1, ychanged = 1;

	TripleID currentTriple;
	bool istypedSubject = false;

	currentTriple = arrayOfTriples[0];
	swapComponentOrder(&currentTriple, SPO, order);

    size_t x = currentTriple.getSubject();
    size_t y = currentTriple.getPredicate();
    size_t z = currentTriple.getObject();

	vector<int> pendingpartialycounts;
	pendingpartialycounts.clear();

	if (y == rdftypeID) {
		istypedSubject = true;
		listClasses.push_back(z);
	}
	std::stringstream ss;
    ss << (size_t) (currentTriple.getPredicate());
	listPredicates = ss.str(); // The resulting string
	numberofYs++;

	//cout << arrayOfTriples[0].getSubject() << " " << arrayOfTriples[0].getPredicate() << " " << arrayOfTriples[0].getObject() << endl;
	cout << "Number of Elements:" << getNumberOfElements() << endl;
//	cout << "Predicate rdf:type ID:" << rdftypeID << endl;
	fflush(stdout);
    for (size_t i = 1; i < getNumberOfElements(); i++) {
		if (i % 1000000 == 0) {
			cout << i << " triples" << endl;
		}
		currentTriple = arrayOfTriples[i];
		swapComponentOrder(&currentTriple, SPO, order);
		//cout<<currentTriple.getSubject()<< " " << currentTriple.getPredicate() << " " << currentTriple.getObject()<<"\n";
		// Ignore duplicate triples
		if ((x == currentTriple.getSubject())
				&& (y == currentTriple.getPredicate())
				&& (z == currentTriple.getObject())) {
			continue;
		}

		if (x != currentTriple.getSubject()) {
			if (istypedSubject) {
				//cout << "\tdegree: " << xcount <<endl;
				hDegree.add(xcount);

				//cout << "\tpartial degree: " << ycount << endl;
				hDegreePartial.add(ycount);

				//cout << "\tlabeled degree: " << ychanged << endl;
				hDegreeLabeled.add(ychanged);
				//save list of Predicates

				//register the number of lists per predicate
				if (listsofPredicates[listPredicates] == 0) {
#ifdef WIN32
                    char *copylist = new char[listPredicates.size()+1];
#else
					char copylist[listPredicates.size()+1];
#endif
                                        listPredicates.copy(copylist,listPredicates.size(), 0);
                                        copylist[listPredicates.size()]='\0';

					char *part = strtok(copylist, "+"); // passing a string starts a new iteration

					while (part) {
						predicateinlists[atoi(part)] = predicateinlists[atoi(
								part)] + 1;
						part = strtok(NULL, "+");
					}
#ifdef WIN32
                    delete copylist;
#endif
				}
				// register the number of repetitions per lists
				listsofPredicates[listPredicates] =
						listsofPredicates[listPredicates] + 1;

				//register the number of lists per class
				for (size_t k = 0; k < listClasses.size(); k++) {
					string concatenationClassPred = "c"; //to concanetate something different from an ID
					std::stringstream ss;
                    ss << (size_t) (listClasses[k]);
					concatenationClassPred = concatenationClassPred + ss.str()
							+ "+" + listPredicates;
					if (listofClassesPredicates[concatenationClassPred] == 0) {
						classesinlists[listClasses[k]] =
								classesinlists[listClasses[k]] + 1;
					}

					// register the number of repetitions per lists
					listofClassesPredicates[concatenationClassPred] =
							listofClassesPredicates[concatenationClassPred] + 1;
				}
				for (size_t k = 0; k < pendingpartialycounts.size(); k++) {
					hDegreePartial.add(pendingpartialycounts[k]);
				}
			}
			pendingpartialycounts.clear();

			//check the current one
			istypedSubject = false;
			listClasses.clear();
			if (currentTriple.getPredicate() == rdftypeID) {
				istypedSubject = true;
				listClasses.push_back(currentTriple.getObject());
			}

			xcount = ycount = 1;
			ychanged = 1;
			//cout << "listPredicates:" << listPredicates << endl;

			std::stringstream ss;
            ss << (size_t) (currentTriple.getPredicate());
			listPredicates = ss.str(); // The resulting string
			numberofYs++;
		} else {
			xcount++;

			if (y != currentTriple.getPredicate()) {

				std::stringstream ss;
                ss << (size_t) (currentTriple.getPredicate());
				listPredicates = listPredicates + "+" + ss.str(); // The resulting string

				numberofYs++;

				ychanged++;
				if (currentTriple.getPredicate() == rdftypeID) {
					istypedSubject = true;
					listClasses.push_back(currentTriple.getObject());
				}
				//save all partial to give them finally is they are typed subjects
				pendingpartialycounts.push_back(ycount);
				//cout << "\tpartial degree: " << ycount << endl;
				//hDegreePartial.add(ycount);
				;

				ycount = 1;
			} else {
				ycount++;
				if (currentTriple.getPredicate() == rdftypeID) {
					listClasses.push_back(currentTriple.getObject());
				}

			}
		}

		//cout << currentTriple.getSubject() << " " << currentTriple.getPredicate() << " " << currentTriple.getObject() << endl;

		x = currentTriple.getSubject();
		y = currentTriple.getPredicate();
		z = currentTriple.getObject();
	}

//cout << "Fin listPredicates:" << listPredicates << endl;
	fflush(stdout);

// save list of predicates

	if (istypedSubject) {
		//register the number of lists per predicate
		if (listsofPredicates[listPredicates] == 0) {
#ifdef WIN32
            char *copylist = new char[listPredicates.size()+1];
#else
			char copylist[listPredicates.size()+1];
#endif
                        listPredicates.copy(copylist,listPredicates.size(), 0);
                        copylist[listPredicates.size()]='\0';

			char *part = strtok(copylist, "+"); // passing a string starts a new iteration

			while (part) {
				predicateinlists[atoi(part)] = predicateinlists[atoi(part)] + 1;
				part = strtok(NULL, "+");
			}
#ifdef WIN32
            delete copylist;
#endif
		}
		// register the number of repetitions per lists
		listsofPredicates[listPredicates] = listsofPredicates[listPredicates]
				+ 1;

		//register the number of lists per class
		for (size_t k = 0; k < listClasses.size(); k++) {
			string concatenationClassPred = "c"; //to concanetate something different from an ID
			std::stringstream ss;
            ss << (size_t) (listClasses[k]);
			concatenationClassPred = concatenationClassPred + ss.str() + "+"
					+ listPredicates;
			if (listofClassesPredicates[concatenationClassPred] == 0) {
				classesinlists[listClasses[k]] = classesinlists[listClasses[k]]
						+ 1;
			}

			// register the number of repetitions per lists
			listofClassesPredicates[concatenationClassPred] =
					listofClassesPredicates[concatenationClassPred] + 1;
		}

		//cout << "\tdegree: " << xcount <<endl;
		hDegree.add(xcount);

		//cout << "\tpartial degree: " << ycount << endl;
		hDegreePartial.add(ycount);

		for (size_t k = 0; k < pendingpartialycounts.size(); k++) {
			hDegreePartial.add(pendingpartialycounts[k]);
		}

		//cout << "\tlabeled degree: " << ychanged << endl;
		hDegreeLabeled.add(ychanged);
	}
	hDegree.end();
	hDegreePartial.end();
	hDegreeLabeled.end();

	//************ TEST */
	/*
	 cout << "Classes and #different lists:" << endl;
	 for (std::map<int, int>::iterator it = classesinlists.begin();
	 it != classesinlists.end(); ++it) {
	 std::cout << it->first << " => " << it->second << '\n';
	 }

	 cout << "different class-predicate lists:" << endl;
	 for (std::map<string, int>::iterator it = listofClassesPredicates.begin();
	 it != listofClassesPredicates.end(); ++it) {
	 std::cout << it->first << " => " << it->second << '\n';
	 }
	 */
//#if 0
	ofstream out;
	ofstream out_summary;

	out_summary.open((path + "_Typed_Summary").c_str(), ios::out); //append to the end

	string direcc = ""; //predicate total degree is neither in nor out

	direcc = "out";

	out.open((path + "_Typed_" + direcc).c_str(), ios::out);
	out << "# " << direcc << " degree" << endl;
	out_summary << "* " << direcc << " degree" << endl;
	hDegree.dumpStr(out);
	hDegree.dumpStr(out_summary, false);
	out.close();

	out.open((path + "_Typed_p" + direcc).c_str(), ios::out);
	out << "# Partial " << direcc << " degree" << endl;
	out_summary << "* Partial " << direcc << " degree" << endl;
	hDegreePartial.dumpStr(out);
	hDegreePartial.dumpStr(out_summary, false);
	out.close();
//cout << endl << "Labeled degree" << endl;
	out.open((path + "_Typed_l" + direcc).c_str(), ios::out);
	out << "# Labeled" << direcc << " degree" << endl;
	out_summary << "* Labeled" << direcc << " degree" << endl;
	hDegreeLabeled.dumpStr(out);
	hDegreeLabeled.dumpStr(out_summary, false);
	out.close();

	Histogram predicateFreqs(0, maxval, nbins);
	for (std::map<string, int>::iterator it = listsofPredicates.begin();
			it != listsofPredicates.end(); ++it) {
		//	std::cout << it->first << " => " << it->second << '\n';
		predicateFreqs.add(it->second);
		//print lists:
		//cout<<"it->second:"<<it->second<<endl;
	}
	predicateFreqs.end();
	out_summary << "* Predicate Lists" << endl;
	predicateFreqs.dumpStr(out_summary, false);

// this has no sense now
//	out_summary << "* Number of Predicates in P List" << endl;
//	out_summary << "# Total: " << numberofYs << endl;

	Histogram ListClassesHist(0, maxval, nbins);
	for (std::map<int, int>::iterator it = classesinlists.begin();
			it != classesinlists.end(); ++it) {
		//std::cout << it->first << " => " << it->second << '\n';
		ListClassesHist.add(it->second);
		//print lists:
		//cout<<"it->second:"<<it->second<<endl;
	}
	ListClassesHist.end();
	out_summary << "* Lists per Class" << endl;
	ListClassesHist.dumpStr(out_summary, false);

	Histogram predicateHist(0, maxval, nbins);
	for (std::map<int, int>::iterator it = predicateinlists.begin();
			it != predicateinlists.end(); ++it) {
		//std::cout << it->first << " => " << it->second << '\n';
		predicateHist.add(it->second);
		//print lists:
		//cout<<"it->second:"<<it->second<<endl;
	}
	predicateHist.end();
	out_summary << "* Lists per Predicate" << endl;
	predicateHist.dumpStr(out_summary, false);

	out_summary.close();
//#endif

}

/** Calculate Degrees
 * @param path Description of the param.
 * @return void
 */
void TriplesList::calculateDegrees(string path, size_t maxSO,size_t numPredicates,
        size_t rdftypeID, bool allStats) {

	StopWatch st;

	if (allStats==false){
		cout<<"Generating minimum stats"<<endl;
		cout << "Calculate OUT Degree" << endl;
		sort(SPO);
		calculateMinStats(path,numPredicates);
		cout << "Calculate IN Degree" << endl;
		cout << "..... sorting OPS" << endl;
		sort(OPS);
		cout << "......sort done" << endl;
		calculateMinStats(path,numPredicates);
	}
	else{
		cout<<"Generating full stats"<<endl;
		cout << "Calculate OUT Degree" << endl;
		sort(SPO);
		calculateDegree(path,numPredicates);

		if (maxSO > 0) {
			cout << "Calculate OUT Degree for SO (max:" << maxSO << ") " << endl;
			calculateDegree(path, numPredicates,maxSO);
		}
		if (rdftypeID > 0) {
			cout << "Calculate OUT Degree for Subjects with rdftype " << endl;
			calculateDegreeType(path, rdftypeID);
		}

		cout << "Calculate IN Degree" << endl;
		cout << "..... sorting OPS" << endl;
		sort(OPS);
		cout << "......sort done" << endl;
		calculateDegree(path,numPredicates);
		if (maxSO > 0) {
			cout << "Calculate IN Degree for SO (max:" << maxSO << ") " << endl;
			calculateDegree(path, numPredicates,maxSO);
		}

		cout << "Calculate Direct OUT Degree" << endl;
		cout << "..... sorting SOP" << endl;
		sort(SOP);
		cout << "......sort done" << endl;
		calculateDegree(path,numPredicates);

		if (maxSO > 0) {
			cout << "Calculate Direct OUT Degree for SO (max:" << maxSO << ") "
					<< endl;
			calculateDegree(path, numPredicates,maxSO);
		}

		cout << "Calculate Direct IN Degree" << endl;
		fflush(stdout);
		cout << "..... sorting OSP" << endl;
		sort(OSP);
		cout << "......sort done" << endl;
		fflush(stdout);
		calculateDegree(path,numPredicates);
		if (maxSO > 0) {
				cout << "Calculate Direct IN Degree for SO (max:" << maxSO << ") "
						<< endl;
				calculateDegree(path, numPredicates,maxSO);
			}

		cout << "Calculate Predicate IN Degree" << endl;
		cout << "..... sorting PSO" << endl;
		sort(PSO);
		cout << "......sort done" << endl;
		calculateDegree(path,numPredicates);

		cout << "Calculate Predicate OUT Degree" << endl;
		cout << "..... sorting POS" << endl;
		sort(POS);
		cout << "......sort done" << endl;
		calculateDegree(path,numPredicates);
		cout << "Degrees calculated in " << st << endl;
	}
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

} // namespace hdt
