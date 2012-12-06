#include <iostream>
#include <HDTManager.hpp>

using namespace std;
using namespace hdt;

int main(int argc, char *argv[]) {

	// Load HDT file
	HDT *hdt = HDTManager::mapHDT("data/test.hdt");

	// Enumerate all triples matching a pattern ("" means any)
	IteratorTripleString *it = hdt->search("http://example.org/uri3","","");
	while(it->hasNext()){
		TripleString *triple = it->next();
		cout << "Result: " << triple->getSubject() << ", " << triple->getPredicate() << ", " << triple->getObject() << endl;
	}
	delete it; // Remember to delete iterator to avoid memory leaks!
	
	// Enumerate all different predicates
	cout << "Dataset contains " << hdt->getDictionary()->getNpredicates() << " predicates.";
	IteratorUCharString *itPred = hdt->getDictionary()->getPredicates();
	while(itPred->hasNext()) {
		unsigned char *str = itPred->next(); // Warning this pointer is only valid until next call to next();
		cout << str << endl;
		itPred->freeStr(str);
	}
	delete itPred;  // Remember to delete iterator to avoid memory leaks!

	delete hdt; // Remember to delete instance when no longer needed!
}
