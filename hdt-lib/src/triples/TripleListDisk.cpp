/*
 * TripleListDisk.cpp
 *
 *  Created on: 07/03/2011
 *      Author: mck
 */


#include "TripleListDisk.hpp"

using namespace std;

namespace hdt {

TripleListDisk::TripleListDisk() : fileName(tmpnam(NULL)), outStream(0), numTriples(0) {
	cout << "TripleListDisk: "<< fileName << endl;
}

TripleListDisk::~TripleListDisk() {
	// TODO Auto-generated destructor stub
}

void TripleListDisk::load(ModifiableTriples & input)
{
	TripleID all(0,0,0);
	IteratorTripleID it = input.search(all);

	startProcessing();
	insert(it);
	stopProcessing();
}

void TripleListDisk::load(std::istream & input, Header & header)
{
}

bool TripleListDisk::save(std::ostream & output)
{
	ifstream in(fileName);

	std::copy(std::istream_iterator<unsigned char>(in),	std::istream_iterator<unsigned char>(),	std::ostream_iterator<unsigned char>(output));

	in.close();
}

void TripleListDisk::startProcessing()
{
	cout << "TripleListDisk Start processing" << endl;
	outStream = new std::ofstream(fileName);
}

void TripleListDisk::stopProcessing()
{
	cout << "TripleListDisk Stop processing" << endl;
	outStream->close();
	delete outStream;
}

void TripleListDisk::populateHeader(Header &header)
{
}

unsigned int TripleListDisk::getNumberOfElements()
{
	return numTriples;
}

unsigned int TripleListDisk::size()
{
	return getNumberOfElements()*sizeof(TripleID);
}


IteratorTripleID TripleListDisk::search(TripleID &pattern)
{
	TripleListDiskIterator it(fileName);
	return it;
}

float TripleListDisk::cost(TripleID & triple)
{
	// TODO:
	throw "Not implemented";
}


bool TripleListDisk::insert(TripleID & triple)
{
	cout << "Insert: " << triple << endl;
	outStream->write((char *)&triple, sizeof(TripleID));
	numTriples++;
}

bool TripleListDisk::insert(IteratorTripleID & triples)
{
	while(triples.hasNext()) {
		TripleID triple = triples.next();
		insert(triple);
	}
}

bool TripleListDisk::remove(TripleID & pattern)
{
}

bool TripleListDisk::remove(IteratorTripleID & triples)
{

}

bool TripleListDisk::edit(TripleID & oldTriple, TripleID & newTriple)
{
	remove(oldTriple);
	insert(newTriple);
}

void TripleListDisk::sort(TripleComponentOrder order)
{
}

void TripleListDisk::setOrder(TripleComponentOrder order)
{
}



}
