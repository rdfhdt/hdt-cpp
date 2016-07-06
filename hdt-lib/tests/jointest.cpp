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

#include <sstream>

using namespace hdt;
using namespace std;


class SparqlQuery {
public:
	set<string> vars;
	vector<TripleString> patterns;
};

#if 0
const string minusone="-1";
const string zero="0";

SparqlQuery parseJavi(string line) {
	SparqlQuery output;
	vector<string> pattern;

	std::stringstream ss(line);
	std::string item;
	int count=0;
	char baseChar = 'b';
	cout << "******" << endl;
	while(std::getline(ss, item, ';')) {
		//cout << item << endl;

		if(item==zero) {
			pattern.push_back(string("?").append(1, baseChar));
			baseChar++;
		} else if(item==minusone) {
			pattern.push_back("?a");
		} else {
			pattern.push_back(item);
		}

		if(pattern.size()==3) {
			TripleString trip(pattern[0], pattern[1], pattern[2]);
			output.patterns.push_back(trip);
			pattern.clear();
		}
		if(count==5) {
			break;
		}

		count++;
	}

	return output;
}
#endif

SparqlQuery parseSparql(string query) {
        SparqlQuery output;
        string word;
        unsigned int phase = 0;
        vector<string> pattern;

        stringstream in(query, ios::in);

        while(in >> word) {
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
                                        throw std::runtime_error("Pattern should have 3 components");
                                }
                                hdt::TripleString trip(pattern[0], pattern[1], pattern[2]);
                                output.patterns.push_back(trip);
                                pattern.clear();
                        } else {
				if(word.at(0)=='{') {

				} else if(word.at(0)!='.'){
					if(word.at(0)=='<') {
						pattern.push_back(word.substr(1, word.length()-2));
					} else {
				   cout << "**"<< word << "**" << endl;
				    pattern.push_back(word);
					}
                                }
                        }
                }
        }
        return output;
}


int main(int argc, char **argv) {
	int c;
	string inputFile;
	string queryFile;

	while( (c = getopt(argc,argv,"hq:"))!=-1) {
		switch(c) {
		case 'h':
			break;
		case 'q':
			queryFile = optarg;
			break;
		default:
			cout << "ERROR: Unknown option" << endl;
			return 1;
		}
	}

	if(argc-optind<1) {
		cout << "ERROR: You must supply an HDT File" << endl << endl;
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

#if 0
		{
			std::ifstream ifs( queryFile.c_str() );
			string line;

			while( getline( ifs, line ) ) {
				try {
					SparqlQuery a = parseSparql(line);

					VarBindingString *binding = hdt->searchJoin(a.patterns, a.vars);

					while(binding->findNext()) {
						for(unsigned int i=0;i<binding->getNumVars();i++) {
							binding->getVar(i);
						}
					}

					delete binding;
				} catch (const char *e) {
					cerr << "Error in query: " << line << endl;
				} catch (char *e) {
					cerr << "Error in query: " << line << endl;
				}
			}

			ifs.close();
		}
#endif

		StopWatch global;
		uint64_t numqueries=0;

		int iter=1;
		for(int i=0;i<iter;i++) {
			// Read file
			std::ifstream ifs( queryFile.c_str() );
			string line;

			StopWatch st;
			while( getline( ifs, line ) ) {
				try {
					//cerr << line << endl;
					st.reset();
					SparqlQuery a = parseSparql(line);

					VarBindingString *binding = processor.searchJoin(a.patterns, a.vars);

					uint64_t count = 0;
					while(binding->findNext()) {
						for(unsigned int i=0;i<binding->getNumVars();i++) {
							//cout << binding->getVarName(i) << "=" << binding->getVar(i) << endl;
							cout << binding->getVar(i) << " ";
						}
						cout << endl;
						count++;
					}
					#if 0
					cerr << "Join Results: " << count << endl;
					cerr << "Join time: " << st << endl;
					cerr << "Query time real: " << st.getReal() << endl;
					cerr << "Query time user: " << st.getUser() << endl << endl;
					#endif

					delete binding;
					numqueries++;
				} catch (std::exception& e) {
					cerr << "Error in query: " << line << endl;
				}
			}
			ifs.close();
		}

		cerr << "Total time: " << global << endl;
		cerr << "Average time user: " << ((double)global.getUser())/numqueries << endl;
		cerr << "Average time real: " << ((double)global.getReal())/numqueries << endl;

		delete hdt;
	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
	}
}
