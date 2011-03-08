/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTFactory.hpp>

#include "../src/header/EmptyHeader.hpp"
#include "../src/triples/TripleListDisk.hpp"

#include <string>
#include <fstream>

using namespace hdt;
using namespace std;

int main(int argc, char **argv) {
	std::string inputFileName = "data/test.n3";
	// final String inputFileName = "data/1.owl";
	// final String inputFileName = "data/2.rdf";
	// final String inputFileName = "data/3.owl";

	std::string outputFileName = "data/nytimes.T";

	EmptyHeader h;
	TripleListDisk triples;

	ifstream in;
	in.open(outputFileName.c_str());
	triples.load(in, h);
	in.close();

	triples.sort(SPO);

//	TripleID toDelete(2,2,0);
//	triples.remove(toDelete);

	TripleID all(0,0,0);
	IteratorTripleID *it = triples.search(all);

	while(it->hasNext()) {
		TripleID triple = it->next();
		cout << ">>" << triple << endl;
	}
	delete it;
}



