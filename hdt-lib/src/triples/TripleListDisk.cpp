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

#include <stdlib.h> // for qsort
#include <sys/mman.h> // For mmap
#include <string.h> // memcpy

#include "../util/StopWatch.hpp"

namespace hdt {

TripleListDisk::TripleListDisk() :
		numTotalTriples(0),
		numValidTriples(0),
		capacity(0),
		pointer(NULL)
{
	strcpy(fileName, "triplelistdiskXXXXXX");

	fd = mkostemp(fileName, O_RDWR | O_CREAT | O_TRUNC);
	if (fd == -1) {
		perror("Error open");
		throw "Error open";
	}

	cout << "TriplelistDisk: " << fileName << endl;

	this->increaseSize();
	this->mapFile();
}

TripleListDisk::~TripleListDisk() {
	this->unmapFile();
	close(fd);
	int res = unlink(fileName);
	if(res==-1) {
		perror("Unlinking tmp file");
	}
}

void TripleListDisk::mapFile() {
	if(pointer!=NULL) {
		return;
	}

	//cout << "MAP" << endl;
	pointer = (TripleID *) mmap(0, capacity*sizeof(TripleID), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	if(pointer==(TripleID *)-1) {
		throw "Could not mmap";
	}
}

void TripleListDisk::unmapFile() {
	if(pointer!=NULL && pointer!=(TripleID *)-1) {
		//cout << "UNMAP" << endl;
		munmap(pointer, capacity*sizeof(TripleID));
		pointer=NULL;
	}
}

void TripleListDisk::increaseSize() {
	ensureSize(capacity+4096);
}

// NOTE: Must be closed before calling.
void TripleListDisk::ensureSize(unsigned int size) {
	if(capacity>=size) {
		return;
	}

	unmapFile();

	//cout << "Increase size: " << size << endl;
	capacity = size;

	int pos = lseek(fd, 3*capacity*sizeof(unsigned int)-1, SEEK_SET);
	if(pos==-1) {
		perror("Error lseek");
		throw "Error lseek";
	}
	//cout << "Seek pos: "<< pos << endl;

	char c = 0;
	int wr = write(fd, &c, 1);
	if(wr==-1) {
		perror("Error write");
		throw "Error write";
	}
	//cout << "Written: " << wr << endl;;
	fsync(fd);

	mapFile();
}

TripleID *TripleListDisk::getTripleID(unsigned int num) {
	if(num>numTotalTriples || pointer==NULL) {
		return NULL;
	}

	return &pointer[num];
}

bool invalidTriple(TripleID *triple) {
	return triple->getSubject()==0 || triple->getPredicate()==0 || triple->getObject()==0;
}

void TripleListDisk::load(ModifiableTriples & input)
{
	TripleID all(0,0,0);
	IteratorTripleID *it = input.search(all);

	startProcessing();
	ensureSize(input.getNumberOfElements());
	insert(it);
	stopProcessing();
}

void TripleListDisk::load(std::istream & input, Header & header)
{
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
			insert(tid);
		}
		numRead++;
	}
	cout << "Succesfully read triples: " << numRead << endl;
}



bool TripleListDisk::save(std::ostream & output)
{
	unsigned int numSaved=0;
	unsigned int type=1;

	cout << "Saving triples: " << numValidTriples << endl;

	output.write((char *)&type, sizeof(unsigned int));
	output.write((char *)&numValidTriples, sizeof(unsigned int));

	//for(tid=pointer; tid<pointer+numTotalTriples; tid++) {
	for(unsigned int i=0; i<numTotalTriples; i++) {
		TripleID *tid = getTripleID(i);
		if(!invalidTriple(tid)) {
			//cout << "Write: " << tid << " " << *tid << endl;
			output.write((char *)tid, sizeof(TripleID));
		}
	}
}

void TripleListDisk::startProcessing()
{
	cout << "TripleListDisk Start processing" << endl;
}

void TripleListDisk::stopProcessing()
{
	sort(SPO);
	removeDuplicates();
	cout << "TripleListDisk Stop processing: Triples=" <<numValidTriples<< endl;
}

void TripleListDisk::populateHeader(Header &header)
{
}

unsigned int TripleListDisk::getNumberOfElements()
{
	return numValidTriples;
}

unsigned int TripleListDisk::size()
{
	return getNumberOfElements()*sizeof(TripleID);
}


IteratorTripleID *TripleListDisk::search(TripleID &pattern)
{
	if(numValidTriples>0) {
		return new TripleListDiskIterator(this, pattern);
	} else {
		return new IteratorTripleID();
	}
}

