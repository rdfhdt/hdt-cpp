
#ifndef _CPPUTILS
#define _CPPUTILS

#include <libcdsBasics.h>
#include <cppUtils.h>

#include <vector>
using namespace std;

namespace cds_utils
{

	uint transform(const string & s);

	template <typename T> void saveValue(ostream & out, const T val) {
        //assert(out.good());
        //cout << "saveValue: " << sizeof(T) << " " << val << endl;
	if(!out.good()) {
		throw "Output not ready";
	}	
		out.write((char*)&val,sizeof(T));
	}

	template <typename T> T loadValue(istream & in) {
        //assert(in.good());
	if(!in.good()) {
		throw "Input not ready";
	}	
		T ret;
		in.read((char*)&ret,sizeof(T));
        //cout << "loadValue: " << in.tellg() << " " << sizeof(T) << " " << ret << endl;
        if((size_t)in.gcount()!=sizeof(T)) {
	    cout << "1Reading: " << sizeof(T) << " Read: " << in.gcount() << endl;
            throw "Could not read the expected bytes from input";
        }
		return ret;
	}

	template <typename T> void saveValue(ostream & out, const T * val, const size_t len) {
        //assert(out.good());
	if(!out.good()) {
		throw "Output not ready";
	}	
        //cout << "saveValueM: " << sizeof(T) << " " << len << endl;
		out.write((char*)val,len*sizeof(T));
	}

	template <typename T> T * loadValue(istream & in, const size_t len) {
        //assert(in.good());
	if(!in.good()) {
		throw "Input not ready";
	}	
		T * ret = new T[len];

        //cout << "loadValueM: " << in.tellg() << " " << sizeof(T) << " " << len << endl;
#if 0
        unsigned int counter=0;
        char *ptr = (char *)ret;
        while(counter<len && fp.good()) {
            in.read(ptr, len-counter > 1024 ? 1024 : len-counter);
            ptr += fp.gcount();
            counter += fp.gcount();
        }
        if(counter!=len*sizeof(T)) {
            throw "Could not read the expected bytes from input";
        }
#else
		in.read((char*)ret,len*sizeof(T));
        if((size_t)in.gcount()!=len*sizeof(T)) {
	    cout << "Reading: " << sizeof(T) << " Read: " << in.gcount() << endl;
            throw "Could not read the expected bytes from input";
        }
#endif
		return ret;
	}

	template <typename T> T * loadValue(const char * name, size_t & slen) {
        ifstream in(name);
		assert(in.good());
		in.seekg(0,ios_base::end);
		size_t len = in.tellg();
		slen = len/sizeof(T);
		if(len%sizeof(T)) slen--;
		in.seekg(0,ios_base::beg);
		T * ret = loadValue<T>(in,slen);
		in.close();
		return ret;
	}

	void tokenize(string str, vector<string> &tokens, char delim);

};
#endif
