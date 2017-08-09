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

#include <string>
#include <iostream>
#include <fstream>
#include <getopt.h>

#include "../src/dictionary/LiteralDictionary.hpp"
#include "../src/dictionary/FourSectionDictionary.hpp"

#include "../src/util/StopWatch.hpp"
#include "../src/util/fileUtil.hpp"


using namespace hdt;
using namespace std;


int main(int argc, char **argv) {
	char *inputFile, *headerFile, *outputFile;

	if(argc!=4) {
		cout << "ERROR: Specify the arguments:" << endl << "replaceHeader <input HDT> <outputHDT> <newHeader>" << endl << endl;
		return 1;
	}

	inputFile = argv[1];
	outputFile = argv[2];
	headerFile = argv[3	];

	try {
		// LOAD
		HDT *hdt = HDTManager::mapHDT(inputFile);

		// Replace header
		Header *head= hdt->getHeader();
		head->clear();

		ifstream in(headerFile, ios::binary);
		ControlInformation ci;
		ci.setFormat(HDTVocabulary::HEADER_NTRIPLES);
		ci.setUint("length", fileUtil::getSize(in));

		head->load(in, ci);
		in.close();

		// SAVE
		hdt->saveToHDT(outputFile);

		delete hdt;
	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
		return 1;
	}
}
