/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include <HDTFactory.hpp>

#include <string>
#include <fstream>

#include "../src/header/EmptyHeader.hpp";
#include "../src/dictionary/PlainDictionary.hpp";
#include "../src/triples/TripleListDisk.hpp";
#include "../src/BasicHDT.hpp"
#include "../src/RDFSerializerN3.hpp"

using namespace hdt;
using namespace std;

int main(int argc, char **argv) {
	string dataset = "nytimes";
	string headFileName = "data/"+dataset+".H";
	string dictFileName = "data/"+dataset+".D";
	string triplesFileName = "data/"+dataset+".T";
	string hdtFileName = "data/"+dataset+".hdt";

	EmptyHeader h;

	ifstream in;

	in.open(dictFileName.c_str());
	PlainDictionary dict;
	dict.load(in, h);
	in.close();

	TripleListDisk triples(triplesFileName.c_str());

	cout << "Triples: " << triples.getNumberOfElements() << endl;

	TripleID tid(0,0,0);
	IteratorTripleID *iterID = triples.search(tid);

	RDFSerializerN3 serializer(cout);

	BasicIteratorTripleString *iterator = new BasicIteratorTripleString(&dict, iterID);

	serializer.serialize(iterator);
	serializer.endProcessing();
}



