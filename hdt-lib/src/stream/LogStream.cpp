/*
 * LogStream.cpp
 *
 *  Created on: 17/05/2011
 *      Author: mck
 */

#include <libcdsBasics.h>
#include <HDTVocabulary.hpp>
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
		max = element > max ? element : max;
	}

	if(array!=NULL) {
		delete array;
		array=NULL;
	}

	array = new cds_utils::Array(vector, cds_utils::bits(max));
}

void LogStream::load(std::istream & input)
{
	std::ifstream *in = dynamic_cast<std::ifstream *>(&input);

	if(array!=NULL){
		delete array;
		array=NULL;
	}

	array = new cds_utils::Array(*in);
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
	return HDTVocabulary::STREAM_TYPE_LOG;
}

}
