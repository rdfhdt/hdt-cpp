/*
 * LogStream.cpp
 *
 *  Created on: 17/05/2011
 *      Author: mck
 */

#include "LogStream.hpp"

namespace hdt {

LogStream::LogStream() : array(NULL) {

}

LogStream::~LogStream() {
	if(array!=NULL)
		delete array;
}


unsigned int LogStream::get(unsigned int position)
{
	if(array==NULL)
		return 0;

	if(position >= array->getLength()) {
		return 0;
	} else {
		return array->getField(position);
	}
}

void LogStream::add(IteratorUint &elements)
{
	std::vector<unsigned int> vector;
	unsigned int max = 0;

	while(elements.hasNext()) {
		unsigned int element = elements.next();
		vector.push_back(element);
		max = element > max ? max : element;
	}

	if(array!=NULL) {
		delete array;
		array=NULL;
	}

	array = new cds_utils::Array(vector, 0);
}

void LogStream::load(std::istream & input)
{
	std::ifstream *in = dynamic_cast<std::ifstream *>(&input);

	if(array!=NULL){
		delete array;
		array=NULL;
	}

	array = new cds_utils::Array(*in);

	cout << "Read Array: Elements: " << array->getLength() << " Max: " << array->getMax() << endl;

}

void LogStream::save(std::ostream & output)
{
	if(array==NULL)
		return;

	std::ofstream *out = dynamic_cast<std::ofstream *>(&output);

	array->save(*out);
}

unsigned int LogStream::getNumberOfElements()
{
	if(array!=NULL)
		return array->getLength();
	return 0;
}

unsigned int LogStream::size()
{
	if(array!=NULL)
		return array->getSize();

	return 0;
}

std::string LogStream::getType()
{
	return "http://purl.org/HDT/hdt#streamLog";
}

}
