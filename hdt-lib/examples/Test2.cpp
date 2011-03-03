#include <string>
//#include <google/sparse_hash_map>
#include <ext/hash_map>
#include <string.h>
#include <iostream>

namespace std { using namespace __gnu_cxx; }

using namespace std;

struct str_cmp {
	bool operator()(const char* s1, const char* s2) const {
		int result = strcmp(s1,s2);
		std::cout << "Compare: " << s1 << " : " << s2 << " Result: " << result << std::endl;

	    return strcmp(s1,s2)==0;
	}
};

typedef std::pair<const char*, const char*> DictEntryPair;
typedef std::hash_map<const char *, const char*, std::hash<const char *>, str_cmp> DictEntryHash;
//typedef google::sparse_hash_map<const char *, const char*, std::hash<const char *>, str_cmp> DictEntryHash;
typedef DictEntryHash::iterator DictEntryIt;

int main(int argc, char **argv) {

	DictEntryHash hashSubject;

	hashSubject["HOLA"] = "buu";
	hashSubject["ADIOS"] = "eee";
	hashSubject["AsdDIOS"] = "edfee";
	hashSubject["ADdfIOS"] = "eedfe";
	hashSubject["ADIOerS"] = "asdfeee";

	DictEntryIt found = hashSubject.find("ADIOasdfS");

	int a = found==hashSubject.end();
	int b = found!=hashSubject.end();

	cout << "A: " << a << " B: " << b << endl;

	if(found!=hashSubject.end()) {
		cout << "Found: " << found->first << " => " << found->second << endl;
	} else {
		cout << "Not found" << endl;
	}

	for(DictEntryHash::const_iterator subj_it = hashSubject.begin(); subj_it!=hashSubject.end(); subj_it++) {
		cout << "Check Subj: " << subj_it->first << endl;
	}

}
