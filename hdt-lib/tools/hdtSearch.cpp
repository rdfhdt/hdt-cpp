/*
 * File: HDTEnums.cpp
 * Last modified: $Date: 2012-08-13 23:00:07 +0100 (lun, 13 ago 2012) $
 * Revision: $Revision: 222 $
 * Last modified by: $Author: mario.arias $
 *
 * Copyright (C) 2012, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#include <HDT.hpp>
#include <HDTManager.hpp>
#include <signal.h>

#include <getopt.h>
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

void iterate(HDT *hdt, char *query, ostream &out, bool measure) {
	TripleString tripleString;
	tripleString.read(query);

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

#if 0
	cout << "Subject: |" << subj <<"|"<< endl;
	cout << "Predicate: |" << pred <<"|"<< endl;
	cout << "Object: |" << obj << "|"<<endl;
#endif

	try {
		IteratorTripleString *it = hdt->search(subj, pred, obj);

		StopWatch st;
		unsigned int numTriples=0;
		while(it->hasNext() && interruptSignal==0) {
			TripleString *ts = it->next();
			if(!measure)
				out << *ts << endl;
			numTriples++;
		}
		cout << numTriples << " results in " << st << endl;
		delete it;

		interruptSignal=0;	// Interrupt caught, enable again.
	} catch (char *e) {
		cerr << e << endl;
	}

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
		HDT *hdt = HDTManager::mapIndexedHDT(inputFile.c_str());

		ostream *out;
		ofstream outF;

		if(outputFile!="") {
			outF.open(outputFile.c_str());
			out = &outF;
		} else {
			out = &cout;
		}

		if(query!="") {
			// Supplied query, search and exit.
			iterate(hdt, (char*)query.c_str(), *out, measure);
		} else {
			// No supplied query, show terminal.
			char line[1024*10];

			signal(SIGINT, &signalHandler);
			cout << ">> ";
			while(cin.getline(line, 1024*10)) {
				if(strcmp(line, "exit")==0|| strcmp(line,"quit")==0) {
					break;
				}
				if(strcmp(line, "help")==0) {
					cout << "Please write Triple Search Pattern, using '?' for wildcards. e.g " << endl;
					cout << "   http://www.somewhere.com/mysubject ? ?" << endl;
					cout << "If you just press 'enter', all triples will be shown, Interrupt with Control+C."<< endl;
					cout << "Type 'exit', 'quit' or Control+D to exit the shell." << endl;
					cout << ">> ";
					continue;
				}

				iterate(hdt, line, *out, measure);

				cout << ">> ";
			}
		}

		if(outputFile!="") {
			outF.close();
		}

		delete hdt;
	} catch (char *e) {
		cout << "ERROR: " << e << endl;
	} catch (const char *e) {
		cout << "ERROR: " << e << endl;
	}
}



