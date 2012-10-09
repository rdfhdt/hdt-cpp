/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTFactory.hpp>
#include <Header.hpp>
#include <Dictionary.hpp>
#include <Triples.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <getopt.h>

#include "../src/dictionary/LiteralDictionary.hpp"

#include "../src/util/StopWatch.hpp"

using namespace hdt;
using namespace std;

class ConvertProgress : public ProgressListener {
private:
public:
	virtual ~ConvertProgress() { }

    void notifyProgress(float level, const char *section) {
    	cout << section << ": " << level << " %";
    	cout << "\r " << section << ": " << level << " %                      \r";
		cout.flush();
	}

};

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

	HDT *hdt = HDTFactory::createDefaultHDT();

	try {
		// LOAD
		hdt->loadFromHDT(inputFile.c_str());

		// CONVERT
		Dictionary *dict = hdt->getDictionary();
		LiteralDictionary litDict;
		ConvertProgress progress;
		litDict.import(dict, &progress);

		// SAVE
		ofstream out(outputFile.c_str(), ios::binary | ios::out);
		ControlInformation ci;

		// HEADER
		ci.clear();
		ci.setHeader(true);
		hdt->getHeader()->save(out, ci, NULL);

		// NEW DICTIONARY
		ci.clear();
		ci.setDictionary(true);
		litDict.save(out, ci, NULL);

		// TRIPLES
		ci.clear();
		ci.setTriples(true);
		hdt->getTriples()->save(out, ci, NULL);

		out.close();

		delete hdt;
	} catch (char *e) {
		cout << "ERROR: " << e << endl;
	} catch (const char *e) {
		cout << "ERROR: " << e << endl;
	}
}



