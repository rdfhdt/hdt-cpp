/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTFactory.hpp>
#include <HDTSpecification.hpp>
#include <SingleTriple.hpp>

#include <string>
#include <fstream>

using namespace hdt;
using namespace std;

/**
 * @see Tutorial03b.cpp
 */
int main(int argc, char **argv) {

	// Set input file name
	std::string inputFileName = "data/test.n3";

	// Set HDT specifications
	HDTSpecification hdtspec;
	HDT *hdt = HDTFactory::createBasicHDT(hdtspec);

	// Load data from file
	ifstream in(inputFileName.c_str());
	hdt->loadFromRDF(in);
	in.close();

	// Load triples
	Triples &triples = hdt->getTriples();

	// Create pattern to match
	TripleID tid(0,0,0);

	// Retrieve iterator from triples in terms of pattern
	IteratorTripleID *it = triples.search(tid);

	// Print each triple found to standard output
	while (it->hasNext()) {
		cout << "Found a triple: " << it->next() << endl;
	}

	// Deallocate memory previously reserved for objects
	delete it;
	delete hdt;
}
