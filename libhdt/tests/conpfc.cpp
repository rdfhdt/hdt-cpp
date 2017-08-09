/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTManager.hpp>
#include <HDTVocabulary.hpp>
#include <Header.hpp>
#include <Dictionary.hpp>
#include <Triples.hpp>
#include <unistd.h>

#include <string>
#include <iostream>
#include <fstream>

#include "../src/dictionary/LiteralDictionary.hpp"
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
		// LOAD
		HDT *hdt = HDTManager::mapHDT(inputFile.c_str());

		// CONVERT
		Dictionary *dict = hdt->getDictionary();
		//LiteralDictionary litDict;
		FourSectionDictionary litDict;
		StdoutProgressListener progress;
		litDict.import(dict, &progress);

		// SAVE
		ofstream out(outputFile.c_str(), ios::binary | ios::out);
		ControlInformation ci;

		// GLOBAL
		ci.clear();
		ci.setType(GLOBAL);
		ci.setFormat(HDTVocabulary::HDT_CONTAINER);
		ci.save(out);

		// HEADER
		ci.clear();
		ci.setType(HEADER);
		hdt->getHeader()->save(out, ci, NULL);

		// NEW DICTIONARY
		ci.clear();
		ci.setType(DICTIONARY);
		litDict.save(out, ci, NULL);

		// TRIPLES
		ci.clear();
		ci.setType(TRIPLES);
		hdt->getTriples()->save(out, ci, NULL);

		out.close();

		delete hdt;
	} catch (std::exception& e) {
		cout << "ERROR: " << e.what() << endl;
	}
}
