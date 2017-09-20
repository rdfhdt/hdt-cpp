/*
 * Tutorial01.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */
#if HAVE_CDS

#include <vector>
#include <WaveletTreeNoptrs.h>

using namespace std;

int main(int argc, char **argv) {
	//unsigned int arr[] = { 1,2,4,5,1,2,3,1,4,3,2,1 };
	unsigned int arr[] = { 3,4,1,2,1 };
	unsigned int numel = sizeof(arr)/sizeof(unsigned int);

	vector<unsigned int> vector;

	for(unsigned int i=0;i<numel;i++){
		vector.push_back(arr[i]);
	}

	cds_static::BitSequenceBuilder *builder = new cds_static::BitSequenceBuilderRG(20);
	//cds_static::BitSequenceBuilder *builder = new cds_static::BitSequenceBuilderRRR(32);
	cds_static::Mapper *mapper = new cds_static::MapperNone();

	cds_static::Sequence *sequence = new cds_static::WaveletTreeNoptrs(&vector[0], vector.size(), builder, mapper);

	for(unsigned int i=1;i<=5;i++){
		unsigned int rank = sequence->rank(i, sequence->getLength());
		cout << "Rank item: " << i << " => " << rank << endl;
		for(unsigned int j=1;j<=rank;j++){
			cout << "\t" << sequence->select(i, j)<<endl;
		}

	}

	ofstream out("tmp");
	sequence->save(out);
	out.close();

	delete sequence;

	cout << "------" << endl;

	ifstream in("tmp");
	cds_static::Sequence *seq2 = cds_static::Sequence::load(in);

	for(unsigned int i=1;i<=5;i++){
		unsigned int rank = sequence->rank(i, sequence->getLength());
		cout << "Rank item: " << i << " => " << rank << endl;
		for(unsigned int j=1;j<=rank;j++){
			cout << "\t" << sequence->select(i, j)<<endl;
		}
	}

	delete seq2;
}
#else
int main()
{
	return 0;
}
#endif
