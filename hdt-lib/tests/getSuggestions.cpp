/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
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
	cout << "$ getSuggestions <HDT file> <preffixSearch> " << endl;
	cout << "\t-h\t\t\t\tThis help" << endl;
	cout << "\t-p\t<preffixSearch>\t\tPreffix search." << endl;
	cout << "\t-r\t<rol>\t\tRol (subject|predicate|object)." << endl;

	//cout << "\t-v\tVerbose output" << endl;
}

int main(int argc, char **argv) {
	int c;
	string query, inputFile, rolUser;

	while ((c = getopt(argc, argv, "hp:r:")) != -1) {
		switch (c) {
		case 'h':
			help();
			break;
		case 'p':
			query = optarg;
			break;
		case 'r':
			rolUser = optarg;
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
			Dictionary *dict = hdt->getDictionary();

			TripleComponentRole role = SUBJECT;
			if (rolUser == "object")
				role = OBJECT;
			else if (rolUser == "predicate")
				role = PREDICATE;

			/*
			 * SOLUTIONS WITH THE TRADITIONAL METHOD
			 */

			cout<< "- 10 SOLUTIONS WITH THE TRADITIONAL METHOD:"<<endl;
			vector<string> results;
			dict->getSuggestions(query.c_str(), role, results, 10);

			for (int i = 0; i < results.size(); i++) {
				cout << "Solution "<<(i+1)<<":" << results[i] << endl;
			}

			cout<< endl<<endl<<"- ALL SOLUTIONS WITH THE NEW ITERATOR METHOD:"<<endl;

			IteratorUCharString * itSol = dict->getSuggestions(query.c_str(),role);

			int i=1;
			while (itSol->hasNext()) {
				stringstream s;
				s << itSol->next();
				string solString = s.str();
				cout << "Solution "<<(i++)<<":"<< solString << ", which is ID "<<dict->stringToId(solString,role) << endl;
				//printf("Sol %d:%s\n",(i++),itSol->next());
			}


			cout<<endl<<"Now let's try the same but getting the solution as IDs"<<endl;

			IteratorUInt *itIDSol = dict->getIDSuggestions(query.c_str(),role);

			unsigned int sol=0;
			while (itIDSol->hasNext()) {
				sol =  itIDSol->next();
				cout << "Solution ID is "<<sol << ", which corresponds to string: "<<dict->idToString(sol,role)<<endl;
				//printf("Sol %d:%s\n",(i++),itSol->next());
			}

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
