/*
 * Test4.cpp
 *
 *  Created on: 18/05/2011
 *      Author: mck
 */

#include "../src/huffman/Huffman.h"

using namespace std;

int main(int argc, char **argv) {

	ifstream in;
	in.open("tmp.huffman");
	URICompressed::Huffman *huff = URICompressed::Huffman::load(in);

	unsigned int numElements;
	unsigned int encEntries;
	vector<unsigned int> plainV;

	in.read((char *)&numElements, sizeof(unsigned int));
	in.read((char *)&encEntries, sizeof(unsigned int));

	cout << "Stream contains numElements" << numElements << endl;
	cout << "Stream contains encEntries " << encEntries << endl;

	unsigned int *encV = new unsigned int[encEntries];

	in.read((char*)encV, encEntries*sizeof(unsigned int));

	size_t pos = 0;
	unsigned int read = 0;

	for(unsigned int i=0;i<numElements;i++) {
		pos = huff->decode(&read, encV, pos);
		cout << "Read pos: " << pos << " " << read << endl;
		plainV.push_back(read);
	}

	for(unsigned int i=0;i<plainV.size();i++){
		cout << i << " => " << plainV[i] << endl;
	}

	in.close();

	delete huff;
	delete encV;
}

