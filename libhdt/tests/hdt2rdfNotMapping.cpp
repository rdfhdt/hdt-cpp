/*
 * File: HDTEnums.cpp
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
#include <getopt.h>
#include <iostream>
#include <fstream>

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
		} else {
			cout << "ERROR: The RDF output format must be one of: (ntriples, n3, turtle)" << endl;
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

	try {
		HDT *hdt = HDTManager::loadHDT(inputFile.c_str());
		//HDT *hdt = HDTManager::mapHDT(inputFile.c_str());

		if(outputFile!="-") {
			RDFSerializer *serializer = RDFSerializer::getSerializer(outputFile.c_str(), notation);
			hdt->saveToRDF(*serializer);
			delete serializer;
		} else {
			RDFSerializer *serializer = RDFSerializer::getSerializer(cout, notation);
			hdt->saveToRDF(*serializer);
			delete serializer;
		}
		delete hdt;
	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
	}

}
