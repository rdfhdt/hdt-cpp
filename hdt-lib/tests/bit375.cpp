/*
 * bit375.cpp
 *
 *  Created on: 15/08/2012
 *      Author: mck
 */

#include <fstream>

#include "../src/bitsequence/BitSequence375.h"

using namespace std;
using namespace hdt;

int main(int argc, char **argv) {
	BitSequence375 bit(1000);

	for(int i=0;i<1000;i++) {
		bit.set(i,(i%3)==0);
	}

	ofstream out("test.bin");
	bit.save(out);
	out.close();


	ifstream in("test.bin");
	BitSequence375 *bit2 = BitSequence375::load(in);
	in.close();

	size_t count=0;
	for(int i=0;i<1005;i++) {
		if(bit2->access(i)) {
			count++;
		}
		size_t rank = bit2->rank1(i);

		cout << "i= "<<i<< " rank1=" << rank << " Test="<<count <<endl;
	}

}
