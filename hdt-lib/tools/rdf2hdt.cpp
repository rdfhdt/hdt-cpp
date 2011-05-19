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
#include "../src/lm_access/gzstream.hpp"

#include "../src/triples/PlainTriples.hpp"

using namespace hdt;
using namespace std;

void help() {
	cout << "$ rdf2hdt [options] <input> <output> " << endl;
	cout << "\t-h\t\t\tThis help" << endl;
	cout << "\t-H\t<header>\tSave Header in separate file" << endl;
	cout << "\t-D\t<dictionary>\tSave Dictionary in separate file" << endl;
	cout << "\t-T\t<triples>\tSave Triples in separate file" << endl;
	cout << "\t-c\t<configfile>\tHDT Config options file" << endl;
	cout << "\t-o\t<options>\tHDT Additional options (option1:value1;option2:value2;...)" << endl;
	cout << "\t-v\tVerbose output" << endl;
}

int main(int argc, char **argv) {
	string inputFile;
	string outputFile;
	string headerFile;
	string dictionaryFile;
	string triplesFile;
	bool verbose=false;
	string configFile;
	string options;

	int c;
	while( (c = getopt(argc,argv,"H:D:T:c:o:v"))!=-1) {
		switch(c) {
		case 'H':
			headerFile = optarg;
			cout << "Header: " << headerFile << endl;
			break;
		case 'D':
			dictionaryFile = optarg;
			cout << "Dictionary: " << dictionaryFile << endl;
			break;
		case 'T':
			triplesFile = optarg;
			cout << "Triples: " << triplesFile << endl;
			break;
		case 'c':
			configFile = optarg;
			cout << "Configfile: " << configFile << endl;
			break;
		case 'o':
			options = optarg;
			cout << "Options: " << options << endl;
			break;
		case 'v':
			verbose = true;
			break;
		default:
			cout << "Unknown option: " << c << endl;
		}
	}

	if(argc-optind<2) {
		cout << "ERROR: You must supply an input and output" << endl << endl;
		help();
		return 1;
	}

	/*for (int i = optind; i < argc; i++) {
		cout << "Non opt: " << argv[i] << endl;
	}*/

	inputFile = argv[optind];
	outputFile = argv[optind+1];

	if(inputFile=="") {
		cout << "ERROR: You must supply an input file" << endl << endl;
		help();
		return 1;
	}

	if(outputFile=="") {
		cout << "ERROR: You must supply an input file" << endl << endl;
		help();
		return 1;
	}

	cout << "Convert " << inputFile << " to " << outputFile << endl;

	// Process
	HDTSpecification spec(configFile);
	spec.setOptions(options);

	HDT *hdt = HDTFactory::createBasicHDT(spec);

	// Read RDF
	ifstream in(inputFile.c_str());
	hdt->loadFromRDF(in, N3);
	in.close();

	ofstream out;

	// Save HDT
	out.open(outputFile.c_str());
	hdt->saveToHDT(out);
	out.close();

	// Save header
	if(headerFile!="") {
		Header &header = hdt->getHeader();
		out.open(headerFile.c_str());
		header.save(out);
		out.close();
	}

	ControlInformation controlInformation;

	// Save dictionary
	if(dictionaryFile!="") {
		Dictionary &dictionary = hdt->getDictionary();
		out.open(dictionaryFile.c_str());
		dictionary.save(out, controlInformation);
		out.close();
	}

	// Save triples
	if(triplesFile!=""){
		Triples &triples = hdt->getTriples();
		out.open(triplesFile.c_str());
		triples.save(out, controlInformation);
		out.close();
	}

	delete hdt;
}
