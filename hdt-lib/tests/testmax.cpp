
#include <iostream>

using namespace std;

inline size_t maxVal1(unsigned int numbits) {
	return ~((size_t)-1<<numbits);
}

inline size_t maxVal2(unsigned int numbits) {
	//return ~((size_t)(-1)<<numbits);
	return (size_t)-1;
}

int main(int argc, char **argv) {

	for(int i=0;i<=64;i++) {
		cout << i << " -> "<< hex << maxVal1(i) << " = " << maxVal2(i) << dec << endl;
	}
}
