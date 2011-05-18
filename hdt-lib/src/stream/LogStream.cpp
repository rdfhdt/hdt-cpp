/*
 * LogStream.cpp
 *
 *  Created on: 17/05/2011
 *      Author: mck
 */

#include "LogStream.hpp"

namespace hdt {

LogStream::LogStream() {
	// TODO Auto-generated constructor stub

}

LogStream::~LogStream() {
	// TODO Auto-generated destructor stub
}


unsigned int LogStream::get(unsigned int position)
{
	return 0;
}

void LogStream::add(IteratorUint &elements)
{

}

void LogStream::load(std::istream & input)
{
#if 0
	unsigned int numRead=0;
	unsigned int numElements;
	unsigned int readItem;
	input.read((char *)&numElements, sizeof(unsigned int));

	vector.reserve(numElements);

	while(input.good() && numRead<numElements) {
		input.read((char *) &readItem, sizeof(unsigned int));
		vector.push_back(readItem);
		numRead++;
	}
#endif
}



void LogStream::save(std::ostream & output)
{
	unsigned int numElements = getNumberOfElements();

	output.write((char *)&numElements, sizeof(unsigned int));

	// FIXME
}



unsigned int LogStream::getNumberOfElements()
{
	return 0;
}

unsigned int LogStream::size()
{
	return 0;
}

}
