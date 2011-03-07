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
	std::ofstream *outStream;
	unsigned int numTriples;

public:
	TripleListDisk();
	virtual ~TripleListDisk();

	// From Triples

	/**
	 * Returns a vector of triples matching the pattern
	 *
	 * @param triple
	 * @return
	 */
	IteratorTripleID search(TripleID &triple);

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

	bool insert(IteratorTripleID &triples);

	/**
	 * Deletes one or more triples according to a pattern
	 *
	 * @param pattern
	 *            The pattern to match against
	 * @return boolean
	 */
	bool remove(TripleID &pattern);

	bool remove(IteratorTripleID &pattern);

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
	std::ifstream stream;
	bool hasNextv;
	TripleID previousv, nextv;

	void doFetch() {
		if(stream.good()) {
			stream.read((char *)&nextv, sizeof(TripleID));

			hasNextv = !stream.fail();
		}
		hasNextv = false;
	}
public:
	TripleListDiskIterator(char *tmpfile) : stream(tmpfile) {

	}

	virtual ~TripleListDiskIterator(){
		if(stream.good())
			stream.close();
	}

	bool hasNext() {
		return hasNextv;
	}

	TripleID next() {
		previousv = nextv;
		doFetch();
		return previousv;
	}
};


}

#endif /* TRIPLELISTDISK_H_ */
