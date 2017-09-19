/*
 * Retrieve a random solution from a query
 *
 *  Created on: 18/09/2017
 *      Author: Javier D. Fernández
 */

#include <HDT.hpp>
#include "../src/hdt/BasicHDT.hpp"
#include "../src/dictionary/LiteralDictionary.hpp"
#include <HDTManager.hpp>
#include <signal.h>

#include <getopt.h>
#include <string>
#include <iostream>
#include <fstream>

#include "../src/util/StopWatch.hpp"

using namespace hdt;
using namespace std;

int interruptSignal = 0;

void signalHandler(int sig) {
	interruptSignal = 1;
}

void help() {
	cout << "$ randomSolution <HDT file> <subject> <predicate> <object> " << endl;
	cout << "\t-h\t\t\t\tThis help" << endl;
	cout << "\t-q\t<query>\t\tLaunch query and pick a random solution. E.g. \"? http://www.w3.org/2000/01/rdf-schema#label ?\"" << endl;
}

int main(int argc, char **argv) {
	int c;
	string query, inputFile;
	TripleString tripleString;

	while ((c = getopt(argc, argv, "hq:")) != -1) {
		switch (c) {
		case 'h':
			help();
			break;
		case 'q':
			query = optarg;
			break;
		default:
			cout << "ERROR: Unknown option" << endl;
			help();
			return 1;
		}
	}

	if (argc - optind < 1) {
		cout << "ERROR: You must supply an HDT File" << endl << endl;
		help();
		return 1;
	}

	inputFile = argv[optind];

	try {
		HDT *hdt = HDTManager::mapIndexedHDT(inputFile.c_str());

		if (query != "") {

			tripleString.read(query);

			const char *subj = tripleString.getSubject().c_str();
			const char *pred = tripleString.getPredicate().c_str();
			const char *obj = tripleString.getObject().c_str();
			if (strcmp(subj, "?") == 0) {
				subj = "";
			}
			if (strcmp(pred, "?") == 0) {
				pred = "";
			}
			if (strcmp(obj, "?") == 0) {
				obj = "";
			}

			cout<<"Query: subj->"<<subj<< " pred->"<<pred<<" "<<"obj->"<<obj<<endl;
			TripleID tid;
			hdt->getDictionary()->tripleStringtoTripleID(tripleString, tid);
			cout<<"Query: subj->"<<tid.getSubject()<< " pred->"<<tid.getPredicate()<<" "<<"obj->"<<tid.getObject()<<endl;

			IteratorTripleID *it = hdt->getTriples()->search(tid);

			size_t numResults = it->estimatedNumResults();
			srand(time(NULL));
			int randNumber = int(numResults * rand() / (RAND_MAX));
			cout << "We pick the random solution #" << randNumber << " out of " << numResults << " results"<< endl;

			it->skip(randNumber);
			TripleID* result=it->next();
			cout<<"The selected random triple ID is: "<<result->getSubject()<<" "<<result->getPredicate()<<" "<<result->getObject()<<endl;
			TripleString resString;
			hdt->getDictionary()->tripleIDtoTripleString(*result,resString);
			cout<<"The selected random triple String is: "<<resString.getSubject()<<" "<<resString.getPredicate()<<" "<<resString.getObject()<<endl;
			cout<<endl;


		} else {
			cout << "Please provide a search term" << endl;
			help();
			exit(-1);
		}

		delete hdt;
	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
	}
}
