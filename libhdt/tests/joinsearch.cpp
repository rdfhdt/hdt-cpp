/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTManager.hpp>

#include <getopt.h>
#include <string>
#include <iostream>
#include <fstream>

#include "../src/util/StopWatch.hpp"
#include "../src/sparql/QueryProcessor.hpp"

using namespace hdt;
using namespace std;


void help() {
	cout << "$ hdtSearch [options] <hdtfile> " << endl;
	cout << "\t-h\t\t\tThis help" << endl;

	//cout << "\t-v\tVerbose output" << endl;
}

class SparqlQuery {
public:
	set<string> vars;
	vector<TripleString> patterns;
};

SparqlQuery parseFile(string fileName) {
	struct SparqlQuery output;
	string word;
	unsigned int phase = 0;
	vector<string> pattern;

	ifstream in(fileName.c_str());

	while(in >> word) {
		//cout << word << endl;
		if(word.size()==0) {
			continue;
		}
		if(phase==0) {
			if(word.at(0)=='?') {
				output.vars.insert(word);
			} else if(word=="WHERE") {
				phase++;
			}
		} else {
			if(word.at(0)=='.') {
				while(pattern.size()>3) {
					string last = pattern[pattern.size()-1];
					pattern.pop_back();
					pattern[pattern.size()-1].append(" ");
					pattern[pattern.size()-1].append(last);
				}
				if(pattern.size()!=3) {
					cout << "Pattern size: " << pattern.size() << endl;
					throw std::runtime_error("Pattern should have 3 components");
				}
				TripleString trip(pattern[0], pattern[1], pattern[2]);
				output.patterns.push_back(trip);
				pattern.clear();
			} else {
				if(word.at(0)=='{') {
					//pattern.push_back(word.substr(1));
				} else if(word.at(0)!='.'){
					pattern.push_back(word);
				}
			}
		}
	}

	in.close();
	return output;
}


int main(int argc, char **argv) {
	int c;
	string inputFile;
	string queryFile;

	while( (c = getopt(argc,argv,"hq:"))!=-1) {
		switch(c) {
		case 'h':
			help();
			break;
		case 'q':
			queryFile = optarg;
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

	if(queryFile.size()==0) {
		cout << "ERROR: You must supply a query" << endl;
		return 1;
	}

	inputFile = argv[optind];


	try {
		HDT *hdt = HDTManager::mapIndexedHDT(inputFile.c_str());

		QueryProcessor processor(hdt);

		SparqlQuery a = parseFile(queryFile);

		StopWatch st;
		VarBindingString *binding = processor.searchJoin(a.patterns, a.vars);

		unsigned int count=0;
		while(binding->findNext()) {
			for(unsigned int i=0;i<binding->getNumVars();i++) {
				cout << binding->getVarName(i) << "=" << binding->getVar(i) << endl;
			}
			cout << endl;
			count++;
		}
		cout << "Join results: " << count << endl;
		cout << "Join time: " << st << endl;

		delete binding;

		delete hdt;
	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
	}
}
