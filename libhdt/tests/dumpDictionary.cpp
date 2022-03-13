#include <getopt.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>

#include "../include/Dictionary.hpp"
#include "../include/HDT.hpp"
#include "../include/HDTEnums.hpp"
#include "../include/HDTManager.hpp"
#include "../include/Iterator.hpp"
#include "../include/Triples.hpp"
#include "../src/triples/TriplesList.hpp"

using namespace std;
using namespace hdt;

void help() {
	cout << "$ dumpDictionary <inputHDTFile>" << endl;
	cout
			<< "Process the input HDT file and generate <termURI>;EstimatedTriples;<rdf:label>"
			<< endl << endl;
	cout << "\t-o\t\t\tDump also objects" << endl << endl;
	cout << "\t-u\t\t\tDump only URIs" << endl << endl;
	cout << "\t-p <exportPredicateFile>\t\t\texportPredicateFile (outPred.txt by default)" << endl << endl;
	cout << "\t-t <exportTermsFile>\t\t\exportTermsFile (outTerms.txt by default)" << endl << endl;
	cout << "\t-h\t\t\tThis help" << endl << endl;
}

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

int main(int argc, char *argv[]) {
	int c;
	string inputFile;
	string outputFile;
	bool dumpObjects=false;
	bool onlyURIs=false;
	string outputPredFileString;
	string outputTermFileString;
	string label = "http://www.w3.org/2000/01/rdf-schema#label";

	while ((c = getopt(argc, argv, "houp:t:")) != -1) {
		switch (c) {
		case 'h':
			help();
			break;
		case 'o':
			dumpObjects=true;
			break;
		case 'u':
			onlyURIs=true;
			break;
		case 'p':
			outputPredFileString = optarg;
			break;
		case 't':
			outputTermFileString = optarg;
			break;
		default:
			cout << "ERROR: Unknown option" << endl;
			help();
			return 1;
		}
	}

	// if (argc < 2) {
	// 	cout << "ERROR: You must supply an input HDT File" << endl << endl;
	// 	help();
	// 	return 1;
	// }
	// inputFile = argv[optind];

	// Input is explicitly set to the revision controlled .hdt for
	// automated test execution.
	inputFile = "../data/literals.hdt";

	// Load HDT file
	HDT *hdt = HDTManager::mapIndexedHDT(inputFile.c_str());

	ostream *outP,*outT;
	ofstream outPF,outTF;

	if(outputPredFileString!="") {
		outPF.open(outputPredFileString.c_str());
		outP = &outPF;
	} else {
		outP = &cout;
	}

	if(outputTermFileString!="") {
			outTF.open(outputTermFileString.c_str());
			outT = &outTF;
		} else {
			outT = &cout;
		}


	/*
	 * Compute over the dictionary 
	 */

	for (size_t i=0;i<hdt->getDictionary()->getNpredicates();i++){
		string pred = hdt->getDictionary()->idToString(i+1, PREDICATE);

		IteratorTripleString *it1 = hdt->search("",pred.c_str(),"");
		size_t numResults = it1->estimatedNumResults();

		// check label
                IteratorTripleString *itlabel = hdt->search(pred.c_str(),label.c_str(),"");
                string rdfsLabel="";
                while (itlabel->hasNext()){
                        string label = itlabel->next()->getObject();
                        if (hasEnding(label,"@en")||hasEnding(label,"\"")){

                                std::size_t found = label.find_last_of("\"");
                                label = label.substr(1,found-1);
                                if (rdfsLabel.length()>0){
                                        rdfsLabel=rdfsLabel+" "+label;
                                }
                                else
                                        rdfsLabel=label;
                        }
                }
		delete itlabel;
		delete it1;
		
		*outP<<pred<<";"<<numResults<<";"<<rdfsLabel<<endl;



	}
	
	for (size_t i=1;i<=hdt->getDictionary()->getNsubjects();i++){

		string subj = hdt->getDictionary()->idToString(i, SUBJECT);

		IteratorTripleString *it1 = hdt->search(subj.c_str(),"","");
		size_t numResults = it1->estimatedNumResults();
		if (i<hdt->getDictionary()->getNshared()){
			IteratorTripleString* it2 = hdt->search("","",subj.c_str());
			numResults+=it2->estimatedNumResults();
		}

		// check label
		IteratorTripleString *itlabel = hdt->search(subj.c_str(),label.c_str(),"");
		string rdfsLabel="";
		while (itlabel->hasNext()){
			string label = itlabel->next()->getObject();
			if (hasEnding(label,"@en")||hasEnding(label,"\"")){

				std::size_t found = label.find_last_of("\"");
				label = label.substr(1,found-1);
				if (rdfsLabel.length()>0){
					rdfsLabel=rdfsLabel+" "+label;
				}
				else
					rdfsLabel=label;
			}
		}
		delete itlabel;
		delete it1;



		*outT<<subj<<";"<<numResults<<";"<<rdfsLabel<<endl;

	}
	if (dumpObjects){
		for (int i=hdt->getDictionary()->getNshared();i<hdt->getDictionary()->getNobjects();i++){
			string obj = hdt->getDictionary()->idToString(i+1, OBJECT);

			IteratorTripleString *it1 = hdt->search("","",obj.c_str());
			size_t numResults = it1->estimatedNumResults();
			if (!onlyURIs||obj.at(0)!='"'){
					*outT<<obj<<";"<<numResults<<endl;
			}
			delete it1;

		}
	}
	outTF.close();
	outPF.close();


	delete hdt;
}
