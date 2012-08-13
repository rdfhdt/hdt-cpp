/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTFactory.hpp>
#include <signal.h>

#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include "../src/util/StopWatch.hpp"

using namespace hdt;
using namespace std;

int interruptSignal = 0;

void signalHandler(int sig)
{
	interruptSignal = 1;
}

void help() {
	cout << "$ hdtSearch [options] <hdtfile> " << endl;
	cout << "\t-h\t\t\tThis help" << endl;
	cout << "\t-q\t<query>\t\tLaunch query and exit." << endl;
	cout << "\t-o\t<output>\tSave query output to file." << endl;
	cout << "\t-m\t\t\tDo not show results, just measure query time." << endl;

	//cout << "\t-v\tVerbose output" << endl;
}


int main(int argc, char **argv) {
	int c;
	string query, inputFile, outputFile;
	bool measure = false;

	while( (c = getopt(argc,argv,"hq:o:m"))!=-1) {
		switch(c) {
		case 'h':
			help();
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
			help();
			return 1;
		}
	}

	if(argc-optind<1) {
		cout << "ERROR: You must supply an HDT File" << endl << endl;
		help();
		return 1;
	}

	inputFile = argv[optind];

	HDT *hdt = HDTFactory::createDefaultHDT();

	try {
		hdt->loadFromHDT(inputFile.c_str());

		hdt->generateIndex();

		if(query!="") {
			ostream *out;
			ofstream outF;

			if(outputFile!="") {
				outF.open(outputFile.c_str());
				out = &outF;
			} else {
				out = &cout;
			}

			TripleString tripleString;
			tripleString.read(query);

			const char *subj = tripleString.getSubject().c_str();
			const char *pred = tripleString.getPredicate().c_str();
			const char *obj = tripleString.getObject().c_str();
			if(strcmp(subj, "?")==0) {
				subj="";
			}
			if(strcmp(pred, "?")==0) {
				subj="";
			}
			if(strcmp(obj, "?")==0) {
				obj="";
			}

			IteratorTripleString *it = hdt->search(subj, pred, obj);

			StopWatch st;
			unsigned int numTriples=0;
			while(it->hasNext()) {
				TripleString *ts = it->next();
				if(!measure)
					*out << *ts << endl;
				numTriples++;
			}
			if(measure)
				cout << numTriples << " results in " << st << endl;
			delete it;

			if(outputFile!="") {
				outF.close();
			}

		} else {
			TripleString tripleString;
			char line[1024*10];

			signal(SIGINT, &signalHandler);
			cout << ">> ";
			while(cin.getline(line, 1024*10)) {
				if(line[0]=='\0'||strcmp(line, "exit")==0|| strcmp(line,"quit")==0) {
					//break;
				}

				tripleString.read(line);


				const char *subj = tripleString.getSubject().c_str();
				const char *pred = tripleString.getPredicate().c_str();
				const char *obj = tripleString.getObject().c_str();
				if(strcmp(subj, "?")==0) {
					subj="";
				}
				if(strcmp(pred, "?")==0) {
					pred="";
				}
				if(strcmp(obj, "?")==0) {
					obj="";
				}

				cout << "Query: " << tripleString << endl;

				try {
					IteratorTripleString *it = hdt->search(subj, pred, obj);

					StopWatch st;
					unsigned int numTriples = 0;
					interruptSignal = 0;
					while(it->hasNext() && !interruptSignal) {
						TripleString *ts = it->next();

						if(!measure) {
							cout << *ts << endl;
						}

						numTriples++;
					}
					cout << numTriples << " results in " << st << endl;
					delete it;
				} catch (char *e) {
					cout << e << endl;
				}

				cout << ">> ";
			}
		}

		delete hdt;
	} catch (char *e) {
		cout << "ERROR: " << e << endl;
	} catch (const char *e) {
		cout << "ERROR: " << e << endl;
	}
}



