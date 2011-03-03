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
	std::string outputFileName = "data/test.n3.D";

	HDT *hdt = HDTFactory::createDefaultHDT();

	ifstream in(inputFileName.c_str());
	hdt->loadFromRDF(in);

	Dictionary &dictionary = hdt->getDictionary();

	cout << dictionary.numberOfElements() << " elements in the dictionary." << endl;

	ofstream out(outputFileName.c_str());
	dictionary.save(out);
	out.close();

	//string s("Rebecca");
	//cout << "Rebecca has id " << dictionary.stringToId(s, SUBJECT) << endl;

	delete hdt;
}



