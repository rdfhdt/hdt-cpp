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

using namespace hdt;
using namespace std;

int main(int argc, char **argv) {
	string inputFileName = "data/test.n3";
	// final String inputFileName = "data/1.owl";
	// final String inputFileName = "data/2.rdf";
	// final String inputFileName = "data/3.owl";

	HDT *hdt = HDTFactory::createDefaultHDT();

	ifstream in(inputFileName.c_str());
	hdt->loadFromRDF(in);

	//hdt->saveToRDF(cout, NTRIPLE);

	delete hdt;
}



