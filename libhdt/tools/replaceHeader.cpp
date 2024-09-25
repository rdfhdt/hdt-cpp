/*
 * File: replaceHeader.cpp
 * Last modified: $Date: 2012-11-29 15:51:00 +0100 (thu, 29 nov 2012) $
 * Revision: $Revision: 222 $
 * Last modified by: $Author: mario.arias $
 *
 * Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
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
#include <HDTVocabulary.hpp>
#include <Header.hpp>
#include <Dictionary.hpp>
#include <Triples.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <getopt.h>

#include "../src/dictionary/LiteralDictionary.hpp"
#include "../src/dictionary/FourSectionDictionary.hpp"

#include "../src/util/StopWatch.hpp"
#include "../src/util/fileUtil.hpp"


using namespace hdt;
using namespace std;


int main(int argc, char **argv) {
	char *inputFile, *headerFile, *outputFile;

	if(argc!=4) {
		cout << "ERROR: Specify the arguments:" << endl << "replaceHeader <input HDT> <outputHDT> <newHeader>" << endl << endl;
		return 1;
	}

	inputFile = argv[1];
	outputFile = argv[2];
	headerFile = argv[3	];

	try {
		// LOAD
		auto hdt = HDTManager::mapHDT(inputFile);

		// Replace header
		Header *head= hdt->getHeader();
		head->clear();

		ifstream in(headerFile, ios::binary);
		ControlInformation ci;
		ci.setFormat(HDTVocabulary::HEADER_NTRIPLES);
		ci.setUint("length", fileUtil::getSize(in));

		head->load(in, ci);
		in.close();

		// SAVE
		hdt->saveToHDT(outputFile);

	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
		return 1;
	}
}
