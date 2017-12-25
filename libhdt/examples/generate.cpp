#include <HDTManager.hpp>

using namespace hdt;

int main(int argc, char *argv[]) {

	HDTSpecification spec;
	
	// Read RDF into an HDT file.
 	HDT *hdt = HDTManager::generateHDT("data/test.nt", "http://example.org/test", NTRIPLES, spec);

	// Add additional domain-specific properties to the header
	Header *header = hdt->getHeader();
	header->insert("myResource1", "property", "value");

	// Save HDT to a file
	hdt->saveToHDT("data/test.hdt");

	delete hdt;
}
