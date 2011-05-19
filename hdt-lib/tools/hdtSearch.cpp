/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTFactory.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include "../src/lm_access/gzstream.hpp"

using namespace hdt;
using namespace std;

int main(int argc, char **argv) {
	//string dataset = "naplesplus";
	string dataset = "test";

	if(argc==2) {
		dataset = argv[1];
	}

	string headFileName = "data/"+dataset+".H";
	string dictFileName = "data/"+dataset+".D";
	string triplesFileName = "data/"+dataset+".T";
	string hdtFileName = "data/"+dataset+".hdt";
	string rdfOutFileName = "data/"+dataset+".hdt.n3";

	HDT *hdt = HDTFactory::createDefaultHDT();

	ifstream in;

	in.open(hdtFileName.c_str());
	hdt->loadFromHDT(in);

	/*in.open(dictFileName.c_str());
	Dictionary &dict = hdt->getDictionary();
	dict.load(in, hdt->getHeader());
	in.close();

	in.open(triplesFileName.c_str());
	Triples &triples = hdt->getTriples();
	triples.load(in, hdt->getHeader());
	in.close();
	*/

	char line[1024*10];
	TripleString tripleString;

	cout << ">> ";
	while(cin.getline(line, 1024*10)) {

		tripleString.read(line);
		cout << "Query: " << tripleString << endl;

		IteratorTripleString *it = hdt->search(tripleString.getSubject().c_str(), tripleString.getPredicate().c_str(), tripleString.getObject().c_str());

		unsigned int numTriples = 0;
		while(it->hasNext()) {
			TripleString ts = it->next();

			cout << ts << endl;
			numTriples++;
		}
		delete it;
		cout << numTriples << " results shown." << endl;

		cout << ">> ";
	}

	delete hdt;
}



