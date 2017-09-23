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
#include <sstream>
#include <string>

#include "../src/dictionary/LiteralDictionary.hpp"
#include "../src/dictionary/FourSectionDictionary.hpp"

#include "../src/util/StopWatch.hpp"
#include "../src/util/fileUtil.hpp"

using namespace hdt;
using namespace std;

void help() {
	cout << "$ modifyHeader [options] <input HDT> <outputHDT> " << endl;
	cout << "\t-h\t\t\tThis help" << endl;
	cout << "\t-i\t<s p o>\t\tInsert new Triple to the Header [please escape IRIs \\< \\> and quotes \\\" \\\"]" << endl;
	cout << "\t-r\t<s p o>\t\tRemove triple from the Header [please escape IRIs \\< \\> and quotes \\\" \\\"]" << endl;
	cout
			<< "\t-I\t<insertFile>\tInsert new Triples from insertFile (one per line) to the Header"
			<< endl;
	cout
			<< "\t-R\t<removeFile>\tRemove Triples from removeFile (one per line) from the Header"
			<< endl;
}

int main(int argc, char **argv) {
	int c;
	char *inputFile=NULL, *insertFile=NULL, *removeFile=NULL, *outputFile=NULL;
	char *insertSubject=NULL, *insertPredicate=NULL, *insertObject=NULL;
	char *removeSubject=NULL, *removePredicate=NULL, *removeObject=NULL;
	bool insertSingle = false;
	bool removeSingle = false;

	bool insertMultiple = false;
	bool removeMultiple = false;

	while ((c = getopt(argc, argv, "hO:i:r:I:R:")) != -1) {
		switch (c) {
		case 'h':
			help();
			break;
		case 'O':
			outputFile = optarg;
			break;
		case 'i':
			insertSingle = true;
			insertSubject = optarg;
			insertPredicate = argv[optind++];
			insertObject = argv[optind++];
			break;
		case 'r':
			removeSingle = true;
			removeSubject = optarg;
			removePredicate = argv[optind++];
			removeObject = argv[optind++];
			break;
		case 'I':
			insertMultiple = true;
			insertFile = optarg;
			break;
		case 'R':
			removeMultiple = true;
			removeFile = optarg;
			break;
		default:
			cout << "ERROR: Unknown option" << endl;
			help();
			return 1;
		}
	}

	if (argc - optind < 2) {
		cout << "ERROR: You must supply an input and output HDT File" << endl << endl;
		help();
		return 1;
	}
	inputFile = argv[optind];
	outputFile = argv[optind+1];
	if (strcmp(inputFile,outputFile)==0){
		cerr<< "ERROR: input and output files must me different" << endl <<endl;
		return 1;
	}

	try {
		// LOAD
		HDT *hdt = HDTManager::mapHDT(inputFile);

		// Replace header
		Header *head = hdt->getHeader();

		if (insertSingle) {
			TripleString ti(insertSubject, insertPredicate, insertObject);
			head->insert(ti);
		}
		if (removeSingle) {
			TripleString ti(removeSubject, removePredicate, removeObject);
			head->remove(ti);
		}
		if (insertMultiple) {
			string line;
			std::ifstream infile(insertFile);
			while (getline(infile, line)) {
				TripleString ti;
				ti.read(line);
				head->insert(ti);
			}
		}
		if (removeMultiple) {
			string line;
			std::ifstream infile(removeFile);
			while (getline(infile, line)) {
				TripleString ti;
				ti.read(line);
				head->remove(ti);
			}
		}
		// SAVE
		hdt->saveToHDT(outputFile);

		delete hdt;
	} catch (std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
		return 1;
	}
}
