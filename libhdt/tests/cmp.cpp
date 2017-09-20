#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;

#include <Iterator.hpp>
#include "../src/libdcs/CSD.h"
#include "../src/libdcs/CSD_PFC.h"
#include "../src/util/StopWatch.hpp"

#define NUM 2000000000

class MyIterator : public hdt::IteratorUCharString {
	size_t count;
	unsigned char *buffer;

public:

	MyIterator() : count(0) {
		buffer = (unsigned char*) malloc(50);
	}

	~MyIterator() {
		free(buffer);
	}

	bool hasNext() {
		return count<NUM;
	}

	unsigned char *next() {
		sprintf((char*)buffer, "AAA %015zd FINNN", count++);
		return buffer;
	}

	size_t getNumberOfElements() {
		return NUM;
	}
};

int main(int argc, char **argv) {

#if 0
	cout << "size_t: " << sizeof(size_t) << endl;
	uint64_t size = 4100;
	size *= 1000000;

	cout << "SIZE: " << size << endl;

	char *buffer = (char *)malloc(size);

	if(buffer!=NULL) {
		for(uint64_t i=0;i<size;i+=1024) {
			buffer[i] = (char) i & 0xFF;
		}

		sleep(10);

		free(buffer);
	}
#endif


	hdt::IteratorUCharString *it=new MyIterator;

#if 0
	unsigned long long total = 0;
	while(it->hasNext()) {
		unsigned char *str = it->next();

//		cout << str << endl;
		total+=strlen((char *)str);
	}
	cout << "Total: " << total << endl;

#else

	hdt::StdoutProgressListener progress;
	StopWatch st;
	csd::CSD_PFC *pfc = new csd::CSD_PFC(it, 32, &progress);
	free(it);

	cout << endl << NUM << " entries added in " << st << endl;
	cout << "Total PFC size: " << pfc->getSize()/1000000 << "MB"<< endl;

	std::ofstream out("dict.bin");
	pfc->save(out);
	out.close();

	for(int i=1;i<=pfc->getLength();i++) {
		cout << i << " = " << pfc->extract(i) << endl;
	}

	free(pfc);
#endif
}
