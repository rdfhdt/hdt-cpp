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

int interruptSignal = 0;

void signalHandler(int sig)
{
	interruptSignal = 1;
}

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

	if(argc-optind<2) {
		cout << "ERROR: You must supply an HDT File" << endl << endl;
		help();
		return 1;
	}

	inputFile = argv[optind];
	outputFile = argv[optind+1];

	try {
		HDT *hdt = HDTManager::mapHDT(inputFile.c_str());
		hdt->saveToHDT(outputFile.c_str());

		cout << "IN: " << inputFile << " Out: " << outputFile << endl;

		delete hdt;
	} catch (std::exception& e) {
		cout << "ERROR: " << e.what() << endl;
	}
}
