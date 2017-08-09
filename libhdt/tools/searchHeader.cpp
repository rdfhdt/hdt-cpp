#include <getopt.h>
#include <exception>
#include <iostream>
#include <string>

#include "../include/Header.hpp"
#include "../include/Iterator.hpp"
#include "../include/SingleTriple.hpp"
#include "../src/hdt/BasicHDT.hpp"

using namespace hdt;
using namespace std;

void help() {
	cout << "$ searchHeader <input HDT> \"s p o\"" << endl;
	cout << "\t-h\t\t\tThis help" << endl;
}

int main(int argc, char **argv) {
	int c;
	char *inputFile = NULL, *query = NULL;

	while ((c = getopt(argc, argv, "h")) != -1) {
		switch (c) {
		case 'h':
			help();
			break;
		}
	}

	if (argc - optind < 2) {
		cout << "ERROR: You must supply an input HDT and a query" << endl
				<< endl;
		help();
		return 1;
	}
	inputFile = argv[optind];
	query = argv[optind + 1];

	try {
		// LOAD
		//HDT *hdt = HDTManager::mapHDT(inputFile);
		BasicHDT *hdt = new BasicHDT();
		ProgressListener *listener=NULL;
	     hdt->loadHeader(inputFile,listener);

		Header *head = hdt->getHeader();

		TripleString ti;
		ti.read(query);
		string subject = ti.getSubject();
		string predicate = ti.getPredicate();
		string object = ti.getObject();

		if (!subject.compare(0, 1, "?")) {

			subject = "";
		}
		if (!predicate.compare(0, 1, "?")) {
			predicate = "";
		}
		if (!object.compare(0, 1, "?")) {
			object = "";
		}
		IteratorTripleString *it = head->search((char*) subject.c_str(),
				(char*) predicate.c_str(),
				(char*) object.c_str());
		while (it->hasNext()) {
			TripleString* ts = it->next();
			cout << *ts<<endl;
		}

		delete hdt;
	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
		return 1;
	}
}
