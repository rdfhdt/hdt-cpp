/*
 * UintStream.cpp
 *
 *  Created on: 10/05/2011
 *      Author: mck
 */

#include <iostream>
#include "UintStream.hpp"

namespace hdt {

UintStream::UintStream() {

}

UintStream::~UintStream() {
}

unsigned int UintStream::get(unsigned int position)
{
	return vector[position];
}

void UintStream::add(IteratorUint &elements)
{
	unsigned int count = 0;
	while(elements.hasNext()) {
		vector.push_back(elements.next());
		count++;
	}
}

void UintStream::load(std::istream & input)
{
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
}

void UintStream::save(std::ostream & output)
{
	unsigned int numElements = vector.size();

	output.write((char *)&numElements, sizeof(unsigned int));

	output.write((char *)&vector[0], sizeof(unsigned int)*numElements);
}

unsigned int UintStream::getNumberOfElements()
{
	return vector.size();
}

unsigned int UintStream::size()
{
	return vector.size()*sizeof(unsigned int);
}

std::string UintStream::getType()
{
	return "http://purl.org/HDT/hdt#streamInteger";
}

}
