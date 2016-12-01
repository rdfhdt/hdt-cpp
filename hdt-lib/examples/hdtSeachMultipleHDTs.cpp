#include <iostream>
#include <HDTManager.hpp>
#include "../src/hdt/MultipleHDT.hpp"
#include <stdio.h>
#include <stdlib.h>     /* atoi */
#include <sstream>
#include <string>
#include <getopt.h>
#include <iostream>
#include <fstream>
#include <signal.h>
#include <iomanip>
#include <dirent.h>
#include <sys/stat.h>
#include "../src/util/StopWatch.hpp"

using namespace std;
using namespace hdt;

string listFile = "", outputFile, limit;
string folder = "";
string query = "";
std::vector<std::string> files = std::vector<std::string>();
MultipleHDT* multipleHDT;
int numVersions;
bool measure, warmup;

int interruptSignal = 0;

void signalHandler(int sig) {
	interruptSignal = 1;
}

void help() {
	cout << "$ hdtSeachMultipleHDTs [options] <HDT location> " << endl;
	cout << " Map the provided HDTs and run the provided TP query" << endl;
	cout
			<< " HDT location can be a folder (all HDTs are considered) or a text file containing a list of HDTs (one per line)"
			<< endl;
	cout << "\t-h\t\t\tThis help" << endl;
	cout << "\t-o\t<output>\tSave query output to file." << endl;
	cout << "\t-l\t<number>\tlimit up to <number> of versions" << endl;
	cout << "\t-q\t<query>\t\tLaunch query and exit." << endl;
	cout << "\t-m\t\t\tDo not show results, just measure query time." << endl;
	cout << "\t-w\t\t\tWarmup enabled." << endl;
	cout << "\t-d\t\t\tDebug" << endl;
}

int getdir(std::string dir, std::vector<std::string> &files) {
	DIR *dp;
	struct dirent *dirp;
	if ((dp = opendir(dir.c_str())) == NULL) {
		std::cerr << "Error opening " << dir << std::endl;
	}

	while ((dirp = readdir(dp)) != NULL) {
		files.push_back(dir + std::string(dirp->d_name));
	}
	closedir(dp);
	return 0;
}

void warmupQueries() {
	cout << "WARMUP one by one... " << endl;
	for (int i = 0; i < numVersions; i++) {

		cout << "query:" << files[i].c_str() << endl;
		// Enumerate all triples matching a pattern ("" means any)
		IteratorTripleString *it = multipleHDT->search("", "", "",
				files[i].c_str());
		int count = 0;
		while (it->hasNext() && count < 10000) {
			it->next();
			count++;
		}
		cout << count << " results " << endl;
		delete it; // Remember to delete iterator to avoid memory leaks!
	}
	cout << "... WARMUP one by one finished!" << endl;

	cout << "WARMUP test two ... " << endl;
	if (numVersions > 1) {
		vector<const char*> severalFiles;
		severalFiles.push_back((const char*) files[0].c_str());
		severalFiles.push_back((const char*) files[1].c_str());
		int count = 0;
		IteratorTripleString *it = multipleHDT->search("", "", "",
				severalFiles);
		while (it->hasNext() && count < 10000) {
			it->next();
			count++;
		}
		cout << count << " results " << endl;
		delete it; // Remember to delete iterator to avoid memory leaks!
	}

	cout << "... WARMUP test two finished!" << endl;

	cout << "WARMUP all... " << endl;

	// Enumerate all triples matching a pattern ("" means any)
	IteratorTripleString *it = multipleHDT->search("", "", "");
	int count = 0;
	while (it->hasNext() && count < 10000) {
		it->next();
		count++;
	}

	cout << count << " results " << endl;
	delete it; // Remember to delete iterator to avoid memory leaks!

	cout << "... WARMUP all finished!" << endl;

}

void iterate(MultipleHDT *hdt, char *query, ostream &out, bool measure) {
	TripleString tripleString;
	tripleString.read(query);

	const char *subj = tripleString.getSubject().c_str();
	const char *pred = tripleString.getPredicate().c_str();
	const char *obj = tripleString.getObject().c_str();
	if (subj[0]== '?') {
		subj = "";
	}
	else if (subj[0]== '<' && subj[strlen(subj)-1]=='>') {
		string subjTemp(subj);
		subjTemp = subjTemp.substr(1,subjTemp.size()-2);
		subj = subjTemp.c_str();
	}
	if (pred[0]== '?') {
		pred = "";
	}
	else if (pred[0]== '<' && pred[strlen(pred)-1]=='>') {
		string predTemp(pred);
		predTemp = predTemp.substr(1,predTemp.size()-2);
		pred = predTemp.c_str();
	}
	if (obj[0]=='?') {
		obj = "";
	}
	else if (obj[0]== '<' && obj[strlen(obj)-1]=='>') {
			string objTemp(obj);
			objTemp = objTemp.substr(1,objTemp.size()-2);
			obj = objTemp.c_str();
		}

#if 0
	cout << "Subject: |" << subj <<"|"<< endl;
	cout << "Predicate: |" << pred <<"|"<< endl;
	cout << "Object: |" << obj << "|"<<endl;
#endif

	try {
		TripleString ts;
		IteratorTripleString *it = hdt->search(subj, pred, obj,NULL);

		StopWatch st;
		unsigned int numTriples = 0;
		while (it->hasNext() && interruptSignal == 0) {
			TripleString *ts = it->next();
			if (!measure)
				out << *ts << endl;
			numTriples++;
		}
		cerr << numTriples << " results in " << st << endl;
		delete it;

		interruptSignal = 0;	// Interrupt caught, enable again.
	} catch (std::exception& e) {
		cerr << e.what() << endl;
	}

}

