
#include <fstream>

#if 1
#include "../src/rdf/RDFParserRaptor.hpp"

using namespace std;
using namespace hdt;

int main(int argc, char **argv) {


	ifstream in(argv[1]);

	RDFParserPull *parser = RDFParserPull::getParserPull(in, NTRIPLES);

	while(parser->hasNext()){
		TripleString *ts = parser->next();
		cout << "TS: " << *ts << endl;
	}
	in.close();
	delete parser;
}
#else

int main(int argc, char **argv) {

}

#endif



