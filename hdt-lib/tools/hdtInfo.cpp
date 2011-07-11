/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTFactory.hpp>

#include "../src/rdf/RDFSerializerN3.hpp"

#include <string>
#include <iostream>
#include <fstream>

using namespace hdt;
using namespace std;


void help() {
	cout << "$ hdtInfo [options] <hdtfile> " << endl;
	cout << "\t-h\t\t\tThis help" << endl;
	cout << "\t-o\t<output>\tSave query output to file." << endl;
}


int main(int argc, char **argv) {
	int c;
	string query, inputFile, outputFile;
	bool measure = false;

	while( (c = getopt(argc,argv,"ho:"))!=-1) {
		switch(c) {
		case 'h':
			help();
			break;
		case 'o':
			outputFile = optarg;
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
		ifstream in(inputFile.c_str(), ios::in | ios::binary);
		if(!in.good()){
			throw "Could not open input file.";
		}

		// Load header
		ControlInformation controlInformation;
		controlInformation.load(in);
		Header *header = HDTFactory::readHeader(controlInformation);
		header->load(in, controlInformation);
		in.close();

		// Save
		IteratorTripleString *it = header->search("","","");

		while(it->hasNext()) {
			TripleString *ts = it->next();
			cout << *ts << endl;
		}

		if(outputFile!="") {
			ofstream out(outputFile.c_str());
			if(!out.good()){
				throw "Could not open output file.";
			}
			RDFSerializerN3 serializer(out, N3);
			serializer.serialize(it);
			out.close();
		} else {
			RDFSerializerN3 serializer(cout, N3);
			serializer.serialize(it);
		}
		delete it;

		delete header;

	} catch (char *e) {
		cout << "ERROR: " << e << endl;
	} catch (const char *e) {
		cout << "ERROR: " << e << endl;
	}
}



