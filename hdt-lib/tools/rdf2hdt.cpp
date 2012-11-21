/*
 * File: rdf2hdt.cpp
 * Last modified: $Date: 2012-08-13 23:00:07 +0100 (lun, 13 ago 2012) $
 * Revision: $Revision: 222 $
 * Last modified by: $Author: mario.arias $
 *
 * Copyright (C) 2012, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#include <HDT.hpp>
#include <HDTManager.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <getopt.h>

#include "../src/triples/PlainTriples.hpp"
#include "../src/util/StopWatch.hpp"

using namespace hdt;
using namespace std;

void help() {
	cout << "$ rdf2hdt [options] <rdf input file> <hdt output file> " << endl;
	cout << "\t-h\t\t\tThis help" << endl;
	cout << "\t-i\t\tAlso generate index to solve all triple patterns." << endl;
	cout << "\t-c\t<configfile>\tHDT Config options file" << endl;
	cout << "\t-o\t<options>\tHDT Additional options (option1:value1;option2:value2;...)" << endl;
	cout << "\t-f\t<format>\tFormat of the RDF input (ntriples, nquad, n3, turtle, rdfxml)" << endl;
	cout << "\t-B\t\"<base URI>\"\tBase URI of the dataset." << endl;
	//cout << "\t-v\tVerbose output" << endl;
}

class ConvertProgress : public ProgressListener {
private:
public:
	virtual ~ConvertProgress() { }

    void notifyProgress(float level, const char *section) {
    	cout << section << ": " << level << " %";
    	cout << "\r " << section << ": " << level << " %                      \r";
		cout.flush();
	}

};

int main(int argc, char **argv) {
	string inputFile;
	string outputFile;
	bool verbose=false;
	bool generateIndex=false;
	string configFile;
	string options;
	string rdfFormat;
	string baseUri;

	RDFNotation notation = NTRIPLES;

	int c;
	while( (c = getopt(argc,argv,"c:o:vf:B:i"))!=-1) {
		switch(c) {
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
		case 'B':
			baseUri = optarg;
			break;
		case 'i':
			generateIndex=true;
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

	if(baseUri=="") {
		baseUri="<file://"+inputFile+">";
	}

	if(rdfFormat!="") {
		if(rdfFormat=="ntriples") {
			notation = NTRIPLES;
		} else if(rdfFormat=="nquad") {
			notation = NQUAD;
		} else if(rdfFormat=="n3") {
			notation = N3;
		} else if(rdfFormat=="turtle") {
			notation = TURTLE;
		} else if(rdfFormat=="rdfxml") {
			notation = XML;
		} else {
			cout << "ERROR: The RDF input format must be one of: (ntriples, nquad, n3, turtle, rdfxml)" << endl;
			help();
			return 1;
		}
	}

	// Process
	ConvertProgress progress;
	HDTSpecification spec(configFile);

	spec.setOptions(options);

	try {
		// Read RDF
		StopWatch globalTimer;

		HDT *hdt = HDTManager::generateHDT(inputFile.c_str(), baseUri.c_str(), notation, spec, &progress);

		ofstream out;

		// Save HDT
		out.open(outputFile.c_str(), ios::out | ios::binary | ios::trunc);
		if(!out.good()){
			throw "Could not open output file.";
		}
		hdt->saveToHDT(out, &progress);
		out.close();

		globalTimer.stop();
		cout << "HDT Successfully generated.                        " << endl;
		cout << "Total processing time: ";
		cout << "Clock(" << globalTimer.getRealStr();
		cout << ")  User(" << globalTimer.getUserStr();
		cout << ")  System(" << globalTimer.getSystemStr() << ")" << endl;

		if(generateIndex) {
			hdt = HDTManager::indexedHDT(hdt, &progress);
		}

		delete hdt;
	} catch (char *exception) {
		cerr << "ERROR: " << exception << endl;
	} catch (const char *exception) {
		cerr << "ERROR: " << exception << endl;
	}

}
