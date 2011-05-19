/*
 * ControlInformation.cpp
 *
 *  Created on: 29/04/2011
 *      Author: mck
 */

#include <string>
#include <iostream>
#include <sstream>
#include "ControlInformation.hpp"

namespace hdt {

#define TRIPLES_BIT 1
#define DICTIONARY_BIT 2

ControlInformation::ControlInformation() : version(0), components(0) {
	// TODO Auto-generated constructor stub

}

ControlInformation::~ControlInformation() {
	// TODO Auto-generated destructor stub
}

void ControlInformation::save(std::ostream &out) {

	out << "$HDT";

	out.write((char *)&this->version, sizeof(unsigned short));
	out.write((char *)&this->components, sizeof(unsigned short));

	for(PropertyMapIt it = map.begin(); it!=map.end(); it++) {
		out << it->first << ':' << it->second << ';' << std::endl;
	}
	out << "$END" << std::endl;
}

void ControlInformation::load(std::istream &in) {
	std::string line;
	std::string all;

	// Read input
	char hdt[5];
	in.read(hdt, 4);
	hdt[4]=0;
	if(strcmp(hdt,"$HDT")!=0) {
		throw "Non-HDT Section";

	}

	in.read((char *)&this->version, sizeof(unsigned short));
	in.read((char *)&this->components, sizeof(unsigned short));

	while(getline(in,line)) {
		if(line=="$END") {
			break;
		}
		all.append(line);
	}

	// Process options
	std::istringstream strm(all);
	while(getline(strm, all, ';') ){
		if(all!="$END") {
			size_t pos = all.find(':');

			if(pos!=std::string::npos) {
				std::string property = all.substr(0, pos);
				std::string value = all.substr(pos+1);
				//std::cout << "Property= "<< property << "\tValue= " << value << std::endl;
				map[property] = value;
			}
		}
	}
}

void ControlInformation::clear() {
	map.clear();

}

std::string ControlInformation::get(std::string key) {
	return map[key];
}

void ControlInformation::set(std::string key, std::string value) {
	map[key] = value;
}

unsigned int ControlInformation::getUint(std::string key) {
	std::string str = map[key];
	return atoi(str.c_str()); // TODO: WARNING: unsigned int??
}

void ControlInformation::setUint(std::string key, unsigned int value) {
	std::stringstream out;
	out << value;
	map[key] = out.str();
}



void ControlInformation::setVersion(unsigned short  version)
{
	this->version = version;
}

unsigned short ControlInformation::getVersion()
{
	return this->version;
}

bool ControlInformation::getTriples()
{
	return this->components & TRIPLES_BIT;
}

void ControlInformation::setTriples(bool trip)
{
	if(trip) {
		this->components |= TRIPLES_BIT;
	} else {
		this->components &= ~TRIPLES_BIT;
	}
}

void ControlInformation::setDictionary(bool dict)
{
	if(dict) {
		this->components |= DICTIONARY_BIT;
	} else {
		this->components &= ~DICTIONARY_BIT;
	}
}

bool ControlInformation::getDictionary()
{
	return this->components & DICTIONARY_BIT;
}

}
