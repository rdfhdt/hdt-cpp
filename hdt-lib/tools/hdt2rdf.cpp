/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTFactory.hpp>

#include "dataset.h"

#include <string>
#include <iostream>
#include <fstream>
#include "../src/lm_access/gzstream.hpp"

using namespace hdt;
using namespace std;

int main(int argc, char **argv) {
	string dataset = DATASET;
	//string dataset = "test";

	string headFileName = "data/"+dataset+".H";
	string dictFileName = "data/"+dataset+".D";
	string triplesFileName = "data/"+dataset+".T";
	string hdtFileName = "data/"+dataset+".hdt";
	string rdfOutFileName = "data/"+dataset+".hdt.n3";

	HDT *hdt = HDTFactory::createDefaultHDT();

	ifstream in;

	in.open(hdtFileName.c_str());
	hdt->loadFromHDT(in);

	/*in.open(dictFileName.c_str());
	Dictionary &dict = hdt->getDictionary();
	dict.load(in, hdt->getHeader());
	in.close();

	in.open(triplesFileName.c_str());
	Triples &triples = hdt->getTriples();
	triples.load(in, hdt->getHeader());
	in.close();
	*/

	ofstream out;
	out.open(rdfOutFileName.c_str());
	hdt->saveToRDF(out, N3);

	delete hdt;
}



