/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTFactory.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include "../src/lm_access/gzstream.hpp"

using namespace hdt;
using namespace std;

int main(int argc, char **argv) {
	//string inputFileName = "data/test.n3";
	string inputFileName = "/Users/mck/rdf/dataset/nytimes";

	string dictFileName = "data/nytimes.D";
	string triplesFileName = "data/nytimes.T";
	string hdtFileName = "data/nytimes.hdt";

	HDT *hdt = HDTFactory::createDefaultHDT();
	ifstream in(inputFileName.c_str());
//	gz::igzstream in(inputFileName.c_str());
	hdt->loadFromRDF(in);
	in.close();

	// Save dict

	ofstream out;

	Dictionary &dictionary = hdt->getDictionary();

	out.open(dictFileName.c_str());
	dictionary.save(out);
	out.close();

	// Save triples
	Triples &triples = hdt->getTriples();

	out.open(triplesFileName.c_str());
	triples.save(out);
	out.close();

	out.open(hdtFileName.c_str());
	hdt->saveToHDT(out);
	out.close();

	//hdt->saveToRDF(cout, NTRIPLE);
	delete hdt;
}



