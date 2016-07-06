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

using namespace hdt;
using namespace std;


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
		HDT *hdt = HDTManager::mapIndexedHDT(inputFile.c_str(), &progress);

		delete hdt;
	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
	}
}
