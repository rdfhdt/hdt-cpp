/*
 * streamtest.cpp
 *
 *  Created on: 28/08/2011
 *      Author: mck
 */

#include <fstream>

#include "../src/sequence/LogSequence2.hpp"
#include "../src/util/StopWatch.hpp"

using namespace hdt;
using namespace std;

int main(int argc, char **argv) {
	StopWatch st;

	int num = 100;
	
	LogSequence2 arr(bits(num), num);
	cout << "Bits: " << bits(num) << " Val: "<<num << endl;
	for(int i=0;i<num;i++) {
		arr.push_back(i);
	}

	cout << "Added in " << st << endl;

	for(int i=0;i<num;i++) {
		cout << "1: " << arr.get(i) << endl;
	}


	ofstream out("log2.bin", ios::binary);

	arr.save(out);
	out.close();
}