float TripleListDisk::cost(TripleID & triple)
{
	// TODO:
	throw "Not implemented";
}


bool TripleListDisk::insert(TripleID &triple)
{
	if(pointer==NULL) {
		throw "Invalid pointer";
	}

	if(numTotalTriples>=capacity) {
		increaseSize();
	}

	//cout << "Insert: " <<&pointer[numTotalTriples] << "* "<< triple << " "<<sizeof(TripleID) << endl;

	memcpy(&pointer[numTotalTriples], &triple, sizeof(TripleID));
	numTotalTriples++;
	numValidTriples++;
	//cout << "Inserted: "<< numTotalTriples << endl;

	return true;
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
	TripleID *tid;

	for(tid=pointer; tid<pointer+numTotalTriples; tid++) {
		if (tid->match(pattern)) {
			tid->clear();
			numValidTriples--;
		}
	}
}

bool TripleListDisk::remove(IteratorTripleID *triples)
{
	TripleID *tid;

	vector<TripleID> allPat;

	while(triples->hasNext()) {
		allPat.push_back(triples->next());
	}

	for(tid=pointer; tid<pointer+numTotalTriples; tid++) {
		for(int i=0; i<allPat.size(); i++) {
			if (tid->match(allPat[i])) {
				tid->clear();
				numValidTriples--;
			}
		}
	}
}

bool TripleListDisk::edit(TripleID & oldTriple, TripleID & newTriple)
{
	remove(oldTriple);
	startProcessing();
	insert(newTriple);
	stopProcessing();
}



#if 0
// Not used yet
void radixSort(unsigned int *a, size_t size, int bits) {
	unsigned int mask;
	unsigned int rshift=0u;
	unsigned int *p, *b, *b_orig;
	unsigned int i;
	unsigned int key;
	int cntsize;
	int *cntarray;

	b=b_orig=(unsigned int *)malloc(size*sizeof(unsigned int));

	cntsize=1u<<bits;
	cntarray=(int *)calloc(cntsize, sizeof(int));

	for(mask=~(UINT_MAX<<bits); mask; mask<<=bits, rshift+=bits) {
		for(p=a; p<a+size; p++) {
			key=(*p & mask)>>rshift;
			cntarray[key]++;
		}


		for(i=1; i<cntsize; i++) {
			cntarray[i] += cntarray[i-1];
		}

		for(p=a+size-1; p>=a; --p) {
			key=(*p & mask)>>rshift;
			b[cntarray[key]-1]=*p;
			--cntarray[key];
		}

		p=b; b=a; a=p;

		bzero(cntarray, cntsize * sizeof(int));
	}


	if(a==b_orig) memcpy(b, a, size*sizeof(unsigned int));

	free(b_orig);
	free(cntarray);

}
#endif

int tripleIDcmp(const void *a, const void *b) {
	TripleID *t1 = (TripleID *) a;
	TripleID *t2 = (TripleID *) b;

	return t1->compare(*t2);
}

void TripleListDisk::sort(TripleComponentOrder order)
{
	// SORT
	// FIXME: USE specified order
	StopWatch st;

	qsort(pointer, numTotalTriples, sizeof(TripleID), tripleIDcmp);

	cout << "Sorted in " << st << endl;
}

void TripleListDisk::removeDuplicates() {

	TripleID previous = *pointer;
	StopWatch st;

	unsigned int numduplicates = 0;

	for(TripleID *tid = pointer+1; tid<pointer+numTotalTriples; tid++) {
		//cout << "Compare: " << &previous << " "<<previous << " New: " << tid << " "<<*tid << endl;
		if(previous == *tid) {
			tid->clear();
			numValidTriples--;
			numduplicates++;
		} else {
			previous = *tid;
		}
	}

	cout << "Removed "<< numduplicates << " duplicates in " << st << endl;
}

void TripleListDisk::setOrder(TripleComponentOrder order)
{
}

///// ITERATOR

void TripleListDiskIterator::doFetch() {

	do {
		nextv = triples->getTripleID(pos);
		pos++;
	} while(pos<=triples->numTotalTriples && (invalidTriple(nextv) || !nextv->match(pattern)));

	hasNextv= pos<=triples->numTotalTriples;
}

TripleListDiskIterator::TripleListDiskIterator(TripleListDisk *t, TripleID &p) : triples(t), pattern(p), hasNextv(true) {
	doFetch();
}

TripleListDiskIterator::~TripleListDiskIterator(){

}

bool TripleListDiskIterator::hasNext() {
	return hasNextv;
}

TripleID TripleListDiskIterator::next() {
	TripleID previousv = *nextv;
	doFetch();
	return previousv;
}


}
