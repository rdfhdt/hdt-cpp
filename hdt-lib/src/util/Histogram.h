#include <math.h>
#include <limits>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

/** A histogram class.
 * The Histogram object can keep a tally of values within a range, the range is arranged into some
 * number of bins specified during construction. Any allocation of a Histogram object may throw
 * a bad_alloc exception.
 */
class Histogram {
private:
	double Start, nBins_by_interval;
	unsigned int nBins;
	unsigned int number;
	unsigned int* freq;
	double minValue, maxValue, mean, deviation, total;
public:
	/** Constructor
	 * A histogram that can count within a range of values. All bins of the histogram are set to zero.
	 * @param Start Description of the param.
	 * @param End Description of the param.
	 * @param nBins Description of the param.
	 */
	Histogram(const double& Start, const double& End, const unsigned int& nBins) :
		Start(Start), nBins_by_interval(nBins / (End - Start)), nBins(nBins),
				freq(new unsigned int[nBins]) {
		reset();
	}

	/** Constructor
	 * Constructs a histogram by copying from another one.
	 * @param other Description of the param.
	 */
	Histogram(const Histogram& other) :
		Start(other.Start), nBins_by_interval(other.nBins_by_interval), nBins(
				other.nBins), freq(new unsigned int[nBins]) {
		for (unsigned int i(0); i < nBins; ++i)
			freq[i] = other.freq[i];
	}

	/** Reset
	 * @return void
	 */
	void reset() {
		for (unsigned int i(0); i < nBins; ++i)
			freq[i] = 0U;
		number = total = mean = deviation = 0;
		minValue = std::numeric_limits<double>::max();
		maxValue = std::numeric_limits<double>::min();

	}

	/** Destructor for Histogram */
	~Histogram() {
		delete[] freq;
	}

	/** operator=
	 * Set this histogram equal to another.
	 * @param other Description of the param.
	 * @return The expected result.
	 */
	Histogram& operator=(const Histogram& other) {
		if (this != &other) {
			Start = other.Start;
			nBins_by_interval = other.nBins_by_interval;
			if (nBins != other.nBins) {
				nBins = other.nBins;
				delete[] freq;
				freq = new unsigned int[nBins];
			}
			for (unsigned int i(0); i < nBins; ++i)
				freq[i] = other.freq[i];
		}
		return *this;
	}

	/** Add
	 * @param x Description of the param.
	 * @param ntimes Description of the param.
	 * @return void
	 */
        void add(const double& x, int ntimes) {
		number += ntimes;
		total += (x * ntimes);
		mean += (x * ntimes);
		deviation += (x * x) * ntimes;
		minValue = min(minValue, x);
		maxValue = max(maxValue, x);

		const unsigned int i(static_cast<unsigned int> ((x - Start)
				* nBins_by_interval));

		if (i < nBins) {
			freq[i] += ntimes;
		}
	}

	/** Add
	 * Increase the count for the bin that holds a value that is in range for this histogram.
	 * @param x Description of the param
	 * @return void
	 */
        void add(const double& x) {
                add(x, 1);
	}

	/** end
	 * @return void
	 */
	void end() {
		mean = mean / number;
		deviation = deviation / number - mean * mean;
		deviation = sqrt(deviation);
	}

	/** dumpStr
	 * @param outfile Description of the param.
	 * @return void
	 */
	void dumpStr(ostream &outfile) {
		int maxfreq = 0;
		for (unsigned int i = 0; i < nBins && i <= maxValue; i++) {
			outfile << i << "  " << freq[i] << endl;
			maxfreq = freq[i] > maxfreq ? freq[i] : maxfreq;
		}

		outfile << "# Number: " << number << endl;
		outfile << "# Mean: " << mean << endl;
		outfile << "# Deviation: " << deviation << endl;
		outfile << "# Min: " << minValue << endl;
		outfile << "# Max: " << maxValue << endl;

		unsigned int max = 15;
		for (unsigned int i = maxValue < nBins ? maxValue : nBins; i > 15; i--) {
			if (freq[i] > maxfreq / 10000) {
				max = i;
				break;
			}
		}
		outfile << "# Latest: " << max << endl;
	}

	/** dump
	 * @param name Description of the param.
	 * @param suffix Description of the param.
	 * @return void
	 */
	void dump(const char * name, const char *suffix) {
		string fileName(name);
		fileName.append(suffix);
		ofstream outfile;

		cout << "Writing histogram to: " << fileName << endl;

		outfile.open(fileName.c_str());

		dumpStr(outfile);

		outfile.close();
	}

	/** GetTotalCount
	 * Get the sum of all counts in the histogram.
	 * @return The expected result.
	 */
        const unsigned int getTotalCount() const {
		unsigned int c(0U);
		for (unsigned int i(0); i < nBins; ++i)
			c += freq[i];
		return c;
	}
};
