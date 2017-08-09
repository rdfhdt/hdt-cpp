/*
 * File: hdtExtract.cpp
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

#include <getopt.h>
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

	try {
		HDT *hdt = HDTManager::mapHDT(inputFile.c_str());

		ControlInformation controlInformation;
		ofstream out;

		// Save header
		if(headerFile!="") {
			Header *header = hdt->getHeader();
			out.open(headerFile.c_str());
			if(!out.good()){
				throw std::runtime_error("Could not open Header file.");
			}
			header->save(out, controlInformation);
			out.close();
		}

		// Save dictionary
		if(dictionaryFile!="") {
			Dictionary *dictionary = hdt->getDictionary();
			out.open(dictionaryFile.c_str());
			if(!out.good()){
				throw std::runtime_error("Could not open Dictionary file.");
			}
			dictionary->save(out, controlInformation);
			out.close();
		}

		// Save triples
		if(triplesFile!=""){
			Triples *triples = hdt->getTriples();
			out.open(triplesFile.c_str());
			if(!out.good()){
				throw std::runtime_error("Could not open Triples file.");
			}
			triples->save(out, controlInformation);
			out.close();
		}

		delete hdt;
	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
	}
}
