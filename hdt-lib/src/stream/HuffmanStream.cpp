/*
 * HuffmanStream.cpp
 *
 *  Created on: 19/05/2011
 *      Author: mck
 */

#include <HDTVocabulary.hpp>
#include "HuffmanStream.hpp"

namespace hdt {

HuffmanStream::HuffmanStream() : huffman(NULL) {

}

HuffmanStream::~HuffmanStream() {
	if(huffman!=NULL) {
		delete huffman;
	}
}

unsigned int HuffmanStream::get(unsigned int position)
{
	if(position<vectorPlain.size())
		return vectorPlain[position];

	return 0;
}

void HuffmanStream::add(IteratorUint &elements)
{
	// Fixme: Modify Huffman.cpp class to pass the frequencies and avoid copying.
	while(elements.hasNext()) {
		unsigned int element = elements.next();
		vectorPlain.push_back(element);
	}

	if(huffman!=NULL) {
		delete huffman;
	}

	huffman = new URICompressed::Huffman(&vectorPlain[0], vectorPlain.size());
}

void HuffmanStream::load(std::istream & input)
{
	std::ifstream *in = dynamic_cast<std::ifstream *>(&input);

	huffman = URICompressed::Huffman::load(*in);
	unsigned int numElements;
	unsigned int encEntries;
	size_t pos = 0;

	input.read((char *)&numElements, sizeof(unsigned int));
	input.read((char *)&encEntries, sizeof(unsigned int));

	vectorPlain.resize(numElements);
	unsigned int *encV = new unsigned int[encEntries];

	input.read((char*)encV, encEntries*sizeof(unsigned int));

	for(unsigned int i=0;i<numElements;i++) {
		pos = huffman->decode(&vectorPlain[i], encV, pos);
	}
	delete encV;
}

void HuffmanStream::save(std::ostream & output)
{
	if(huffman==NULL) {
		throw "Must add elements to stream before saving";
	}

	// Create encoded vector
	std::vector<unsigned int> vectorEncoded;
	size_t maxEncodedElements = 1 + huffman->maxLength()*vectorPlain.size()/(sizeof(unsigned int)*8);
	vectorEncoded.resize(maxEncodedElements);

	// Encode stream
	size_t pos = 0;
	for(unsigned int i=0;i<vectorPlain.size();i++){
		pos = huffman->encode(vectorPlain[i], &vectorEncoded[0], pos );
	}

	// Calculate size
	unsigned int numElements = vectorPlain.size();
	unsigned int encodedEntries = 1+pos/(sizeof(unsigned int)*8);
	unsigned int bytes = 1+pos/8;

#if 0
	cout << "Total bits: " << pos << endl;
	cout << "Total entries: " << encodedEntries << endl;
	cout << "Total bytes: " << bytes << endl;
	cout << "Total elements: " << numElements << endl;
#endif

	// Write data to output
	std::ofstream *out = dynamic_cast<std::ofstream *>(&output);
	huffman->save(*out);
	output.write((char*)&numElements, sizeof(unsigned int));
	output.write((char*)&encodedEntries, sizeof(unsigned int));
	output.write((char*)&vectorEncoded[0], encodedEntries * sizeof(unsigned int));
}

unsigned int HuffmanStream::getNumberOfElements()
{
	return vectorPlain.size();
}

unsigned int HuffmanStream::size()
{
	if(huffman!=NULL)
		return huffman->getSize();

	return 0;
}

std::string HuffmanStream::getType()
{
	return HDTVocabulary::STREAM_TYPE_HUFFMAN;
}

}
