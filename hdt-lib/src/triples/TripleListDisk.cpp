/*
 * File: TripleListDisk.cpp
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
		numValidTriples(0),
		numTotalTriples(0),
		arrayTriples(NULL)
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
	cout << "\t: " <<&v[0] << endl;
	cout << "\t: " <<s << endl;

	this->increaseSize();
	this->mapFile();
}

TripleListDisk::~TripleListDisk() {
	this->unmapFile();
	close(fd);

	if(unlink(fileName.c_str())==-1) {
		perror("Unlinking tmp file");
	}
}

void TripleListDisk::mapFile() {
	if(arrayTriples!=NULL) {
		return;
	}

	getFileSize();

#ifndef WIN32
	cout << "Mapping: " << mappedSize << endl;
	// Map File to memory
	arrayTriples = (TripleID *) mmap(0, mappedSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
#endif
	if(arrayTriples==(TripleID *)-1) {
		throw "Could not mmap";
	}
}

void TripleListDisk::unmapFile() {
	if(arrayTriples!=NULL && arrayTriples!=(TripleID *)-1) {
		cout << "UNMAP" << endl;

#ifndef WIN32
		munmap(arrayTriples, mappedSize);
#endif
		arrayTriples=NULL;
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
    ensureSize(capacity+(1024*1024));
}

void TripleListDisk::ensureSize(unsigned int newsize) {
	if(capacity>=newsize) {
		return;
	}

	unmapFile();

	int pos = lseek(fd, newsize*sizeof(TripleID)-1, SEEK_SET);
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
	if(num>numTotalTriples || arrayTriples==NULL) {
		return NULL;
	}

	return &arrayTriples[num];
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
	// FIXME: Read controlInformation

	this->ensureSize(numTotalTriples);

	unsigned int numRead=0;
	while(input.good() && numRead<numTotalTriples) {
		input.read((char *)&arrayTriples[numRead], sizeof(TripleID));
		numRead++;
	}
	cout << "Succesfully read triples: " << numRead << endl;
}



void TripleListDisk::save(std::ostream & output, ControlInformation &controlInformation, ProgressListener *listener)
{
	cout << "Saving triples: " << numValidTriples << endl;

	//FIXME: Fill controlInformation

	for(unsigned int i=0; i<numTotalTriples; i++) {
		TripleID *tid = getTripleID(i);
		if(tid->isValid()) {
			//cout << "Write: " << tid << " " << *tid << endl;
			output.write((char *)tid, sizeof(TripleID));
		}
	}
}

void TripleListDisk::startProcessing(ProgressListener *listener)
{
	cout << "TripleListDisk Start processing" << endl;
}

void TripleListDisk::stopProcessing(ProgressListener *listener)
{
	sort(SPO);
	removeDuplicates();
	cout << "TripleListDisk Stop processing: Triples=" << numValidTriples<< endl;
}

void TripleListDisk::populateHeader(Header &header, string rootNode)
{
	header.insert(rootNode, HDTVocabulary::TRIPLES_TYPE, HDTVocabulary::TRIPLES_TYPE_TRIPLESLISTDISK);
	header.insert(rootNode, HDTVocabulary::TRIPLES_NUM_TRIPLES, getNumberOfElements() );
	header.insert(rootNode, HDTVocabulary::TRIPLES_ORDER, order );  // TODO: Convert to String
}

unsigned int TripleListDisk::getNumberOfElements()
{
	return numValidTriples;
}

size_t TripleListDisk::size()
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

IteratorTripleID *TripleListDisk::searchJoin(TripleID &a, TripleID &b, unsigned short conditions) {
	throw "Not implemented";
}

float TripleListDisk::cost(TripleID & triple)
{
	// TODO:
	throw "Not implemented";
}

void TripleListDisk::generateIndex(ProgressListener *listener) {

}

void TripleListDisk::saveIndex(std::ostream &output, ControlInformation &controlInformation, ProgressListener *listener) {

}

void TripleListDisk::loadIndex(std::istream &input, ControlInformation &controlInformation, ProgressListener *listener) {

}




void TripleListDisk::insert(TripleID &triple)
{
	if(arrayTriples==NULL) {
		throw "Invalid pointer";
	}

	if(numTotalTriples>=capacity) {
		increaseSize();
	}

	//cout << "Insert: " <<&pointer[numTotalTriples] << "* "<< triple << " "<<sizeof(TripleID) << endl;

	memcpy(&arrayTriples[numTotalTriples], &triple, sizeof(TripleID));
	numTotalTriples++;
	numValidTriples++;
	//cout << "Inserted: "<< numTotalTriples << endl;
}

void TripleListDisk::insert(IteratorTripleID *triples)
{
	while(triples->hasNext()) {
		TripleID *triple = triples->next();
		insert(*triple);
	}
}

bool TripleListDisk::remove(TripleID & pattern)
{
	TripleID *tid;

	bool removed = false;
	for(tid=arrayTriples; tid<arrayTriples+numTotalTriples; tid++) {
		if (tid->match(pattern)) {
			tid->clear();
			numValidTriples--;
			removed = true;
		}
	}

	return removed;
}

bool TripleListDisk::remove(IteratorTripleID *triples)
{
	TripleID *tid;

	vector<TripleID> allPat;

	while(triples->hasNext()) {
		allPat.push_back(*triples->next());
	}

	bool removed = false;
	for(tid=arrayTriples; tid<arrayTriples+numTotalTriples; tid++) {
		for(int i=0; i<allPat.size(); i++) {
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

void TripleListDisk::sort(TripleComponentOrder order, ProgressListener *listener)
{
	// SORT
	// FIXME: USE specified order
	// FIXME: Use TripleComparator class
	// FIXME: Sort by blocks and merge
//	StopWatch st;

	if(this->order != order) {
		qsort(arrayTriples, numTotalTriples, sizeof(TripleID), tripleIDcmp);
		this->order = order;
	}
}

void TripleListDisk::removeDuplicates(ProgressListener *listener) {
    if(numTotalTriples<=1)
	    return;

    if(order==Unknown){
	    throw "Cannot remove duplicates on unordered triples";
    }

    unsigned int j = 0;
    //StopWatch st;

    for(unsigned int i=1; i<numTotalTriples; i++) {
	    if(arrayTriples[i] != arrayTriples[j]) {
		    j++;
		    arrayTriples[j] = arrayTriples[i];
	    }
	    NOTIFYCOND(listener, "Removing duplicate triples", i, numTotalTriples);
    }

    //cout << "Removed "<< arrayOfTriples.size()-j-1 << " duplicates in " << st << endl;
    numValidTriples = j+1;
}

void TripleListDisk::setOrder(TripleComponentOrder order)
{
}


string TripleListDisk::getType() {
    return HDTVocabulary::TRIPLES_TYPE_TRIPLESLISTDISK;
}

TripleComponentOrder TripleListDisk::getOrder()
{
    return order;
}

///// ITERATOR

TripleListDiskIterator::TripleListDiskIterator(TripleListDisk *t, TripleID p) : triples(t), pattern(p) {
}

TripleListDiskIterator::~TripleListDiskIterator()
{
}


bool TripleListDiskIterator::hasNext()
{
	return pos<triples->numValidTriples;
}

TripleID *TripleListDiskIterator::next()
{
	returnTriple = *triples->getTripleID(pos++);
	return &returnTriple;
}

bool TripleListDiskIterator::hasPrevious()
{
	return pos>0;
}

TripleID *TripleListDiskIterator::previous()
{
	returnTriple = *triples->getTripleID(--pos);
	return &returnTriple;
}

void TripleListDiskIterator::goToStart()
{
    pos=0;
}



}
