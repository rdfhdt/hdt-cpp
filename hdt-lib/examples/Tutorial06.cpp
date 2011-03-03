/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include "../src/dictionary/PlainDictionary.hpp"

#include <string>
#include <fstream>
#include "../src/util/StopWatch.hpp"

using namespace hdt;
using namespace std;

int main(int argc, char **argv) {
	std::string inputFileName = "/Users/mck/rdf/results2/blogger/hdt.hdt.D";

	PlainDictionary dict;

	StopWatch st;
	ifstream in(inputFileName.c_str());
	dict.load(in);
	in.close();

	cout << "Dictionary read in " << st << endl;
}
