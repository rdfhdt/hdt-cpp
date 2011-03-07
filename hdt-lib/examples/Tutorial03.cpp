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
	std::string inputFileName = "data/test.n3";
	// final String inputFileName = "data/1.owl";
	// final String inputFileName = "data/2.rdf";
	// final String inputFileName = "data/3.owl";

	std::string outputFileName = "data/triples.txt";

	HDTSpecification hdtspec;
	HDT *hdt = HDTFactory::createBasicHDT(hdtspec);

	ifstream in(inputFileName.c_str());
	hdt->loadFromRDF(in);
	in.close();

	Triples &triples = hdt->getTriples();

	ofstream out(outputFileName.c_str());
	triples.save(out);
	out.close();

/*	in.open(outputFileName.c_str());
	triples.load(in, hdt->getHeader());
	in.close();
*/

	delete hdt;
}



