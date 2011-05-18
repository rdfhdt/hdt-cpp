#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <map>

using namespace std;

typedef std::map<std::string, std::string> PropertyMap;


int main(int argc, char **argv) {
	PropertyMap map;

	istringstream strm("dictionary:plain;$asdf:http://aspdoifj;$END");

	string propertyFull;
	while(getline(strm, propertyFull, ';') ){
		if(propertyFull!="$END") {
			cout << propertyFull << endl;

			size_t pos = propertyFull.find(':');

			if(pos!=string::npos) {
				string property = propertyFull.substr(0, pos);
				string value = propertyFull.substr(pos+1);
				cout << "Property= "<< property << "\tValue= "<<value<<endl;
			}
		}
	}
}
