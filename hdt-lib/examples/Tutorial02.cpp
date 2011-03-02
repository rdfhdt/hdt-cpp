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
	std::string inputFileName = "data/0.rdf";
	// final String inputFileName = "data/1.owl";
	// final String inputFileName = "data/2.rdf";
	// final String inputFileName = "data/3.owl";

	HDT *hdt = HDTFactory::createDefaultHDT();

	ifstream in(inputFileName.c_str());
	hdt->loadFromRDF(in);

	Dictionary &dictionary = hdt->getDictionary();

	cout << dictionary.numberOfElements() << " elements in the dictionary." << endl;

	string s("Rebecca");
	cout << "Rebecca has id " << dictionary.stringToId(s, SUBJECT) << endl;

	delete hdt;
}



