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
	cout << "$ statistics <inputHDTFile>" << endl;
	cout
			<< "Process the input HDT file and generate statistic (by default, in different files in a tmp folder with a prefix inputHDTFile_Statistics)"
			<< endl << endl;
	cout
			<< "\t-o\t<OutputFile>\tGenerate statistics in different files with the prefix <OutputFile>, e.g. output/Dbpedia "
			<< endl;
	cout
			<< "\t-u\t<OutputHDT>\tGenerate statistics and create the outputHDT file including the stats in the header"
			<< endl;
	cout << "\t-h\t\t\tThis help" << endl << endl;
	cout << "\t-a\t\t\tGenerate all statistics (takes longer)" << endl << endl;
	cout
			<< "Note: The statistics follow the definitions in: Javier D. Fernández, Miguel A. Martínez-Prieto, Pablo de la Fuente Redondo, Claudio Gutiérrez. Characterizing RDF Datasets. Journal of Information Science, accepted for publication, 2016."
			<< endl;
	//cout << "\t-v\tVerbose output" << endl;
}

const string vocabSubject = "_:statistics";
const string vocabPredicate = "http://purl.org/HDT/hdt#";
int main(int argc, char *argv[]) {
	int c;
	string inputFile;
	string outputFile;
	bool createHDT = false;
	string outputFileHDT;
	bool allStats = false;

	while ((c = getopt(argc, argv, "hi:o:u:a")) != -1) {
		switch (c) {
		case 'h':
			help();
			break;
		case 'o':
			outputFile = optarg;
			break;
		case 'u':
			createHDT = true;
			outputFileHDT = optarg;
			break;
		case 'a':
			allStats = true;
			break;
		default:
			cout << "ERROR: Unknown option" << endl;
			help();
			return 1;
		}
	}

	if (argc < 2) {
		cout << "ERROR: You must supply an input HDT File" << endl << endl;
		help();
		return 1;
	}
	inputFile = argv[optind];

	cout << inputFile << endl;

	if (outputFile == "") {
		mkdir("tmp", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		outputFile = "tmp/" + inputFile;
		cout << "outputFile:" << outputFile << endl;
	}
	// Load HDT file
	HDT *hdt = HDTManager::mapHDT(inputFile.c_str());

	TriplesList* tl = new TriplesList();

	Triples * trip = hdt->getTriples();
	cout << " Number of triples: "
			<< (unsigned long) (hdt->getTriples()->getNumberOfElements())
			<< endl;
	fflush(stdout);
	IteratorTripleID *it = trip->searchAll();
	tl->insert(it);
	cout << " Number of tripleslist: " << tl->getNumberOfElements() << endl;
	delete it; // Remember to delete iterator to avoid memory leaks!

	outputFile = outputFile + "_Statistics";

	//erase summary file content
	ofstream out_summary;
	out_summary.open((outputFile + "_Summary").c_str(), ios::trunc);

	ofstream out_header_stats;
	out_header_stats.open((outputFile + "_HeaderStats").c_str(), ios::trunc);

	out_summary << "* General statistics" << endl;
	out_summary << "# Number of Triples: "
			<< hdt->getTriples()->getNumberOfElements() << endl;
	out_summary << "# Number of Predicates: "
			<< hdt->getDictionary()->getNpredicates() << endl;
	out_summary << "# Number of Subjects: "
			<< hdt->getDictionary()->getNsubjects() << endl;
	out_summary << "# Number of Objects: "
			<< hdt->getDictionary()->getNobjects() << endl;
	out_summary << "# Number of Shared Subject-Objects: "
			<< hdt->getDictionary()->getNshared() << endl;

	double ratioSO = (double) hdt->getDictionary()->getNshared()
			/ (hdt->getDictionary()->getNsubjects()
					+ hdt->getDictionary()->getNobjects()
					- hdt->getDictionary()->getNshared());
	out_summary << "# Ratio Shared Subject-Objects => SO / (S U O) : "
			<< ratioSO << endl;

	out_header_stats << vocabSubject << " <" << vocabPredicate
			<< "ratioSharedSO> " << ratioSO << endl;
	/*
	 * Compute over the dictionary to get shared subject-predicate and predicate-object
	 */
	IteratorUCharString *itPred = hdt->getDictionary()->getPredicates();

	int numSubjectPredicates = 0;
	int numPredicatesObjects = 0;
	while (itPred->hasNext()) {
		stringstream s;
		s << itPred->next();
		string pred = s.str();

		if (hdt->getDictionary()->stringToId(pred, SUBJECT) > 0) {
			//found
			numSubjectPredicates++;
		}
		if (hdt->getDictionary()->stringToId(pred, OBJECT) > 0) {
			//found
			numPredicatesObjects++;
		}
	}
	delete itPred; // Remember to delete iterator to avoid memory leaks!

	double ratioSP = (double) numSubjectPredicates
			/ (hdt->getDictionary()->getNsubjects()
					+ hdt->getDictionary()->getNpredicates()
					- numSubjectPredicates);
	out_summary << "# Ratio Shared Subject-Predicate => SP / (S U P) : "
			<< ratioSP << endl;

	out_header_stats << vocabSubject << " <" << vocabPredicate
			<< "ratioSharedSP> " << ratioSP << endl;

	double ratioPO = (double) numPredicatesObjects
			/ (hdt->getDictionary()->getNobjects()
					+ hdt->getDictionary()->getNpredicates()
					- numPredicatesObjects);
	out_summary << "# Ratio Shared Predicate-Object => PO / (P U O) : "
			<< ratioPO << endl;

	out_header_stats << vocabSubject << " <" << vocabPredicate
			<< "ratioSharedPO> " << ratioPO << endl;

	out_summary.close();
	out_header_stats.close();

	//erase summary file SO and Type content
	if (allStats) {
		ofstream out_summarySO;
		out_summarySO.open((outputFile + "_SO_Summary").c_str(), ios::trunc);
		out_summarySO.close();

		ofstream out_summaryType;
		out_summaryType.open((outputFile + "_Typed_Summary").c_str(),
				ios::trunc);
		out_summaryType.close();
	}
	//find rdf:type
	unsigned int IDrdftype = 0;
	string rdftype = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type";

	if (hdt->getDictionary()->stringToId(rdftype, PREDICATE) > 0) {
		//found
		IDrdftype = hdt->getDictionary()->stringToId(rdftype, PREDICATE);
	} else {
		rdftype = "rdf:type";
		if (hdt->getDictionary()->stringToId(rdftype, PREDICATE) > 0) {
			//found
			IDrdftype = hdt->getDictionary()->stringToId(rdftype, PREDICATE);
		} else {
			rdftype = "a";
			if (hdt->getDictionary()->stringToId(rdftype, PREDICATE) > 0) {
				//found
				IDrdftype = hdt->getDictionary()->stringToId(rdftype,
						PREDICATE);
			}
		}
	}

	tl->calculateDegrees(outputFile, hdt->getDictionary()->getNshared(),
			hdt->getDictionary()->getNpredicates(), IDrdftype, allStats);

	if (createHDT == true) {
		// Replace header
		Header *head = hdt->getHeader();
		string line;
		std::ifstream infile((outputFile + "_HeaderStats").c_str());
		while (getline(infile, line)) {
			TripleString ti;
			ti.read(line);
			head->insert(ti);
		}
		// SAVE
		hdt->saveToHDT(outputFileHDT.c_str());
	}

	delete hdt;
}
