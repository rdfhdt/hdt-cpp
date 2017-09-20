/*
 * File: CompactTriples.cpp
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

#include "CompactTriples.hpp"
#include "TripleOrderConvert.hpp"

namespace hdt {


CompactTriples::CompactTriples() : numTriples(0), order(SPO) {
	string typey="";
	string typez="";
	try{
		typey = spec.get("stream.y");
		typez = spec.get("stream.z");
	}catch (std::exception& e){}
	streamY = IntSequence::getArray(typey);
	streamZ = IntSequence::getArray(typez);
}

CompactTriples::CompactTriples(HDTSpecification &specification) : spec(specification), numTriples(0) {
	std::string orderStr = "";
	try{
		orderStr = spec.get("triplesOrder");
	}catch(exception& e){}
	order= parseOrder(orderStr.c_str());
	if(order==Unknown)
		order = SPO;
	string typey="";
	string typez="";
	try{
		typey = spec.get("stream.y");
		typez = spec.get("stream.z");
	}catch (std::exception& e){}
	streamY = IntSequence::getArray(typey);
	streamZ = IntSequence::getArray(typez);
}

CompactTriples::~CompactTriples() {
	delete streamY;
	delete streamZ;
}

float CompactTriples::cost(TripleID & triple) const
{
	return 0;
}

void CompactTriples::load(ModifiableTriples &triples, ProgressListener *listener) {
	triples.sort(order);

	IteratorTripleID *it = triples.searchAll();

	vector<unsigned int> vectorY, vectorZ;
	unsigned int lastX=0, lastY=0, lastZ=0; // FIXME: lastZ is assigned, below, but never actually used?
	unsigned int x, y, z;

	// First triple
	if(it->hasNext()) {
		TripleID *triple = it->next();

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
		//cout << "111> " << triple << endl;

		swapComponentOrder(triple, SPO, order);
		//cout << "222> " << triple << endl;

		x = triple->getSubject();
		y = triple->getPredicate();
		z = triple->getObject();

		if(x!=lastX) {
			vectorY.push_back(0);
			vectorY.push_back(y);

			vectorZ.push_back(0);
			vectorZ.push_back(z);
		} else if(y!=lastY) {
			vectorY.push_back(y);
			vectorZ.push_back(0);
			vectorZ.push_back(z);
		} else {
			vectorZ.push_back(z);
		}

		lastX = x;
		lastY = y;
		lastZ = z;

		NOTIFYCOND(listener, "Converting to CompactTriples.", numTriples, triples.getNumberOfElements());
		numTriples++;
	}

	delete it;

	VectorUIntIterator itY(vectorY);
	VectorUIntIterator itZ(vectorZ);

	streamY->add(itY);
	streamZ->add(itZ);

#if 0
	// Debug Adjacency Lists
	cout << "Y" << vectorY.size() << "): ";
	for(unsigned int i=0;i<arrayY->getNumberOfElements();i++){
		cout << arrayY->get(i) << " ";
	}
	cout << endl;

	cout << "Z" << vectorZ.size() << "): ";
	for(unsigned int i=0;i<arrayZ->getNumberOfElements();i++){
		cout << arrayZ->get(i) << " ";
	}
	cout << endl;
#endif

}

void CompactTriples::populateHeader(Header &header, string rootNode) {
	header.insert(rootNode, HDTVocabulary::TRIPLES_TYPE, HDTVocabulary::TRIPLES_TYPE_COMPACT);
	header.insert(rootNode, HDTVocabulary::TRIPLES_NUM_TRIPLES, getNumberOfElements() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_ORDER, getOrderStr(order) );
	header.insert(rootNode, HDTVocabulary::TRIPLES_SEQY_TYPE, streamY->getType() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_SEQZ_TYPE, streamZ->getType() );
}

IteratorTripleID *CompactTriples::search(TripleID & pattern)
{
	if(pattern.isEmpty()) {
		return new CompactTriplesIterator(this, pattern);
	} else {
		return new SequentialSearchIteratorTripleID(pattern, new CompactTriplesIterator(this, pattern));
	}
}

IteratorTripleID *CompactTriples::searchJoin(TripleID &a, TripleID &b, unsigned short conditions) {
	throw std::logic_error("Not Implemented");
}

void CompactTriples::save(std::ostream & output, ControlInformation &controlInformation, ProgressListener *listener)
{
	controlInformation.clear();
	controlInformation.setUint("numTriples", getNumberOfElements());
	controlInformation.setFormat(HDTVocabulary::TRIPLES_TYPE_COMPACT);
	controlInformation.setUint("order", order);
	controlInformation.save(output);

	IntermediateListener iListener(listener);

	iListener.setRange(0,30);
	iListener.notifyProgress(0, "CompactTriples saving Stream Y");
	streamY->save(output);

	iListener.setRange(30,100);
	iListener.notifyProgress(0, "CompactTriples saving Stream Z");
	streamZ->save(output);
}

void CompactTriples::load(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener)
{
	std::string format = controlInformation.getFormat();
	if(format != HDTVocabulary::TRIPLES_TYPE_COMPACT) {
		throw std::runtime_error("Trying to read CompactTriples but data is not CompactTriples");
	}

	numTriples = controlInformation.getUint("numTriples");
	order = (TripleComponentOrder) controlInformation.getUint("order");

	IntermediateListener iListener(listener);

	iListener.setRange(0,30);
	iListener.notifyProgress(0, "CompactTriples loading Stream Y");
	delete streamY;
	streamY = IntSequence::getArray(input);
	streamY->load(input);

	iListener.setRange(30,100);
	iListener.notifyProgress(0, "CompactTriples saving Stream Y");
	delete streamZ;
	streamZ = IntSequence::getArray(input);
    streamZ->load(input);
}

size_t CompactTriples::load(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener)
{
    throw std::logic_error("Not Implemented");
}

void CompactTriples::generateIndex(ProgressListener *listener) {

}

void CompactTriples::saveIndex(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener) {

}

void CompactTriples::loadIndex(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener) {

}

size_t CompactTriples::loadIndex(unsigned char *ptr, unsigned char *ptrMax, ProgressListener *listener)
{
	return 0;
}

size_t CompactTriples::getNumberOfElements() const
{
	return numTriples;
}

size_t CompactTriples::size() const
{
	return streamY->size()+streamZ->size();
}

string CompactTriples::getType() const
{
    return HDTVocabulary::TRIPLES_TYPE_COMPACT;
}

TripleComponentOrder CompactTriples::getOrder() const
{
    return order;
}

/// ITERATOR
CompactTriplesIterator::CompactTriplesIterator(CompactTriples *trip, TripleID &pat) :
		triples(trip),
		pattern(pat)
{
	// Convert pattern to local order.
	swapComponentOrder(&pattern, SPO, triples->order);
	patX = pattern.getSubject();
	patY = pattern.getPredicate();
	patZ = pattern.getObject();

	goToStart();
}

void CompactTriplesIterator::updateOutput() {
	// Convert local order to SPO
	returnTriple.setAll(x,y,z);
	swapComponentOrder(&returnTriple, triples->order, SPO);
}

bool CompactTriplesIterator::hasNext()
{
	return posZ<triples->streamZ->getNumberOfElements();
}

TripleID *CompactTriplesIterator::next()
{
	//cout << "\t\tposZ=" << posZ << "("<< triples->streamZ->get(posZ)<<") posY="<< posY << "("<< triples->streamY->get(posY) << ")" << endl;

	if(!goingUp) {
		posY++;
		goingUp=true;
	}
	z = triples->streamZ->get(posZ++);

	if(z==0) {
		z = triples->streamZ->get(posZ++);

		y = triples->streamY->get(posY++);
		if(y==0) {
			y = triples->streamY->get(posY++);
			x++;
		}
	}

	updateOutput();

	return &returnTriple;
}

bool CompactTriplesIterator::hasPrevious()
{
	return posZ>0;
}

TripleID *CompactTriplesIterator::previous()
{
	//cout << "\t\tposZ=" << posZ << "("<< triples->streamZ->get(posZ)<<") posY="<< posY << "("<< triples->streamY->get(posY) << ")" << endl;
	z = triples->streamZ->get(--posZ);

	if(goingUp) {
		posY--;
		goingUp = false;
	}

	if(z==0) {
		z = triples->streamZ->get(--posZ);

		y = triples->streamY->get(--posY);
		if(y==0) {
			y = triples->streamY->get(--posY);
			x--;
		}
	}

	updateOutput();

	return &returnTriple;
}

void CompactTriplesIterator::goToStart()
{
	posY = posZ = 0;
	x = 1;
	y = triples->streamY->get(posY++);
	goingUp = true;
}

}
