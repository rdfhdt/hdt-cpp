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

#include "../src/util/StopWatch.hpp"

using namespace hdt;
using namespace std;


void help() {
	cout << "$ hdtExtract [options] <hdtfile> " << endl;
	cout << "\t-h\t\t\tThis help" << endl;
	cout << "\t-H\t<header>\tSave Header in separate file" << endl;
	cout << "\t-D\t<dictionary>\tSave Dictionary in separate file" << endl;
	cout << "\t-T\t<triples>\tSave Triples in separate file" << endl;
}


int main(int argc, char **argv) {
	int c;
	string query, inputFile, outputFile;
	string headerFile, dictionaryFile, triplesFile;
	bool measure = false;

	while( (c = getopt(argc,argv,"hH:D:T:"))!=-1) {
		switch(c) {
		case 'h':
			help();
			break;
		case 'H':
			headerFile = optarg;
			break;
		case 'D':
			dictionaryFile = optarg;
			break;
		case 'T':
			triplesFile = optarg;
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

	if(headerFile.size()==0 && dictionaryFile.size()==0 && triplesFile.size()==0) {
		cout << "ERROR: You probably want to specify at least one of -H -D -T to extract the Header/Dictionary/Triples.";
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

		ControlInformation controlInformation;
		ofstream out;

		// Save header
		if(headerFile!="") {
			Header *header = hdt->getHeader();
			out.open(headerFile.c_str());
			if(!out.good()){
				throw "Could not open Header file.";
			}
			header->save(out, controlInformation);
			out.close();
		}

		// Save dictionary
		if(dictionaryFile!="") {
			Dictionary *dictionary = hdt->getDictionary();
			out.open(dictionaryFile.c_str());
			if(!out.good()){
				throw "Could not open Dictionary file.";
			}
			dictionary->save(out, controlInformation);
			out.close();
		}

		// Save triples
		if(triplesFile!=""){
			Triples *triples = hdt->getTriples();
			out.open(triplesFile.c_str());
			if(!out.good()){
				throw "Could not open Triples file.";
			}
			triples->save(out, controlInformation);
			out.close();
		}

		delete hdt;
	} catch (char *e) {
		cout << "ERROR: " << e << endl;
	} catch (const char *e) {
		cout << "ERROR: " << e << endl;
	}
}

