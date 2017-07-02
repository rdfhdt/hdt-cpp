#include <iostream>
#include <HDTManager.hpp>

using namespace std;
using namespace hdt;

void help() {
	cout << "$ useHeaderStats <inputHDTFile>"
			<< endl;
	cout
			<< "Process the input HDT file and see some statistic in the header"
			<< endl<< endl;
	cout << "\t-h\t\t\tThis help" << endl<< endl;

	cout<< "Note: The statistics follow the definitions in: Javier D. Fernández, Miguel A. Martínez-Prieto, Pablo de la Fuente Redondo, Claudio Gutiérrez. Characterizing RDF Datasets. Journal of Information Science, accepted for publication, 2016."<<endl;
	//cout << "\t-v\tVerbose output" << endl;
}

void searchInHeader(int numPredicates, HDT* hdt,string metric) {

	/*
	 * Example of searching for a specific predicate
	 */
/*
	string sub="http://example.org/predicate1";
	int idPred = hdt->getDictionary()->stringToId(sub,PREDICATE);
	stringstream ss;
	ss << idPred;
	string mysubj = "_:Predicate"+ss.str();
	string pred = "<http://purl.org/HDT/hdt#PartialoutDegree_average>";
	IteratorTripleString* tsol = hdt->getHeader()->search(mysubj.c_str(),
				pred.c_str(), "");
	if (tsol->hasNext()){
		TripleString* mysol = tsol->next();
		string valueofBranchingFactor = mysol->getObject();
		cout<<"my value is:"<<valueofBranchingFactor<<endl;
	}
*/


	for (int p = 1; p <= numPredicates; p++) {
		stringstream ss;
		ss << p;
		string subj = "_:Predicate" + ss.str();
		IteratorTripleString* tsol = hdt->getHeader()->search(subj.c_str(),
				metric.c_str(), "");
		if (tsol->hasNext()) {
			TripleString* t = tsol->next();
			cout << hdt->getDictionary()->idToString(p,PREDICATE) << ": " + t->getObject() << endl;
		}
		delete tsol; // Remember to delete iterator to avoid memory leaks!
	}
}

int main(int argc, char *argv[]) {

	// Load HDT file
	if (argc<2){
		cerr<< "Please provide an input HDT"<<endl;
		help();
		return 1;
	}
	HDT *hdt = HDTManager::mapHDT(argv[1]);


	int numPredicates =  hdt->getDictionary()->getNpredicates();

	cout<<"Partial OUT degree (branching factor SP?)"<<endl;
	searchInHeader(numPredicates, hdt,"<http://purl.org/HDT/hdt#PartialoutDegree_average>");
	cout<<endl<<"Partial in degree (branching factor PO)"<<endl;
	searchInHeader(numPredicates, hdt,"<http://purl.org/HDT/hdt#PartialinDegree_average>");

	delete hdt; // Remember to delete instance when no longer needed!
}
