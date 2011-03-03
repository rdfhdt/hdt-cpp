/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include "../src/PlainDictionary.hpp"

#include <string>
#include <fstream>

using namespace hdt;
using namespace std;

int main(int argc, char **argv) {
	std::string inputFileName = "/home/mck/rdfdata/bibsonomy/hdt.hdt.D";

	PlainDictionary dict;

	ifstream in(inputFileName.c_str());
	dict.load(in);
	in.close();
}
