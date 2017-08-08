/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */
#if HAVE_CDS

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
	cout << "$ filterSearch [options] <hdtfile> " << endl;
	cout << "\t-h\t\t\tThis help" << endl;
	cout << "\t-q\t<query>\t\tLaunch query and exit." << endl;
	cout << "\t-o\t<output>\tSave query output to file." << endl;
	cout << "\t-m\t\t\tDo not show results, just measure query time." << endl;

	//cout << "\t-v\tVerbose output" << endl;
}

int main(int argc, char **argv) {
	int c;
	string query, inputFile, outputFile, filter1, filter2;
	bool measure = false;

	while ((c = getopt(argc, argv, "hq:o:mf:F:")) != -1) {
		switch (c) {
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
		case 'f':
			filter1 = optarg;
			break;
		case 'F':
			filter2 = optarg;
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

		if (filter1 != "" || filter2 != "") {
			ostream *out;
			ofstream outF;

			if (outputFile != "") {
				outF.open(outputFile.c_str());
				out = &outF;
			} else {
				out = &cout;
			}

			string infile;
			if (filter1 != "")
				infile = filter1;
			else
				infile = filter2;

			std::ifstream file(infile.c_str());
			if (!file.good())
				throw std::runtime_error("unable to open filter file");

			string linea = "";
			string property = "";
			string value = "";

			StopWatch st_total;

			size_t totalQueryResults = 0;

			size_t numQuery=0;

			while (!file.eof()) {
				getline(file, linea);
				if(linea.length()==0)
					continue;
				size_t pos = linea.find(';');

				if (pos != std::string::npos) {
					property = linea.substr(0, pos);
					value = linea.substr(pos + 1);

					cerr<<"Query "<<numQuery << ": "<<linea<<endl;
					cout<<">>> Query "<<numQuery << ": "<<linea<<endl;
					cout<<"property:"<<property<<endl;
					cout<<"value:"<<value<<endl<<endl;

					StopWatch st;

					hdt::LiteralDictionary *dict = dynamic_cast<hdt::LiteralDictionary *>(hdt->getDictionary());
					if(dict==NULL) {
						cerr << "This dictionary does not support substring search" << endl;
						break;
					}
					hdt::Triples *triples = hdt->getTriples();

					uint32_t *results = NULL;
					size_t numResults = dict->substringToId((unsigned char *) value.c_str(), value.length(), &results);

					TripleID pattern(0, dict->stringToId(property, PREDICATE), 0);

					for (size_t i = 0; i < numResults; i++) {

						pattern.setObject(results[i]);

						const string objStr = dict->idToString(results[i], OBJECT);

						IteratorTripleID *it = triples->search(pattern);

						//iterate over the first pattern
						while (it->hasNext()) {
							TripleID *ts = it->next();

							if (filter1 != "") {
								// QUERY Q3
								cout << dict->idToString(ts->getSubject(), SUBJECT) << " " << objStr << endl;
								totalQueryResults++;

							} else {
								// QUERY Q4
								TripleID pat2(ts->getSubject(), 0, 0);
								TripleString out;
								const string subjStr = dict->idToString(ts->getSubject(), SUBJECT);

								IteratorTripleID *it2 = triples->search(pat2);
								while(it2->hasNext()) {
									TripleID *inner = it2->next();

									cout << subjStr << " " << dict->idToString(inner->getPredicate(), PREDICATE) <<" ";

									if(inner->getObject()==results[i]) {
										cout << objStr << endl;
									} else {
										cout << dict->idToString(inner->getObject(), OBJECT) << endl;
									}
									totalQueryResults++;
								}
								delete it2;
							}
						}
						delete it;
					}

					cout << ">>> Results: " << totalQueryResults << endl;
					cerr << "Query " << numQuery << " Results: " << totalQueryResults << " in " << st << endl << endl;
					numQuery++;
				}
			}
			cerr << "Total time: " << st_total << endl;

			if (outputFile != "") {
				outF.close();
			}

			file.close();

		}

		delete hdt;
	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
	}
}

#else
int main()
{
	return 0;
}

#endif
