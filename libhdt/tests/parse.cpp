/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <RDFParser.hpp>
#include <HDTEnums.hpp>
#include "../src/util/StopWatch.hpp"


using namespace std;
using namespace hdt;

class Processor : public RDFCallback {

	size_t count;
	StopWatch st;
public:
	Processor() :count(0) {
		st.reset();
	}

	void processTriple(const TripleString &triple, unsigned long long pos) {
		count++;
		if(count%1000000 == 0) {
			cout << (count/1000000) << " M triples parsed in " << st << endl;
			st.reset();
		}
	}
};


int main(int argc, char **argv) {
	Processor proc;
	StopWatch st;

	RDFParserCallback *parser = RDFParserCallback::getParserCallback(NTRIPLES);

	parser->doParse(argv[1], "http://dataset.com/dataset", NTRIPLES, true, &proc);

	cout << argv[1] << " parsed in " << st << endl;

	delete parser;
}
