/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <Header.hpp>
#include <Dictionary.hpp>
#include <Triples.hpp>

#include <getopt.h>
#include <string>
#include <iostream>
#include <fstream>

#include "../src/dictionary/FourSectionDictionary.hpp"

#include "../src/util/StopWatch.hpp"

using namespace hdt;
using namespace std;

int main(int argc, char **argv) {
	int c;
	string query, inputFile, outputFile;
	bool measure = false;

	while( (c = getopt(argc,argv,"hq:o:m"))!=-1) {
		switch(c) {
		case 'h':
			break;
		case 'q':
			query = optarg;
			break;
		case 'o':
			outputFile = optarg;
			break;
		case 'm':
			measure = true;
			break;
		default:
			cout << "ERROR: Unknown option" << endl;
			return 1;
		}
	}

	if(argc-optind<1) {
		cout << "ERROR: You must supply an input and HDT File" << endl << endl;
		return 1;
	}

	inputFile = argv[optind];

	try {

		// CONVERT
		StdoutProgressListener progress;
		ControlInformation ci;
		FourSectionDictionary dict;

		ifstream in(inputFile.c_str(), ios::binary);

		ci.load(in);
		dict.load(in, ci, &progress);
	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
	}
}
