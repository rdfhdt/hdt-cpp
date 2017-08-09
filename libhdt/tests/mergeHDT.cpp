/*
 * File: mergeHDT.cpp
 * Last modified by: $Author: javier.fernandez $
 *
 * Copyright (C) 2016, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
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
#include "../src/hdt/BasicHDT.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <getopt.h>

#include "../src/triples/PlainTriples.hpp"
#include "../src/util/StopWatch.hpp"

using namespace hdt;
using namespace std;

void help() {
	cout << "$ mergeHDT [options] <hdt output file> [<hdt intput file>]+ "
			<< endl;
	cout << "\t-h\t\t\tThis help" << endl;
	cout << "\t-i\t\t\tAlso generate index to solve all triple patterns."
			<< endl;
	cout << "\t-c\t<configfile>\tHDT Config options file" << endl;
	cout
			<< "\t-o\t<options>\tHDT Additional options (option1=value1;option2=value2;...)"
			<< endl;
	cout << "\t-B\t\"<base URI>\"\tBase URI of the dataset." << endl;
	//cout << "\t-v\tVerbose output" << endl;
}

int main(int argc, char **argv) {
	string inputFile;
	string outputFile;
	bool verbose = false;
	bool generateIndex = false;
	string configFile;
	string options;
	string baseUri;

	int c;
	while ((c = getopt(argc, argv, "c:o:vB:i")) != -1) {
		switch (c) {
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
		case 'B':
			baseUri = optarg;
			break;
		case 'i':
			generateIndex = true;
			break;
		default:
			cout << "ERROR: Unknown option" << endl;
			help();
			return 1;
		}
	}

	if (argc - optind < 3) {
		cout << "ERROR: You must supply an output and two or more inputs"
				<< endl << endl;
		help();
		return 1;
	}

	outputFile = argv[optind];
	cout << "outputFile:" << outputFile << endl;

	if (outputFile == "") {
		cout << "ERROR: You must supply two or more HDT input files" << endl
				<< endl;
		help();
		return 1;
	}

	if (baseUri == "") {
		baseUri = "<file://" + outputFile + ">";
	}

#if 0
	cout << "Converting files: " << endl;

	const char **files = (const char **)&argv[optind];
	size_t number = argc-optind-1;

	for(size_t i=0;i<number;i++) {
		cout << "\t" << files[i] << endl;
	}

	cout << " to " << outputFile << endl;
#endif

	// Process
	StdoutProgressListener progress;
	StopWatch globalTimer;
	HDTSpecification spec(configFile);
	spec.setOptions(options);

	try {
		// Read Input Files
		BasicHDT hdt(spec);
		hdt.loadFromSeveralHDT((const char **) &argv[optind + 1],
				argc - optind - 1, baseUri, &progress);

		hdt.saveToHDT(outputFile.c_str(), &progress);


		globalTimer.stop();
		cout << "HDT Successfully generated.                                 "
				<< endl;
		cout << "Total processing time: ";
		cout << "Clock(" << globalTimer.getRealStr();
		cout << ")  User(" << globalTimer.getUserStr();
		cout << ")  System(" << globalTimer.getSystemStr() << ")" << endl;

		if (generateIndex) {
			HDTManager::indexedHDT(&hdt, &progress);
		}
	} catch (char *exception) {
		cerr << "ERROR: " << exception << endl;
	} catch (const char *exception) {
		cerr << "ERROR: " << exception << endl;
	}

}
