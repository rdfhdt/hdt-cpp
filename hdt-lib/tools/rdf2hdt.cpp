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
	cout << "$ rdf2hdt [options] <rdf input file> <hdt output file> " << endl;
	cout << "\t-h\t\t\tThis help" << endl;
	cout << "\t-H\t<header>\tSave Header in separate file" << endl;
	cout << "\t-D\t<dictionary>\tSave Dictionary in separate file" << endl;
	cout << "\t-T\t<triples>\tSave Triples in separate file" << endl;
	cout << "\t-c\t<configfile>\tHDT Config options file" << endl;
	cout << "\t-o\t<options>\tHDT Additional options (option1:value1;option2:value2;...)" << endl;
	cout << "\t-f\t<format>\tFormat of the RDF input (N3, Turtle, RDF-XML)" << endl;
	//cout << "\t-v\tVerbose output" << endl;
}

class ConvertProgress : public ProgressListener {

public:
    void notifyProgress(float level, const char *section) {
    	cout << "Progress: " << level << " " << section << endl;
	}

};

int main(int argc, char **argv) {
	string inputFile;
	string outputFile;
	string headerFile;
	string dictionaryFile;
	string triplesFile;
	bool verbose=false;
	string configFile;
	string options;
	string rdfFormat;

	int c;
	while( (c = getopt(argc,argv,"H:D:T:c:o:vf:"))!=-1) {
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
		case 'f':
			rdfFormat = optarg;
			cout << "RDF format: " << rdfFormat << endl;
			break;
		default:
			cout << "ERROR: Unknown option" << endl;
			help();
			return 1;
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
		cout << "ERROR: You must supply an RDF input file" << endl << endl;
		help();
		return 1;
	}

	if(outputFile=="") {
		cout << "ERROR: You must supply an HDT output file" << endl << endl;
		help();
		return 1;
	}

	// Process
	ConvertProgress progress;
	HDTSpecification spec(configFile);

	spec.setOptions(options);
	HDT *hdt = HDTFactory::createHDT(spec);

	try {
		// Read RDF
		ifstream in(inputFile.c_str());
		if(!in.good()){
			throw "Could not open input file.";
		}
		hdt->loadFromRDF(in, N3, &progress);
		in.close();

		ofstream out;

		// Save HDT
		out.open(outputFile.c_str());
		if(!out.good()){
			throw "Could not open output file.";
		}
		hdt->saveToHDT(out, &progress);
		out.close();

		ControlInformation controlInformation;

		// Save header
		if(headerFile!="") {
			Header &header = hdt->getHeader();
			out.open(headerFile.c_str());
			if(!out.good()){
				throw "Could not open Header file.";
			}
			header.save(out, controlInformation);
			out.close();
		}

		// Save dictionary
		if(dictionaryFile!="") {
			Dictionary &dictionary = hdt->getDictionary();
			out.open(dictionaryFile.c_str());
			if(!out.good()){
				throw "Could not open Dictionary file.";
			}
			dictionary.save(out, controlInformation);
			out.close();
		}

		// Save triples
		if(triplesFile!=""){
			Triples &triples = hdt->getTriples();
			out.open(triplesFile.c_str());
			if(!out.good()){
				throw "Could not open Triples file.";
			}
			triples.save(out, controlInformation);
			out.close();
		}

	} catch (char *exception) {
		cerr << "ERROR: " << exception << endl;
	}

	delete hdt;
}
