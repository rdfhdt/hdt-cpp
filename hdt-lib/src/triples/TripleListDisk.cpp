/*
 * TripleListDisk.cpp
 *
 *  Created on: 07/03/2011
 *      Author: mck
 */


#include "TripleListDisk.hpp"

using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/mman.h> // For mmap


namespace hdt {

TripleListDisk::TripleListDisk() : fileName(tmpnam(NULL)), stream(fileName, std::ios::in | std::ios::out | std::ios::trunc), numTriples(0) {
	cout << "TripleListDisk: "<< fileName << endl;
}

TripleListDisk::~TripleListDisk() {
	// TODO Auto-generated destructor stub
}

void TripleListDisk::load(ModifiableTriples & input)
{
	TripleID all(0,0,0);
	IteratorTripleID *it = input.search(all);

	startProcessing();
	insert(it);
	stopProcessing();
}

void TripleListDisk::load(std::istream & input, Header & header)
{
	stream.close();
	stream.open(fileName, std::ios::out | std::ios::trunc);

	unsigned int type;
	unsigned int ntriples;

	input.read((char *)&type, sizeof(unsigned int));
	input.read((char *)&ntriples, sizeof(unsigned int));

	cout << "Reading input type: " << type << endl;
	cout << "Reading num triples: " << ntriples << endl;

	unsigned int numRead=0;
	TripleID tid;
	while(input.good() && numRead<ntriples) {
		input.read((char *)&tid, sizeof(TripleID));

		if(!input.fail()) {
			stream.write((char *)&tid, sizeof(TripleID));
		}
		numRead++;
	}
	cout << "Read succesfully triples: " << numRead << endl;
	this->numTriples = numRead;

	stream.flush();
}

bool TripleListDisk::save(std::ostream & output)
{
	stream.clear();
	stream.seekg(0);

	unsigned int numSaved=0;
	unsigned int type=1;

	cout << "Saving triples: " << numTriples << endl;

	output.write((char *)&type, sizeof(unsigned int));
	output.write((char *)&numTriples, sizeof(unsigned int));

	TripleID tid;
	while(stream.good() && numSaved < numTriples) {
		stream.read((char *)&tid, sizeof(TripleID));
		if(!stream.fail()) {
			output.write((char *)&tid, sizeof(TripleID));
		}
		numSaved++;
	}
}

void TripleListDisk::startProcessing()
{
	cout << "TripleListDisk Start processing" << endl;
	stream.clear();
	stream.seekg(0);
}

void TripleListDisk::stopProcessing()
{
	cout << "TripleListDisk Stop processing: Triples=" <<numTriples<< endl;
	stream.flush();
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


IteratorTripleID *TripleListDisk::search(TripleID &pattern)
{
	TripleListDiskIterator *it=new TripleListDiskIterator(fileName, pattern);
	return it;
}

float TripleListDisk::cost(TripleID & triple)
{
	// TODO:
	throw "Not implemented";
}


bool TripleListDisk::insert(TripleID &triple)
{
	cout << "Insert: " << triple << endl;
	stream.write((char *)&triple, sizeof(TripleID));
	numTriples++;
}

bool TripleListDisk::insert(IteratorTripleID *triples)
{
	while(triples->hasNext()) {
		TripleID triple = triples->next();
		insert(triple);
	}
}

bool TripleListDisk::remove(TripleID & pattern)
{
	stream.close();

	int fd = open(fileName, O_RDWR);
    if (fd == -1) {
    	throw "Error open";
    }

    char *pointer = (char *) mmap(0, numTriples*sizeof(TripleID), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    if(pointer==(char *)-1) {
    	throw "Could not mmap";
    }

	// FIND
	TripleID *tid;

	for(unsigned int i=0; i<numTriples; i++) {
		tid = (TripleID *) &pointer[i*sizeof(TripleID)];

		if (tid->match(pattern)) {
			tid->setSubject(0);
			tid->setPredicate(0);
			tid->setObject(0);
		}
	}

	munmap(pointer, numTriples*sizeof(TripleID));

	close(fd);
}

bool TripleListDisk::remove(IteratorTripleID *triples)
{
	/*while(stream.good() && numRead < numTriples) {
		stream.read((char *)tid, sizeof(TripleID));
		if(!stream.fail()) {
			if(tid.match(pattern)) {
				tid.setSubject(0);
				tid.setPredicate(0);
				tid.setObject(0);

				stream.seekp(stream.tellg());

				stream.write((char *)tid, sizeof(TripleID));
			}
		}
		numRead++;
	}*/
}

bool TripleListDisk::edit(TripleID & oldTriple, TripleID & newTriple)
{
	remove(oldTriple);
	startProcessing();
	insert(newTriple);
	stopProcessing();
}

void TripleListDisk::sort(TripleComponentOrder order)
{
}

void TripleListDisk::setOrder(TripleComponentOrder order)
{
}

///// ITERATOR

bool invalidTriple(TripleID &triple) {
	return triple.getSubject()==0 || triple.getPredicate()==0 || triple.getObject()==0;
}

void TripleListDiskIterator::doFetch() {
	cout << "Fetch " << endl;
	do {
		if(stream.good()) {
			stream.read((char *)&nextv, sizeof(TripleID));

			hasNextv = !stream.fail();
		} else {
			hasNextv = false;
		}
	} while (hasNextv && (invalidTriple(nextv) || !nextv.match(pattern)));
}

TripleListDiskIterator::TripleListDiskIterator(char *tmpfile, TripleID &p) : stream(tmpfile), pattern(p), hasNextv(true) {
	doFetch();
}

TripleListDiskIterator::~TripleListDiskIterator(){
	if(stream.good())
		stream.close();
}

bool TripleListDiskIterator::hasNext() {
	return hasNextv;
}

TripleID TripleListDiskIterator::next() {
	TripleID previousv = nextv;
	doFetch();
	return previousv;
}


}
