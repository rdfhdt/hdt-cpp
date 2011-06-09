/*
 * TripleListDisk.h
 *
 *  Created on: 07/03/2011
 *      Author: mck
 */

#ifndef TRIPLELISTDISK_H_
#define TRIPLELISTDISK_H_

#include <fstream>

#include <Triples.hpp>

namespace hdt {

struct TripleListHeader {
	unsigned int type;
	unsigned int numValidTriples;
	unsigned int numTotalTriples;
};

class TripleListDisk : public ModifiableTriples {
private:
	TripleComponentOrder order;
	unsigned int capacity;

	char *pointer;
	struct TripleListHeader *tripleHead;
	TripleID *arrayTriples;

	size_t mappedSize;
	string fileName;
	int fd;
	bool isTemp;

	void mapFile();
	void unmapFile();
	void getFileSize();
	void increaseSize();
	void ensureSize(unsigned int size);


	void removeDuplicates();

	TripleID *getTripleID(unsigned int num);

public:
	TripleListDisk();
	TripleListDisk(const char *file);
	virtual ~TripleListDisk();

	// From Triples

	/**
	 * Returns a vector of triples matching the pattern
	 *
	 * @param pattern
	 * @return
	 */
	IteratorTripleID *search(TripleID &pattern);

	/**
	 * Calculates the cost to retrieve a specific pattern
	 *
	 * @param triple
	 * @return
	 */
	float cost(TripleID &triple);

	/**
	 * Returns the number of triples
	 *
	 * @return
	 */
	unsigned int getNumberOfElements();

	/**
	 * Returns size in bytes
	 */
	unsigned int size();

	/**
	 * Saves the triples
	 *
	 * @param output
	 * @return
	 */
	bool save(std::ostream &output, ControlInformation &controlInformation);


	void load(ModifiableTriples &input);

	/**
	 * Loads triples from a file
	 *
	 * @param input
	 * @return
	 */
	void load(std::istream &input, ControlInformation &controlInformation);


	void populateHeader(Header &header, string rootNode);

	void startProcessing();

	void stopProcessing();

	string getType();



	// From ModifiableTriples

	/**
	 * Adds one or more triples
	 *
	 * @param triples
	 *            The triples to be inserted
	 * @return boolean
	 */
	bool insert(TripleID &triple);

	bool insert(IteratorTripleID *triples);

	/**
	 * Deletes one or more triples according to a pattern
	 *
	 * @param pattern
	 *            The pattern to match against
	 * @return boolean
	 */
	bool remove(TripleID &pattern);

	bool remove(IteratorTripleID *pattern);

	/**
	 * Sorts the triples based on an order(TripleComponentOrder)
	 *
	 * @param order
	 *            The order to sort the triples with
	 */
	void sort(TripleComponentOrder order);

	/**
	 * Sets a type of order(TripleComponentOrder)
	 *
	 * @param order
	 *            The order to set
	 */
	void setOrder(TripleComponentOrder order);

	friend class TripleListDiskIterator;
};

class TripleListDiskIterator : public IteratorTripleID {
private:
	TripleListDisk *triples;
	TripleID *nextv, pattern;
	bool hasNextv;
	unsigned int pos;

	void doFetch();

public:
	TripleListDiskIterator(TripleListDisk *triples, TripleID );
	virtual ~TripleListDiskIterator();

	bool hasNext();

	TripleID next();
};


}

#endif /* TRIPLELISTDISK_H_ */
