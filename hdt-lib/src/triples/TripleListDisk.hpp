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

class TripleListDisk : public ModifiableTriples {
private:
	char *fileName;
	std::fstream stream;
	unsigned int numTriples;

public:
	TripleListDisk();
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
	bool save(std::ostream &output);


	void load(ModifiableTriples &input);

	/**
	 * Loads triples from a file
	 *
	 * @param input
	 * @return
	 */
	void load(std::istream &input, Header &header);


	void populateHeader(Header &header);

	void startProcessing();

	void stopProcessing();




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
	 * Updates a triple with new components
	 *
	 * @param oldTriple
	 *            The triple to be replaced
	 * @param newTriple
	 *            The triple to replace the old one
	 * @return boolean
	 */
	bool edit(TripleID &oldTriple, TripleID &newTriple);

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

};

class TripleListDiskIterator : public IteratorTripleID {
private:
	std::fstream stream;
	bool hasNextv;
	TripleID nextv, &pattern;

	void doFetch();

public:
	TripleListDiskIterator(char *tmpfile, TripleID &p);
	virtual ~TripleListDiskIterator();

	bool hasNext();

	TripleID next();
};


}

#endif /* TRIPLELISTDISK_H_ */
