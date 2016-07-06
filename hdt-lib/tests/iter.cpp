/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTManager.hpp>

#include <getopt.h>
#include <string>
#include <iostream>
#include <fstream>

#include "../src/util/StopWatch.hpp"

#include "../src/triples/TripleIterators.hpp"

using namespace hdt;
using namespace std;


void help() {
	cout << "$ hdtSearch [options] <hdtfile> " << endl;
	cout << "\t-h\t\t\tThis help" << endl;
	cout << "\t-q\t<query>\t\tLaunch query and exit." << endl;
	cout << "\t-o\t<output>\tSave query output to file." << endl;
	cout << "\t-m\t\t\tDo not show results, just measure query time." << endl;

	//cout << "\t-v\tVerbose output" << endl;
}


int main(int argc, char **argv) {
	int c;
	string query, inputFile, outputFile;
	bool measure = false;

	while( (c = getopt(argc,argv,"hq:o:m"))!=-1) {
		switch(c) {
		case 'h':
			help();
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
			help();
			return 1;
		}
	}

	if(argc-optind<1) {
		cout << "ERROR: You must supply an HDT File" << endl << endl;
		help();
		return 1;
	}

	inputFile = argv[optind];


	try {
		HDT *hdt = HDTManager::mapHDT(inputFile.c_str());

		TripleID pat(0,1,0);
#if 0
		IteratorTripleID *it = hdt->getTriples()->search(pat);

		StopWatch st;
		unsigned int numTriples = 0;
		while(it->hasNext() && numTriples < 4) {
			TripleID *ts = it->next();
			cout << *ts << endl;

			numTriples++;
		}

		cout << "------" << endl;

		while(it->hasPrevious()) {
			TripleID *ts = it->previous();
			cout << *ts << endl;
		}

		cout << "------" << endl;

		numTriples=0;
		while(it->hasNext() && numTriples<10) {
			TripleID *ts = it->next();
			cout << *ts << endl;
			numTriples++;
		}

		delete it;
		cout << numTriples << " results in " << st << endl;

#else
		IteratorTripleID *it = hdt->getTriples()->search(pat);

#if 1
		while(it->hasNext()) {
			cout << *it->next() << endl;
		}
		it->goToStart();
		cout << "------" << endl;
#endif

		RandomAccessIterator rit(it);

		int i;
		for(i=0;i<rit.getNumElements() && i < 20;i++) {
			TripleID *tid = rit.get(i);
			cout << i << " => " << *tid << endl;
		}

		cout << "------" << endl;

		for(i-- ; i>=0; i--) {
			TripleID *tid = rit.get(i);
			cout << i << " => " << *tid << endl;
		}

		cout << "------" << endl;

		for(i=0;i<rit.getNumElements() && i<20;i++) {
			TripleID *tid = rit.get(i);
			cout << i << " => " << *tid << endl;
		}

		cout << "------" << endl;

		cout << 2 << " => " << *rit.get(2) << endl;
		cout << 7 << " => " << *rit.get(7) << endl;
		cout << 1 << " => " << *rit.get(1) << endl;
		cout << 5 << " => " << *rit.get(5) << endl;
		cout << 0 << " => " << *rit.get(0) << endl;
		cout << 9 << " => " << *rit.get(9) << endl;
		cout << 8 << " => " << *rit.get(8) << endl;

		delete it;

#endif
		delete hdt;
	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
	}
}
