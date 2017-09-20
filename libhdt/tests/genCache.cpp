/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <vector>
#include <HDT.hpp>
#include <HDTManager.hpp>

#include <getopt.h>
#include <string>
#include <iostream>
#include <fstream>

#include "../src/triples/BitmapTriples.hpp"

#include "../src/util/StopWatch.hpp"

using namespace hdt;
using namespace std;

#define RENDER_NUM_POINTS 100000

void help() {
	cout << "$ hdtSearch [options] <hdtfile> " << endl;
	cout << "\t-h\t\t\tThis help" << endl;

	//cout << "\t-v\tVerbose output" << endl;
}

int main(int argc, char **argv) {
	int c;
	string inputFile;

	while( (c = getopt(argc,argv,"h"))!=-1) {
		switch(c) {
		case 'h':
			help();
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
		StdoutProgressListener progress;
		HDT *hdt = HDTManager::mapHDT(inputFile.c_str(), &progress);

		    hdt::Triples *t = hdt->getTriples();
		    size_t increment = t->getNumberOfElements()/RENDER_NUM_POINTS;
		    increment = increment < 1 ? 1 : increment;


		    hdt::BTInterleavedIterator it(dynamic_cast<hdt::BitmapTriples *>(t), increment);
	    vector<hdt::TripleID> triples;


		    size_t count=0;
		    while(it.hasNext()) {
			hdt::TripleID *tid = it.next();
			triples.push_back(*tid);
			//cout << *tid << endl;

			count++;
			//if((count%100)==0)
			    //cout << "Iteration: " << count << endl;

			NOTIFYCOND(&progress, "Generating Matrix", count, RENDER_NUM_POINTS);
		    }

		    // Save

		    std::ofstream out((inputFile+".hdtcache").c_str(), ios::binary);
		    unsigned int numTriples = triples.size();
		    out.write((char *)&numTriples, sizeof(unsigned int));
		    out.write((char *)&triples[0], sizeof(hdt::TripleID)*numTriples);
		    out.close();

		delete hdt;
	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
	}
}
