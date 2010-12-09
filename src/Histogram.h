#include<math.h>
#include <limits>
#include <iostream>
#include <string>
using namespace std;
// A histogram class.
// The Histogram object can keep a tally of values
// within a range, the range is arranged into some
// number of bins specified during construction.
// Any allocation of a Histogram object may throw
// a bad_alloc exception.
class Histogram
{
private:
	double Start,nBins_by_interval;
	unsigned int nBins;
	unsigned int number;
	unsigned int* freq;
	double minValue, maxValue, average, deviation, total;

public:
	// Construct a histogram that can count
	// within a range of values.
	// All bins of the histogram are set to zero.
	Histogram(
			  const double& Start,
			  const double& End,
			  const unsigned int& nBins):
	Start(Start),
	nBins_by_interval(nBins/(End-Start)),
	nBins(nBins),
	freq( new unsigned int[nBins] )
	{
      	reset();
	}
	// Construct a histogram by copying another one.
	Histogram(const Histogram& other):
	Start(other.Start),
	nBins_by_interval(other.nBins_by_interval),
	nBins(other.nBins),
	freq( new unsigned int[nBins] )
	{
		for(unsigned int i(0); i < nBins; ++i)
            freq[i] = other.freq[i];
	}
	
	void reset() {
		for(unsigned int i(0); i < nBins; ++i)
            freq[i] = 0U;
      	number=total=average=deviation=0;
		minValue = std::numeric_limits<double>::max();
		maxValue = std::numeric_limits<double>::min();
		
	}
	
	// Deallocate the memory that was allocated for
	// the tallied counts.
	~Histogram() {delete[] freq;}
	// Set this histogram equal to another.
	Histogram& operator=(const Histogram& other)
	{
		if( this != &other )
		{
            Start = other.Start;
            nBins_by_interval = other.nBins_by_interval;
            if( nBins != other.nBins )
            {
				nBins = other.nBins;
				delete[] freq;
				freq = new unsigned int[nBins];
            }
            for(unsigned int i(0); i < nBins; ++i)
				freq[i] = other.freq[i];
		}
		return *this;
	}
	// Increase the count for the bin that holds a
	// value that is in range for this histogram.
	void Add(const double& x)
	{
		number++;
		total+=x;
		average+=x;
		deviation+= x * x;
		minValue = min(minValue, x);
		maxValue = max(maxValue, x);
		
		const unsigned int i( static_cast<unsigned int>( (x-Start)*nBins_by_interval) );
		
		if( i < nBins ) {
			freq[i]++;
		}
	}
	
	void end() {
		average = average / number;
		deviation = deviation/number - average * average;
		deviation = sqrt(deviation);
	}
	
	void dumpStr(ostream &outfile) {
		for(unsigned int i(0); i<nBins & i<=maxValue;i++) {
			outfile << i << "  " << freq[i] << endl;
		}
		
		outfile << "# Number: "<<number <<endl;
		outfile << "# Average: "<<average<<endl;
		outfile << "# Deviation: "<<deviation<<endl;
		outfile << "# Min: "<<minValue<<endl;
		outfile << "# Max: "<<maxValue<<endl;
		
		for(unsigned int i=maxValue<nBins?maxValue:nBins; i>=50; i--) {
			if(freq[i]>1) {
				outfile << "# Latest: "<<i<<endl;
				break;
			}
		}
	}
	
	void dump(char * name, char *suffix) {
		string fileName(name);
		fileName.append(suffix);
		ofstream outfile;
		
		cout << "Writing histogram to: " << fileName << endl;
			
		outfile.open(fileName.c_str());

		dumpStr(outfile);
		
		outfile.close();
	}
	
	// Get the sum of all counts in the histogram.
	const unsigned int GetTotalCount() const
	{
		unsigned int c(0U);
		for( unsigned int i(0); i < nBins; ++i )
            c += freq[i];
		return c;
	}
};