int main(int argc, char *argv[]) {

	int c;
	while ((c = getopt(argc, argv, "hi:l:o:mwq:")) != -1) {
		switch (c) {
		case 'h':
			help();
			break;
		case 'm':
			measure = true;
			break;
		case 'w':
			warmup = true;
			break;
		case 'o':
			outputFile = optarg;
			break;
		case 'q':
			query = optarg;
			break;
		case 'l':
			limit = optarg;
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

	listFile = argv[optind];

	ostream *out;
	ofstream outF;

	if (outputFile != "") {
		outF.open(outputFile.c_str());
		out = &outF;
	} else {
		out = &cout;
	}

	struct stat s;
	if (stat(listFile.c_str(), &s) == 0) {
		if (s.st_mode & S_IFDIR) {
			//it's a directory
			getdir(folder, files);
		} else if (s.st_mode & S_IFREG) {
			string line;
			std::ifstream infile(listFile.c_str());
			while (getline(infile, line)) {
				//cout << "files:" << line << endl;
				files.push_back(line);
			}
		} else {
			cerr << "Please provide your input HDTs" << endl;
			help();
			return 1;
		}
	} else {
		cerr << "Please provide your input HDTs" << endl;
		help();
		return 1;
	}

	numVersions = files.size();
	if (limit.length() > 0) {
		numVersions = atoi((char*) limit.c_str());
	}

	multipleHDT = new MultipleHDT();
	multipleHDT->mapHDTs(files, NULL);

	if (warmup) {
		warmupQueries();
	}

	if (query != "") {
		// Supplied query, search and exit.
		iterate(multipleHDT, (char*) query.c_str(), *out, measure);
	} else {
		// No supplied query, show terminal.
		char line[1024 * 10];

		signal(SIGINT, &signalHandler);
		cout << "                                                 \r>> ";
		while (cin.getline(line, 1024 * 10)) {
			if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0) {
				break;
			}
			if (strlen(line) == 0 || strcmp(line, "help") == 0) {
				cout
						<< "Please type Triple Search Pattern, using '?' for wildcards. e.g "
						<< endl;
				cout << "   http://www.somewhere.com/mysubject ? ?" << endl;
				cout
						<< "Interrupt with Control+C. Type 'exit', 'quit' or Control+D to exit the shell."
						<< endl;
				cout << ">> ";
				continue;
			}

			iterate(multipleHDT, line, *out, measure);

			cout << ">> ";
		}
	}

	if (outputFile != "") {
		outF.close();
	}

	delete multipleHDT;

	/*
	 //read queries
	 cout
	 << "opening file:" << inputFile << endl;
	 std::ifstream file((char*) inputFile.c_str());
	 cout << "opened! " << endl;

	 if (!file.good())
	 throw "unable to open filter file";
	 string linea = "";

	 vector<double> times(numVersions, 0);
	 int num_queries = 0;
	 while (!file.eof()) {
	 getline(file, linea);
	 cout << "Reading line:" << linea << endl;

	 if (linea.length() == 0)
	 continue;
	 size_t pos = linea.find(' ');

	 if (pos != std::string::npos) {
	 string query = linea.substr(0, pos);
	 string subject = "", predicate = "", object = "";

	 for (int i = 0; i < numVersions; i++) {
	 StopWatch st;
	 IteratorTripleString *it = multipleHDT->search(subject.c_str(),
	 predicate.c_str(), object.c_str());
	 int numResults = 0;
	 while (it->hasNext()) {
	 TripleString *triple = it->next();
	 //cout << "Result: " << triple->getSubject() << ", " << triple->getPredicate() << ", " << triple->getObject() << endl;
	 numResults++;
	 }
	 delete it;
	 double time = st.toMillis();
	 cout << numResults << " results in " << time << " ms" << endl;
	 times[i] = times[i] + time;
	 }
	 num_queries++;
	 }
	 }
	 //compute mean of queries
	 *out << "<version>,<mean_time>" << endl;
	 for (int i = 0; i < numVersions; i++) {
	 *out << (i + 1) << "," << times[i] / num_queries << endl;
	 }

	 if (outputFile != "") {
	 outF.close();
	 }
	 */
}

