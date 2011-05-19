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

#include "../src/triples/PlainTriples.hpp"

using namespace hdt;
using namespace std;

int main(int argc, char **argv) {
	//string dataset = "naplesplus";
	string dataset = "test";

	if(argc==2) {
		dataset = argv[1];
	}

	//string inputFileName = "/home/mck/rdfdata/dataset/"+dataset;
	string inputFileName = "/Users/mck/rdf/dataset/"+dataset;
	//string inputFileName = "data/test.n3";

	string headFileName = "data/"+dataset+".H";
	string dictFileName = "data/"+dataset+".D";
	string triplesFileName = "data/"+dataset+".T";
	string hdtFileName = "data/"+dataset+".hdt";
	string specFileName = "data/hdtspec.properties";

	HDTSpecification spec(specFileName);

	HDT *hdt = HDTFactory::createBasicHDT(spec);

	ifstream in(inputFileName.c_str());
//	gz::igzstream in(inputFileName.c_str());
	hdt->loadFromRDF(in, N3);
	in.close();

	ofstream out;

	// Save header
#if 0
	Header &header = hdt->getHeader();
	out.open(headFileName.c_str());
	header.save(out);
	out.close();
#endif

#if 1
	ControlInformation controlInformation;

	// Save dictionary
	Dictionary &dictionary = hdt->getDictionary();
	out.open(dictFileName.c_str());
	dictionary.save(out, controlInformation);
	out.close();

	// Save triples
	Triples &triples = hdt->getTriples();
	out.open(triplesFileName.c_str());
	triples.save(out, controlInformation);
	out.close();
#endif

	// Save HDT
	out.open(hdtFileName.c_str());
	hdt->saveToHDT(out);
	out.close();

	//hdt->saveToRDF(cout, NTRIPLE);
	delete hdt;
}



