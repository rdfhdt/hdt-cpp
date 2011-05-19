/*
 * Test4.cpp
 *
 *  Created on: 18/05/2011
 *      Author: mck
 */

#include <Array.h>

using namespace std;

int main(int argc, char **argv) {

	cout << "size_t: " << sizeof(size_t) << " Uint: " << sizeof(unsigned int) << endl;

	vector<unsigned int> vector;

	vector.push_back(2);
	vector.push_back(3);
	vector.push_back(1);
	vector.push_back(5);
	vector.push_back(321);

	cds_utils::Array *arr = new cds_utils::Array(vector, 0);

	cout << "Inserted: " << arr->getLength() << " Max: " << arr->getMax() << endl;

	std::ofstream out("tmp");
	arr->save(out);
	out.close();

	delete arr;


	std::ifstream in("tmp");
	cds_utils::Array *arr2 = new cds_utils::Array(in);

	cout << "Size: " << arr2->getLength() << endl;


	for(unsigned int i=0;i<arr2->getLength(); i++){
		cout << "Element: " << arr2->getField(i) << endl;
	}
}

