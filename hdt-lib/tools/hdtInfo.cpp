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

#include <HDTVersion.hpp>
#include <HDT.hpp>

#include "../src/hdt/HDTFactory.hpp"

#include "../src/rdf/RDFSerializerNTriples.hpp"
#include <stdexcept>
#include <getopt.h>
#include <string>
#include <iostream>
#include <fstream>

#include "../third/gzstream.h"

using namespace hdt;
using namespace std;


void help() {
	cout << "$ hdtInfo [options] <hdtfile> " << endl;
	cout << "\t-h\t\t\tThis help" << endl;
	cout << "\t-o\t<output>\tSave query output to file." << endl;
	cout << "\t-V\tPrints the HDT version number." << endl;
}


int main(int argc, char **argv) {
	int c;
	string outputFile;

	while( (c = getopt(argc,argv,"ho:V"))!=-1) {
		switch(c) {
		case 'h':
			help();
			break;
		case 'o':
			outputFile = optarg;
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

	if(argc-optind<1) {
		cerr << "ERROR: You must supply an HDT File" << endl << endl;
		help();
		return 1;
	}


	try {
#ifdef HAVE_LIBZ
		igzstream *inGz=NULL;
#endif
		ifstream *inF=NULL;
		istream *in=NULL;

		string inputFile = argv[optind];
		std::string suffix = inputFile.substr(inputFile.find_last_of(".") + 1);
		std::string pipeCommand;

		if( suffix == "gz"){
			#ifdef HAVE_LIBZ
				in = inGz = new igzstream(inputFile.c_str());
			#else
				throw std::runtime_error("Support for GZIP was not compiled in this version. Please Decompress the file before importing it.");
			#endif
		} else {
			in = inF = new ifstream(inputFile.c_str(), ios::binary);
		}

		if (!in->good())
		{
			cerr << "Error opening file " << inputFile << endl;
			throw std::runtime_error("Error opening file for reading");
		}

		ControlInformation controlInformation;

		// Load Global Control Information
		controlInformation.load(*in);

		// Load header
		controlInformation.load(*in);
		Header *header = HDTFactory::readHeader(controlInformation);
		header->load(*in, controlInformation);

		if( suffix == "gz") {
#ifdef HAVE_LIBZ
			inGz->close();
#endif
		} else {
			inF->close();
		}

		// Save
		IteratorTripleString *it = header->search("","","");

		while(it->hasNext()) {
			TripleString *ts = it->next();
			cout << *ts << " ."<< endl;
		}

		if(outputFile!="") {
			ofstream out(outputFile.c_str());
			if(!out.good()){
				throw std::runtime_error("Could not open output file.");
			}
			RDFSerializerNTriples serializer(out, NTRIPLES);
			serializer.serialize(it);
			out.close();
		} else {
			RDFSerializerNTriples serializer(cout, NTRIPLES);
			serializer.serialize(it);
		}
		delete it;

		delete header;

	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
		return 1;
	}
}
