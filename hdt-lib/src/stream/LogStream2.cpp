/*
 * LogStream2.cpp
 *
 *  Created on: 17/05/2011
 *      Author: mck
 */

#include <iostream>
#include <HDTVocabulary.hpp>
#include "LogStream2.hpp"

using namespace std;

namespace hdt {

LogStream2::LogStream2() : numelements(0), numbits(32) {

}

LogStream2::LogStream2(unsigned int numbits) : numelements(0), numbits(numbits) {

}

LogStream2::LogStream2(unsigned int numbits, unsigned int num) : numelements(0), numbits(numbits) {
	size_t totalSize = numElementsFor(numbits, num);
	array.resize(totalSize);
}

LogStream2::~LogStream2() {

}


unsigned int LogStream2::get(unsigned int position)
{
	if(position>=numelements) {
		throw "Out of bound exception";
	}

	return get_field(&array[0], numbits, position);
}

void LogStream2::add(IteratorUint &elements)
{
	unsigned int max = 0;
	numelements = 0;

	// Count and calculate number of bits needed per element.
	while(elements.hasNext()) {
		unsigned int element = elements.next();
		max = element > max ? element : max;
		numelements++;
	}

	// Prepare array
	numbits = bits(max);
	array.clear();
	array.resize(numElementsFor(numbits, numelements));

	// Save
	elements.goToStart();
	unsigned int count = 0;
	while(elements.hasNext()) {
		unsigned int element = elements.next();
		set_field(&array[0], numbits, count, element);
		count++;
	}

}

void LogStream2::set(unsigned int position, unsigned int value) {
	// Todo: validate position and value.
	set_field(&array[0], numbits, position, value);
}

void LogStream2::push_back(unsigned int element) {
	size_t neededSize = numElementsFor(numbits, numelements+1);
	if(array.size()<neededSize) {
		// TODO: Grow by doubling?
		array.resize(neededSize);
	}
	set(numelements, element);
	numelements++;
}

void LogStream2::reduceBits() {
	unsigned int max = 0;
	for(unsigned int i=0;i<numelements;i++) {
		unsigned int value = get(i);
		max = value>max ? value : max;
	}
	unsigned int newbits = bits(max);

	if(newbits!=numbits) {
		for(unsigned int i=0;i<numelements;i++) {
			unsigned int value = get_field(&array[0], numbits, i);
			set_field(&array[0], newbits, i, value);
		}
		numbits = newbits;
		size_t totalSize = numElementsFor(numbits, numelements);
		array.resize(totalSize);
	}
}

inline void swap(size_t &a, size_t &b) {
	register size_t tmp;
	tmp = a;
	a = b;
	b = tmp;
}

void LogStream2::load(std::istream & input)
{
	input.read((char*)&numbits, sizeof(numbits));
	input.read((char*)&numelements, sizeof(numelements));

	//cout << "Load numbits: " << numbits << " numelements:" << numelements << endl;

	array.resize(numElementsFor(numbits, numelements));
	size_t sizeBytes = numBytesFor(numbits, numelements);
	input.read((char*)&array[0], sizeBytes );
	swapBytesEndian();
}

void LogStream2::save(std::ostream & output)
{
	output.write((char*)&numbits, sizeof(numbits));
	output.write((char*)&numelements, sizeof(numelements));

	swapBytesEndian();

#if 0
	for(int i=1;i<10;i++) {
		std::cout << "To store 10 elements of " << i << " bits: Entries: " << numElementsFor(10, i) << " Bytes: " <<numBytesFor(10, i) << std::endl;
	}
	for(unsigned int i=0;i<numEntries;i++) {
		//swap(array[i], array[i+1]);
		std::cout << "Item: " << i << " => " << array[i] << std::endl;
	}
	std::cout << "Saving on " << sizeof(size_t) << "byte words Elements: " << numelements << " Bits: " << numbits << " Entries: " << numEntries << " Bytes: " << numBytes << std::endl;
#endif

	output.write((char*)&array[0], numBytesFor(numbits, numelements));

	swapBytesEndian();
}

void LogStream2::swapBytesEndian()
{
#if 0
	size_t numEntries = numElementsFor(numbits, numelements);
	size_t numBytes = numBytesFor(numbits, numelements);

	if(sizeof(size_t)==4) {
		// Swap integers on 32 bit platforms due to endianneess.
		for(unsigned int i=0;i<numEntries;i+=2) {
			swap(array[i], array[i+1]);
		}
	}
#endif
}

unsigned int LogStream2::getNumberOfElements()
{
	return numelements;
}

unsigned int LogStream2::size()
{
	return numBytesFor(numbits, numelements);
}

std::string LogStream2::getType()
{
	return HDTVocabulary::STREAM_TYPE_LOG2;
}



}
