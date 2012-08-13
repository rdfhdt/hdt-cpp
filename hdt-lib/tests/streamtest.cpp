/*
 * streamtest.cpp
 *
 *  Created on: 28/08/2011
 *      Author: mck
 */

#include <fstream>

#if 0
#include "../src/sequence/LogSequence.hpp"
#else
#include "../src/sequence/LogSequence2.hpp"
#endif

#include "../src/util/StopWatch.hpp"

using namespace hdt;
using namespace std;

int main(int argc, char **argv) {
	StopWatch st;

#if 0
	LogStream logStream;
#else
	LogSequence2 logStream(bits(1000));
#endif

#if 0
	std::vector<unsigned int> v;
	for(unsigned int i=0; i<10; i++) {
		v.push_back(i);
	}

	VectorIterator it(v);
	st.reset();
	logStream.add(it);

	cout << "Added in " << st << endl;

	ofstream out("tmp.bin", ios::binary);

	/*size_t tmpvar = (size_t) 0x1122334455667788ul;
	unsigned int ptr[] = {  0x55667788, 0x11223344};
	out.write((char*)&tmpvar, sizeof(size_t));
	out.write((char*)&tmpvar, sizeof(size_t));
	out.write((char*)ptr, sizeof(size_t));
	out.write((char*)ptr, sizeof(size_t));*/
	logStream.save(out);
	out.close();

#else
#if 0
	ifstream in("tmp.bin", ios::binary);
	logStream.load(in);
	in.close();
#else
	for(unsigned int i=0; i<100; i++) {
		logStream.push_back(i);
	}
	logStream.reduceBits();
#endif

#endif

	st.reset();
 	for(unsigned int i=0; i<logStream.getNumberOfElements(); i++) {
		cout << logStream.get(i) << endl;

 		unsigned int value = logStream.get(i);
 		value++;
	}

 	cout << "Browsed in " << st << endl;

}
