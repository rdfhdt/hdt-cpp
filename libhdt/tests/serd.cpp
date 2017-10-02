
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RDFParser.hpp"

using namespace std;
using namespace hdt;

int main(int argc, char **argv)
{
#ifdef HAVE_SERD
	// If libhdt was not built with serd support, this will fail
	cout << "calling serd " << endl;
	RDFParserCallback::getParserCallback(TURTLE);
#endif
	return 0;
}
