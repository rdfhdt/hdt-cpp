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

#include <HDTVersion.hpp>
#include <HDT.hpp>
#include <HDTManager.hpp>
#include <stdexcept>
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
	cout << "\t-i\t\t\tAlso generate index to solve all triple patterns." << endl;
	cout << "\t-c\t<configfile>\tHDT Config options file" << endl;
	cout << "\t-o\t<options>\tHDT Additional options (option1=value1;option2=value2;...)" << endl;
	cout << "\t-f\t<format>\tFormat of the RDF input (ntriples, nquad, n3, turtle, rdfxml)" << endl;
	cout << "\t-B\t\"<base URI>\"\tBase URI of the dataset." << endl;
	cout << "\t-V\tPrints the HDT version number." << endl;
	cout << "\t-p\tPrints a progress indicator." << endl;
	cout << "\t-v\tVerbose output" << endl;
}

int main(int argc, char **argv) {
	string inputFile;
	string outputFile;
	bool verbose=false;
	bool showProgress=false;
	bool generateIndex=false;
	string configFile;
	string options;
	string rdfFormat;
	string baseUri;

	RDFNotation notation = NTRIPLES;

	int c;
	while( (c = getopt(argc,argv,"c:o:vpf:B:iV"))!=-1) {
		switch(c) {
		case 'c':
			configFile = optarg;
			break;
		case 'o':
			options = optarg;
			break;
		case 'v':
			verbose = true;
			break;
		case 'p':
			showProgress = true;
			break;
		case 'f':
			rdfFormat = optarg;
			break;
		case 'B':
			baseUri = optarg;
			break;
		case 'i':
			generateIndex=true;
			break;
		case 'V':
			cout << HDTVersion::get_version_string(".") << endl;
			return 0;
		default:
			cerr << "ERROR: Unknown option" << endl;
			help();
			return 1;
		}
	}

#define vout if (!verbose) {} else std::cerr /* Verbose output */

	if (!configFile.empty()) {
		vout << "Configfile: " << configFile << endl;
	}
	if (!options.empty()) {
		vout << "Options: " << options << endl;
	}

	if(argc-optind<2) {
		cerr << "ERROR: You must supply an input and output" << endl << endl;
		help();
		return 1;
	}

	inputFile = argv[optind];
	outputFile = argv[optind+1];

	if(inputFile=="") {
		cerr << "ERROR: You must supply an RDF input file" << endl << endl;
		help();
		return 1;
	}

	if(outputFile=="") {
		cerr << "ERROR: You must supply an HDT output file" << endl << endl;
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
			cerr << "ERROR: The RDF input format must be one of: (ntriples, nquad, n3, turtle, rdfxml)" << endl;
			help();
			return 1;
		}
		vout << "RDF format: " << rdfFormat << endl;
	}

	// Process
	HDTSpecification spec(configFile);

	spec.setOptions(options);

	try {
		// Read RDF
		StopWatch globalTimer;

		ProgressListener* progress = showProgress ? new StdoutProgressListener() : NULL;
		HDT *hdt = HDTManager::generateHDT(inputFile.c_str(), baseUri.c_str(), notation, spec, progress);

		ofstream out;

		// Save HDT
		hdt->saveToHDT(outputFile.c_str(), progress);

		globalTimer.stop();
		vout << "HDT Successfully generated." << endl;
		vout << "Total processing time: ";
		vout << "Clock(" << globalTimer.getRealStr();
		vout << ")  User(" << globalTimer.getUserStr();
		vout << ")  System(" << globalTimer.getSystemStr() << ")" << endl;

		if(generateIndex) {
			hdt = HDTManager::indexedHDT(hdt, progress);
		}

		delete hdt;
		delete progress;
	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
		return 1;
	}

}
