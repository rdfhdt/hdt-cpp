/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTFactory.hpp>

#include <string>
#include <fstream>

using namespace hdt;
using namespace std;

int main(int argc, char **argv) {
	string dictFileName = "data/nytimes.D";
	string triplesFileName = "data/nytimes.T";
	string hdtFileName = "data/nytimes.hdt";

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
	hdt->saveToRDF(cout, N3);

	delete hdt;
}



