/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTFactory.hpp>
#include <HDTSpecification.hpp>

#include <string>
#include <fstream>

using namespace hdt;
using namespace std;

int main(int argc, char **argv) {
	std::string inputFileName = "data/test.n3";
	// final String inputFileName = "data/1.owl";
	// final String inputFileName = "data/2.rdf";
	// final String inputFileName = "data/3.owl";

	string specFile("data/config.properties");
	std::string outFile = "data/test.hdt";

	HDTSpecification spec(specFile);
	HDT *hdt = HDTFactory::createBasicHDT(spec);

	ifstream in(inputFileName.c_str());
	hdt->loadFromRDF(in, N3);
	in.close();

	ofstream out(outFile.c_str());
	hdt->saveToHDT(out);
	out.close();

	delete hdt;
}
