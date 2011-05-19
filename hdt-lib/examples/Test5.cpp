/*
 * Test4.cpp
 *
 *  Created on: 18/05/2011
 *      Author: mck
 */

#include "../src/huffman/Huffman.h"

using namespace std;

int main(int argc, char **argv) {

	vector<unsigned int> plainV, encV;

	plainV.push_back(3);
	plainV.push_back(1);
	plainV.push_back(5);
	plainV.push_back(5);
	plainV.push_back(6);
	plainV.push_back(7);
	plainV.push_back(8);
	plainV.push_back(9);
	plainV.push_back(10);
	plainV.push_back(11);
	plainV.push_back(12);
	plainV.push_back(13);
	plainV.push_back(14);
	plainV.push_back(15);
	plainV.push_back(2);
	plainV.push_back(2);
	plainV.push_back(3);
	plainV.push_back(1);
	plainV.push_back(1);
	plainV.push_back(1);
	plainV.push_back(2);
	plainV.push_back(3);
	plainV.push_back(1);
	plainV.push_back(3);
	plainV.push_back(2);




	URICompressed::Huffman *huff = new URICompressed::Huffman(&plainV[0], plainV.size());

	size_t maxCodeLen = huff->maxLength();
	cout << "Max code length: " << maxCodeLen << endl;

	size_t numEncElem = 1 + maxCodeLen*plainV.size()/(sizeof(unsigned int)*8);
	cout << "Numelem: " << numEncElem << endl;

	encV = plainV;

	size_t pos = 0;

	for(unsigned int i=0;i<plainV.size();i++){
		pos = huff->encode(plainV[i], &encV[0], pos );
		cout << "Posbits: " << pos << endl;
	}

	unsigned int numElements = plainV.size();

	unsigned int encEntries = 1+pos/(sizeof(unsigned int)*8);
	unsigned int bytes = 1+pos/8;

	cout << "Total bits: " << pos << endl;
	cout << "Total entries: " << encEntries << endl;
	cout << "Total bytes: " << bytes << endl;
	cout << "Total elements: " << numElements << endl;

	encV.resize(encEntries);

	std::ofstream out("tmp.huffman");
	huff->save(out);
	out.write((char*)&numElements, sizeof(unsigned int));
	out.write((char*)&encEntries, sizeof(unsigned int));
	out.write((char*)&encV[0], encEntries * sizeof(unsigned int));
	out.close();

	delete huff;
}

