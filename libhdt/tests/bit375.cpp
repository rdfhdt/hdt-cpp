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

	for(size_t i=0;i<1000;i++) {
		bool val = (i%3)==0;
		bit.set(i,val);
		if(bit.access(i)!=val) {
			cout << "Wrong insert << " << i << " / "<< val << endl;
		}
	}

#if 0
	ofstream out("test.bin");
	bit.save(out);
	out.close();


	ifstream in("test.bin");
	BitSequence375 *bit2 = BitSequence375::load(in);
	in.close();
#endif

	size_t count=0;
	for(size_t i=0;i<1005;i++) {
		if(bit.access(i)) {
			count++;
		}
		size_t rank = bit.rank1(i);

		cout << "i= "<<i<<" "<< (bit.access(i)?"*":" ") <<" rank1=" << rank << " Test="<<count <<endl;
		if(rank!=count) {
			cout << "WRONG!!" << endl;
		}
	}

}
