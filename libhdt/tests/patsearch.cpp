#if HAVE_CDS

#include <HDT.hpp>
#include <HDTManager.hpp>
#include <signal.h>

#include <getopt.h>
#include <string>
#include <iostream>
#include <fstream>

#include "../src/util/StopWatch.hpp"
#include "../src/dictionary/LiteralDictionary.hpp"

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


	try {
		HDT *hdt = HDTManager::mapHDT(inputFile.c_str());

		char line[1024*10];

		signal(SIGINT, &signalHandler);
		cout << ">> ";
		while(cin.getline(line, 1024*10)) {
			if(line[0]=='\0'||strcmp(line, "exit")==0|| strcmp(line,"quit")==0) {
				break;
			}

			uint32_t *results = NULL;
			hdt::LiteralDictionary *dict = dynamic_cast<hdt::LiteralDictionary *>(hdt->getDictionary());
			if(dict!=NULL) {
				size_t numResults = dict->substringToId((unsigned char *)line, strlen(line), &results);
				for(size_t i=0;i<numResults;i++) {
					cout << dict->idToString(results[i], hdt::OBJECT) << endl;
				}
			} else {
				cerr << "This dictionary does not support substring search." << endl;
				break;
			}
			cout << ">> ";
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
