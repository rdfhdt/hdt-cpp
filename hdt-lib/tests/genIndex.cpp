/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTFactory.hpp>

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

	HDT *hdt = HDTFactory::createDefaultHDT();

	try {
		ifstream in(inputFile.c_str(), ios::in | ios::binary);
		if(!in.good()){
			throw "Could not open input file.";
		}
		hdt->loadFromHDT(in);
		in.close();

		delete hdt;
	} catch (char *e) {
		cout << "ERROR: " << e << endl;
	} catch (const char *e) {
		cout << "ERROR: " << e << endl;
	}
}



