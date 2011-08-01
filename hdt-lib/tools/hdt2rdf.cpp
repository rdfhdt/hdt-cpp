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

using namespace hdt;
using namespace std;

void help() {
	cout << "$ hdt2rdf [options] <HDT input file> <RDF output file> " << endl;
	cout << "\t-h\t\t\tThis help" << endl;
	cout << "\t-f\t<format>\tRDF Format of the output" << endl;
	//cout << "\t-v\tVerbose output" << endl;

}

int main(int argc, char **argv) {
	int c;
	string rdfFormat, inputFile, outputFile;
	RDFNotation notation = NTRIPLES;

	while( (c = getopt(argc,argv,"f:"))!=-1) {
		switch(c) {
		case 'f':
			rdfFormat = optarg;
			cout << "Format: " << rdfFormat << endl;
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

	if(rdfFormat!="") {
		if(rdfFormat=="ntriples") {
			notation = NTRIPLES;
		} else if(rdfFormat=="n3") {
			notation = N3;
		} else if(rdfFormat=="turtle") {
			notation = TURTLE;
		} else if(rdfFormat=="rdfxml") {
			notation = XML;
		} else {
			cout << "ERROR: The RDF output format must be one of: (ntriples, n3, turtle, rdfxml)" << endl;
			help();
			return 1;
		}
	}

	inputFile = argv[optind];
	outputFile = argv[optind+1];

	if(inputFile=="") {
		cout << "ERROR: You must supply an HDT input file" << endl << endl;
		help();
		return 1;
	}

	if(outputFile=="") {
		cout << "ERROR: You must supply an RDF output file" << endl << endl;
		help();
		return 1;
	}

	HDT *hdt = HDTFactory::createDefaultHDT();

	try {
		ifstream in(inputFile.c_str(), ios::in | ios::binary);
		if(!in.good()){
			throw "Could not open input file.";
		}
		hdt->loadFromHDT(in);
		in.close();

		if(outputFile!="-") {
			ofstream out(outputFile.c_str());
			if(!in.good()){
				throw "Could not write to output file.";
			}
			RDFSerializer *serializer = RDFSerializer::getSerializer(out, notation);
			hdt->saveToRDF(*serializer);
			delete serializer;
			out.close();
		} else {
			RDFSerializer *serializer = RDFSerializer::getSerializer(cout, notation);
			hdt->saveToRDF(*serializer);
			delete serializer;
		}
	} catch (char *exception) {
		cerr << "ERROR: " << exception << endl;
	} catch (const char *e) {
		cout << "ERROR: " << e << endl;
	}

	delete hdt;
}



