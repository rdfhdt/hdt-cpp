/*
 * WaveletStream.cpp
 *
 *  Created on: 17/05/2011
 *      Author: mck
 */

#include <HDTVocabulary.hpp>

#include "WaveletStream.hpp"

namespace hdt {

WaveletStream::WaveletStream() : sequence(NULL) {

}

WaveletStream::~WaveletStream() {
	if(sequence!=NULL)
		delete sequence;
}


unsigned int WaveletStream::get(unsigned int position)
{
	if(sequence==NULL)
		return 0;

	if(position >= sequence->getLength()) {
		return 0;
	} else {
		return sequence->access(position);
	}
}

void WaveletStream::add(IteratorUint &elements)
{
	std::vector<unsigned int> vector;
	unsigned int max = 0;

	while(elements.hasNext()) {
		unsigned int element = elements.next();
		vector.push_back(element);
		max = element > max ? max : element;
	}

	if(sequence!=NULL) {
		delete sequence;
		sequence=NULL;
	}

	cds_static::BitSequenceBuilder *builder = new cds_static::BitSequenceBuilderRG(20);
	//cds_static::BitSequenceBuilder *builder = new cds_static::BitSequenceBuilderRRR(32);
	cds_static::Mapper *mapper = new cds_static::MapperNone();

	sequence = new cds_static::WaveletTreeNoptrs(&vector[0], vector.size(), builder, mapper);
}

void WaveletStream::load(std::istream & input)
{
	std::ifstream *in = dynamic_cast<std::ifstream *>(&input);

	if(sequence!=NULL){
		delete sequence;
		sequence=NULL;
	}

	cout << "Load WV from " << in->tellg() << endl;
	sequence = cds_static::Sequence::load(*in);
	cout << "AFTER " << in->tellg() << endl;
}

void WaveletStream::save(std::ostream & output)
{
	if(sequence==NULL)
		return;

	std::ofstream *out = dynamic_cast<std::ofstream *>(&output);

	cout << "Save WV to " << out->tellp() << endl;
	sequence->save(*out);
	cout << "AFTER " << out->tellp() << endl;
}

unsigned int WaveletStream::getNumberOfElements()
{
	if(sequence!=NULL)
		return sequence->getLength();
	return 0;
}

unsigned int WaveletStream::size()
{
	if(sequence!=NULL)
		return sequence->getSize();

	return 0;
}

std::string WaveletStream::getType()
{
	return HDTVocabulary::STREAM_TYPE_WAVELET;
}


size_t WaveletStream::rank(unsigned int symbol, size_t pos) {
	if(sequence!=NULL) {
		return sequence->rank(symbol, pos);
	}
	return 0;
}

size_t WaveletStream::select(unsigned int symbol, size_t pos) {
	if(sequence!=NULL) {
		return sequence->select(symbol, pos);
	}
	return 0;
}

}
