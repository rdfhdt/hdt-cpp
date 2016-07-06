
#include <iostream>
#include <HDTManager.hpp>

using namespace hdt;

int main(int argc, char **argv) {

	try {
		HDT *hdt = HDTManager::mapHDT(argv[1]);
		IteratorUCharString *it =hdt->getDictionary()->getObjects();

		ofstream lit("lit.txt");
		ofstream blk("blank.txt");
		ofstream uri("uri.txt");
		while(it->hasNext()) {
		    unsigned char *str = it->next();
		    if(*str=='"') {
			  // Literal
			lit << (char*)str << endl;
		    } else if(*str=='_'){
			 // Blanco
			blk << (char*)str << endl;
		    } else {
			 // URI
			uri << (char*)str << endl;
		    }
		}
		lit.close();
		blk.close();
		uri.close();

		delete it;
		delete hdt;
	 } catch(std::exception& e) {
		cerr << e.what() << endl;
	 }

}
