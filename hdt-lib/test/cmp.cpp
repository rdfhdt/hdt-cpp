#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {

	if(argc!=3){
		cout << "Args" << endl;
	}

	ifstream in1(argv[1], ios::in | ios::binary);
	ifstream in2(argv[2], ios::in | ios::binary);

	char buf1, buf2;
	size_t pos = 0;

	while(in1.good() && in2.good()) {
		in1.read(&buf1, 1);
		in2.read(&buf2, 1);

		if(buf1!=buf2) {
			cout << "Difer at: " << pos << endl;
			break;
		}

		pos++;
	}

	in1.close();
	in2.close();
}
