/*
 * TripleListDisk.cpp
 *
 *  Created on: 07/03/2011
 *      Author: mck
 */

#include <HDTVocabulary.hpp>

#include "TripleListDisk.hpp"

using namespace std;

#include <sys/types.h>
#include <sys/stat.h>	// stat()
#include <fcntl.h>

#include <stdlib.h> // for qsort
#ifndef WIN32
#include <sys/mman.h> // For mmap
#endif
#include <string.h> // memcpy

#include "../util/StopWatch.hpp"

namespace hdt {

TripleListDisk::TripleListDisk() :
		capacity(0),
		pointer(NULL),
		arrayTriples(NULL),
		isTemp(true)
{
	std::string s("triplelistdiskXXXXXX");
	std::vector<char> v(100);
	v.assign( s.begin(), s.end() );

#ifdef __APPLE__
	fd = mkstemp(&v[0]);
#elif defined(WIN32)

#else
	fd = mkostemp(&v[0], O_RDWR | O_CREAT | O_TRUNC);
#endif

	if (fd == -1) {
		perror("Error open");
		throw "Error open";
	}

	fileName.assign( &v[0] );

	cout << "TriplelistDisk: " <<fileName << endl;
//	cout << "\t: " <<&v[0] << endl;
//	cout << "\t: " <<s << endl;

	this->increaseSize();
	this->mapFile();

	tripleHead->type = 0;
	tripleHead->numValidTriples = 0;
	tripleHead->numTotalTriples = 0;
}

TripleListDisk::TripleListDisk(const char *file) :
		capacity(0),
		pointer(NULL),
		arrayTriples(NULL),
		isTemp(false)
{
	fileName.assign(file);

	fd = open(fileName.c_str(), O_RDWR | O_CREAT );
	if (fd == -1) {
		perror("Error open");
		throw "Error open";
	}

	this->getFileSize();
	if(mappedSize<sizeof(struct TripleListHeader)) {
		this->increaseSize();
	}

	this->mapFile();
}

TripleListDisk::~TripleListDisk() {
	this->unmapFile();
	close(fd);
	if(isTemp) {
		int res = unlink(fileName.c_str());
		if(res==-1) {
			perror("Unlinking tmp file");
		}
	}
}

void TripleListDisk::mapFile() {
	if(pointer!=NULL) {
		return;
	}

	getFileSize();

#ifndef WIN32
	// Map File to memory
	pointer = (char *) mmap(0, mappedSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
#endif
	if(pointer==(char *)-1) {
		throw "Could not mmap";
	}

	// Adjust pointers
	arrayTriples = (TripleID *) pointer+sizeof(struct TripleListHeader);
	tripleHead = (struct TripleListHeader *) pointer;
}

void TripleListDisk::unmapFile() {
	if(pointer!=NULL && pointer!=(char *)-1) {
		//cout << "UNMAP" << endl;

#ifndef WIN32
		munmap(pointer, mappedSize);
#endif
                pointer=NULL;
		arrayTriples=NULL;
		tripleHead=NULL;
	}
}

void TripleListDisk::getFileSize() {
	// Read File size
	struct stat st;
	int ret = fstat(fd, &st);
	if(ret==-1) {
		perror("Error fstat");
		throw "Error fstat";
	}
	mappedSize = st.st_size;
}

void TripleListDisk::increaseSize() {
	ensureSize(capacity+4096);
}

void TripleListDisk::ensureSize(unsigned int newsize) {
	if(capacity>=newsize) {
		return;
	}

	unmapFile();

	int pos = lseek(fd, sizeof(struct TripleListHeader) + newsize*sizeof(TripleID)-1, SEEK_SET);
	if(pos==-1) {
		perror("Error lseek");
		throw "Error lseek";
	}

	char c = 0;
	int wr = write(fd, &c, 1);
	if(wr==-1) {
		perror("Error write");
		throw "Error write";
	}

#ifndef WIN32
	fsync(fd);
#endif

	capacity = newsize;

	mapFile();
}

TripleID *TripleListDisk::getTripleID(unsigned int num) {
	if(num>tripleHead->numTotalTriples || arrayTriples==NULL) {
		return NULL;
	}

	return &arrayTriples[num];
}

bool invalidTriple(TripleID *triple) {
	return triple->getSubject()==0 || triple->getPredicate()==0 || triple->getObject()==0;
}

void TripleListDisk::load(ModifiableTriples & input, ProgressListener *listener)
{
	TripleID all(0,0,0);
	IteratorTripleID *it = input.search(all);

	startProcessing();
	ensureSize(input.getNumberOfElements());
	insert(it);
	stopProcessing();
}

void TripleListDisk::load(std::istream & input, ControlInformation &controlInformation, ProgressListener *listener)
{
	input.read((char *)tripleHead, sizeof(TripleListHeader));

	cout << "Reading input type: " << tripleHead->type << endl;
	cout << "Reading num triples: " << tripleHead->numTotalTriples << endl;

	this->ensureSize(tripleHead->numTotalTriples);

	unsigned int numRead=0;
	while(input.good() && numRead<tripleHead->numTotalTriples) {
		input.read((char *)&arrayTriples[numRead], sizeof(TripleID));
		numRead++;
	}
	cout << "Succesfully read triples: " << numRead << endl;
}



bool TripleListDisk::save(std::ostream & output, ControlInformation &controlInformation, ProgressListener *listener)
{
	cout << "Saving triples: " << tripleHead->numValidTriples << endl;

	// Since we only save valid triples, adjust the header of the saved file.
	struct TripleListHeader myhead;
	myhead.type = tripleHead->type;
	myhead.numValidTriples = tripleHead->numValidTriples;
	myhead.numTotalTriples = tripleHead->numValidTriples;

	output.write((char *)&myhead, sizeof(struct TripleListHeader));

	for(unsigned int i=0; i<tripleHead->numTotalTriples; i++) {
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
	cout << "TripleListDisk Stop processing: Triples=" << tripleHead->numValidTriples<< endl;
}

void TripleListDisk::populateHeader(Header &header, string rootNode)
{
	header.insert(rootNode, HDTVocabulary::TRIPLES_TYPE, HDTVocabulary::TRIPLES_TYPE_TRIPLESLISTDISK);
	header.insert(rootNode, HDTVocabulary::TRIPLES_NUM_TRIPLES, getNumberOfElements() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_ORDER, order );  // TODO: Convert to String
}

unsigned int TripleListDisk::getNumberOfElements()
{
	return tripleHead->numValidTriples;
}

unsigned int TripleListDisk::size()
{
	return getNumberOfElements()*sizeof(TripleID);
}


IteratorTripleID *TripleListDisk::search(TripleID &pattern)
{
	if(tripleHead->numValidTriples>0) {
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
	if(arrayTriples==NULL) {
		throw "Invalid pointer";
	}

	if(tripleHead->numTotalTriples>=capacity) {
		increaseSize();
	}

	//cout << "Insert: " <<&pointer[numTotalTriples] << "* "<< triple << " "<<sizeof(TripleID) << endl;

	memcpy(&arrayTriples[tripleHead->numTotalTriples], &triple, sizeof(TripleID));
	tripleHead->numTotalTriples++;
	tripleHead->numValidTriples++;
	//cout << "Inserted: "<< numTotalTriples << endl;

	return true;
}

bool TripleListDisk::insert(IteratorTripleID *triples)
{
	while(triples->hasNext()) {
		TripleID *triple = triples->next();
		insert(*triple);
	}
}

bool TripleListDisk::remove(TripleID & pattern)
{
	TripleID *tid;

	for(tid=arrayTriples; tid<arrayTriples+tripleHead->numTotalTriples; tid++) {
		if (tid->match(pattern)) {
			tid->clear();
			tripleHead->numValidTriples--;
		}
	}
}

bool TripleListDisk::remove(IteratorTripleID *triples)
{
	TripleID *tid;

	vector<TripleID> allPat;

	while(triples->hasNext()) {
		allPat.push_back(*triples->next());
	}

	for(tid=arrayTriples; tid<arrayTriples+tripleHead->numTotalTriples; tid++) {
		for(int i=0; i<allPat.size(); i++) {
			if (tid->match(allPat[i])) {
				tid->clear();
				tripleHead->numValidTriples--;
			}
		}
	}
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
	// FIXME: Use TripleComparator class
	StopWatch st;

	qsort(arrayTriples, tripleHead->numTotalTriples, sizeof(TripleID), tripleIDcmp);

	cout << "Sorted in " << st << endl;
}

void TripleListDisk::removeDuplicates() {

	TripleID previous = *arrayTriples;
	StopWatch st;

	unsigned int numduplicates = 0;

	for(TripleID *tid = arrayTriples+1; tid<arrayTriples+tripleHead->numTotalTriples; tid++) {
		//cout << "Compare: " << &previous << " "<<previous << " New: " << tid << " "<<*tid << endl;
		if(previous == *tid) {
			tid->clear();
			tripleHead->numValidTriples--;
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


string TripleListDisk::getType() {
	return HDTVocabulary::TRIPLES_TYPE_TRIPLESLISTDISK;
}

///// ITERATOR

void TripleListDiskIterator::doFetch() {
	do {
		nextv = triples->getTripleID(pos);
		pos++;
	} while(pos<=triples->tripleHead->numTotalTriples && (!nextv->isValid() || !nextv->match(pattern)));

	hasNextv= pos<=triples->tripleHead->numTotalTriples;
}

TripleListDiskIterator::TripleListDiskIterator(TripleListDisk *t, TripleID p) : triples(t), pattern(p), hasNextv(true) {
	doFetch();
}

TripleListDiskIterator::~TripleListDiskIterator(){

}

bool TripleListDiskIterator::hasNext() {
	return hasNextv;
}

TripleID *TripleListDiskIterator::next() {
	ret = *nextv;
	doFetch();
	return &ret;
}


}
