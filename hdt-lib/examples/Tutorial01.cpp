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

	string dataset = "dblp";

	string inputFileName = "/home/mck/rdfdata/dataset/"+dataset;

	string headFileName = "data/"+dataset+".H";
	string dictFileName = "data/"+dataset+".D";
	string triplesFileName = "data/"+dataset+".T";
	string hdtFileName = "data/"+dataset+".hdt";

	HDT *hdt = HDTFactory::createDefaultHDT();
	ifstream in(inputFileName.c_str());
//	gz::igzstream in(inputFileName.c_str());
	hdt->loadFromRDF(in);
	in.close();

	ofstream out;

	// Save header
	Header &header = hdt->getHeader();
	out.open(headFileName.c_str());
	header.save(out);
	out.close();

	// Save dictionary
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



