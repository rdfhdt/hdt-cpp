#include <string>
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
	ifstream in("data/test.n3");

	string line;
	while(getline(in,line)) {
		cout << line << endl;

	}

	in.clear();
	in.seekg(0, std::ios::beg);

	cout << in.is_open() << "  " << in.eof() << endl;

	while(getline(in,line)) {
		cout << line << endl;
	}


	in.close();
}
