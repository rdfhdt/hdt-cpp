/*
 * File: ControlInformation.cpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
#include "ControlInformation.hpp"
#include "../util/crc16.h"


using namespace std;

namespace hdt {

#define TRIPLES_BIT 1
#define DICTIONARY_BIT 2
#define HEADER_BIT 4
#define INDEX_BIT 8

ControlInformation::ControlInformation() : version(0), components(0) {
	// TODO Auto-generated constructor stub

}

ControlInformation::~ControlInformation() {
	// TODO Auto-generated destructor stub
}

void ControlInformation::save(std::ostream &out) {

	//std::cout << "Save ControlInformation " << out.tellp() << std::endl;
	out << "$HDT";

	out.write((char *)&this->version, sizeof(uint16_t));
	out.write((char *)&this->components, sizeof(uint16_t));

	string all;

	for(PropertyMapIt it = map.begin(); it!=map.end(); it++) {
		all.append(it->first);
		all.append(":");
		all.append(it->second);
		all.append(";");
		//out << it->first << ':' << it->second << ';' << std::endl;
	}
	all.append("$END\n");
	out.write((char *)all.c_str(), all.length());

    // CRC16
#ifndef NO_CRC
	crc16_t crc = crc16_init();
	crc = crc16_update(crc, (unsigned char *)"$HDT", 4);
	crc = crc16_update(crc, (unsigned char *)&this->version, sizeof(uint16_t));
	crc = crc16_update(crc, (unsigned char *)&this->components, sizeof(uint16_t));

	crc = crc16_update(crc, (unsigned char *)all.c_str(), all.length());


	crc = crc16_finalize(crc);

	out.write((char *)&crc, sizeof(crc));
#else
	uint16_t nullcrc=0;
	out.write((char *)&nullcrc, sizeof(nullcrc));
#endif
}

void ControlInformation::load(std::istream &in) {
	//std::cout << "Load ControlInformation" << in.tellg() << std::endl;
	std::string line;
	std::string all;

	// Read input
	char hdt[5];
	in.read(hdt, 4);
	hdt[4]=0;
	if(strcmp(hdt,"$HDT")!=0) {
		throw "Non-HDT Section";
	}

	in.read((char *)&this->version, sizeof(uint16_t));
	in.read((char *)&this->components, sizeof(uint16_t));

	while(getline(in,line)) {
		all.append(line);
		if(line.length()>=4 && line.compare(line.length()-4, 4, "$END")==0) {
			break;
		}
	}

	// Process options
	std::istringstream strm(all);
	string token;
	while(getline(strm, token, ';') ){
		if(token!="$END") {
			size_t pos = token.find(':');

			if(pos!=std::string::npos) {
				std::string property = token.substr(0, pos);
				std::string value = token.substr(pos+1);
				//std::cout << "Property= "<< property << "\tValue= " << value << std::endl;
				map[property] = value;
			}
		}
	}

	all.append("\n");

	crc16_t filecrc;
	in.read((char *)&filecrc, sizeof(filecrc));

    // CRC16
#ifndef NO_CRC
	crc16_t crc = crc16_init();
	crc = crc16_update(crc, (unsigned char *)"$HDT", 4);
	crc = crc16_update(crc, (unsigned char *)&this->version, sizeof(uint16_t));
	crc = crc16_update(crc, (unsigned char *)&this->components, sizeof(uint16_t));
	crc = crc16_update(crc, (unsigned char *)all.c_str(), all.length());

	crc = crc16_finalize(crc);

	if(filecrc!=crc) {
		throw "CRC of control information does not match.";
	}
#endif
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

uint32_t ControlInformation::getUint(std::string key) {
	std::string str = map[key];
	return atoi(str.c_str()); // TODO: WARNING: unsigned int??
}

void ControlInformation::setUint(std::string key, uint32_t value) {
	std::stringstream out;
	out << value;
	map[key] = out.str();
}



void ControlInformation::setVersion(uint16_t  version)
{
	this->version = version;
}

uint16_t ControlInformation::getVersion()
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

void ControlInformation::setHeader(bool dict)
{
	if(dict) {
		this->components |= HEADER_BIT;
	} else {
		this->components &= ~HEADER_BIT;
	}
}

bool ControlInformation::getHeader()
{
	return this->components & HEADER_BIT;
}

void ControlInformation::setIndex(bool val)
{
	if(val) {
		this->components |= INDEX_BIT;
	} else {
		this->components &= ~INDEX_BIT;
	}
}

bool ControlInformation::getIndex()
{
	return this->components & INDEX_BIT;
}

}
