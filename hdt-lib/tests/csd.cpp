/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTFactory.hpp>
#include <Header.hpp>
#include <Dictionary.hpp>
#include <Triples.hpp>

#include <getopt.h>
#include <string>
#include <iostream>
#include <fstream>

#include "../src/libdcs/CSD_PFC.h"

#include "../src/util/StopWatch.hpp"

using namespace hdt;
using namespace csd;
using namespace std;

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
	int c;
	string query, inputFile, outputFile;
	bool measure = false;

	while( (c = getopt(argc,argv,"hq:o:m"))!=-1) {
		switch(c) {
		case 'h':
			break;
		case 'q':
			query = optarg;
			break;
		case 'o':
			outputFile = optarg;
			break;
		case 'm':
			measure = true;
			break;
		default:
			cout << "ERROR: Unknown option" << endl;
			return 1;
		}
	}

	if(argc-optind<1) {
		cout << "ERROR: You must supply an input and HDT File" << endl << endl;
		return 1;
	}

	inputFile = argv[optind];

	try {

		ConvertProgress progress;
		// CONVERT
		FileteratorUCharString iterator(inputFile);

		CSD_PFC *csd = new CSD_PFC(&iterator, 32, &progress);

		ofstream out(outputFile.c_str(), ios::binary);
		csd->save(out);
		out.close();

		delete csd;

	} catch (char *e) {
		cout << "ERROR: " << e << endl;
	} catch (const char *e) {
		cout << "ERROR: " << e << endl;
	}
}



