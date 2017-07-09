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
#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
#include "ControlInformation.hpp"
#include "../util/crc16.h"

using namespace std;

const string EMPTY_STRING = "";

namespace hdt {

ControlInformation::ControlInformation() : type(UNKNOWN_CI) {}

ControlInformation::~ControlInformation() {}

void ControlInformation::save(std::ostream &out) {
	CRC16 crc;
	const unsigned char null='\0';

    // Write cookie
	crc.writeData(out, (unsigned char*)"$HDT", 4);

    // Write type
    const uint8_t typeValue = (uint8_t) type;
    crc.writeData(out, (unsigned char *)&typeValue, sizeof(uint8_t));

    // Write format
    crc.writeData(out, (unsigned char*)format.c_str(), format.length());
    crc.writeData(out, &null, 1);

    // Write options
	string all;
	for(PropertyMapIt it = map.begin(); it!=map.end(); it++) {
		all.append(it->first);
		all.append("=");
		all.append(it->second);
		all.append(";");
		//out << it->first << ':' << it->second << ';' << std::endl;
	}
	crc.writeData(out, (unsigned char *)all.c_str(), all.length());
	crc.writeData(out, &null, 1);

    // CRC16
	crc.writeCRC(out);
}

void ControlInformation::load(std::istream &in) {
	CRC16 crc;
    const unsigned char null='\0';

    this->clear();

	// Read magic cookie
	unsigned char hdt[5];
	crc.readData(in, hdt, 4);
	hdt[4]=0;
    if(strncmp((char*)hdt,"$HDT",4)!=0) {
		throw std::runtime_error("Non-HDT Section");
	}

    // Read type
    unsigned char typeValue;
    crc.readData(in, (unsigned char *)&typeValue, sizeof(typeValue));
    type = (ControlInformationType) typeValue;

    // Read Format
    getline(in, format, '\0');
    crc.update((unsigned char *)format.c_str(), format.length());
    crc.update(&null, 1);

    // Read line with options
	std::string line;
    getline(in,line, '\0');
    std::istringstream strm(line);
    string token;
    while(getline(strm, token, ';') ){
    	size_t pos = token.find('=');

    	if(pos!=std::string::npos) {
    		std::string property = token.substr(0, pos);
    		std::string value = token.substr(pos+1);
    		//std::cout << "Property= "<< property << "\tValue= " << value << std::endl;
    		map[property] = value;
    	}
    }
    crc.update((unsigned char *)line.c_str(), line.length());
    crc.update(&null, 1);

    const crc16_t filecrc = crc16_read(in);

    // CRC16
    if(filecrc!=crc.getValue()) {
		throw std::runtime_error("CRC of control information does not match.");
	}
}

#define CHECKPTR(base, max, size) if(((base)+(size))>(max)) throw std::runtime_error("Could not read completely the HDT from the file.");

size_t ControlInformation::load(const unsigned char *ptr, const unsigned char *maxPtr) {
    this->clear();

	size_t count=0;

	// $HDT
	CHECKPTR(ptr,maxPtr, 5);
	if(strncmp((char *)&ptr[count],"$HDT", 4)!=0) {
		throw std::runtime_error("Non-HDT Section");
	}
	count+=4;

	// Type
	CHECKPTR(ptr,maxPtr, 1);
	type = (ControlInformationType) ptr[count];
	count++;

	// Format
	size_t len = 0;
	for(unsigned char *p=(unsigned char *)&ptr[count]; p<maxPtr && *p; ++p) {
		len++;
	}
	format.assign((char *)&ptr[count], len);
	count += len+1;

	// Read Options
	string all;
	len = 0;
	for(unsigned char *p=(unsigned char *)&ptr[count]; p<maxPtr && *p; ++p) {
		len++;
	}
	all.assign((char *)&ptr[count], len);
	count += len+1;

	// Process options
	std::istringstream strm(all);
	string token;
	while(getline(strm, token, ';') ){
		size_t pos = token.find('=');

		if(pos!=std::string::npos) {
			std::string property = token.substr(0, pos);
			std::string value = token.substr(pos+1);
			//std::cout << "Property= "<< property << "\tValue= " << value << std::endl;
			map[property] = value;
		}
	}

	// CRC16
	CRC16 crc;
	crc.update(&ptr[0], count);
	CHECKPTR(ptr,maxPtr, sizeof(crc16_t));
	const crc16_t filecrc = *((crc16_t *)&ptr[count]);
	if(filecrc!=crc.getValue()) {
		throw std::runtime_error("CRC of control information does not match.");
	}
	count+=sizeof(crc16_t);

	return count;
}

void ControlInformation::clear() {
    this->type = UNKNOWN_CI;
    this->format.clear();
	map.clear();
}


const std::string& ControlInformation::getFormat() const {
	return format;
}

void ControlInformation::setFormat(const std::string& format) {
	this->format = format;
}

const std::string& ControlInformation::get(const std::string& key) const {
  std::map<string,string>::const_iterator it = map.find(key);
  return it == map.end() ? EMPTY_STRING : it->second;
}

void ControlInformation::set(const std::string& key, const std::string& value) {
	map[key] = value;
}

uint64_t ControlInformation::getUint(const std::string& key) const {
  std::map<string,string>::const_iterator it = map.find(key);
  return it == map.end() ? 0 : strtoull(it->second.c_str(), NULL, 10);
}

void ControlInformation::setUint(const std::string& key, uint64_t value) {
	std::stringstream out;
	out << value;
	map[key] = out.str();
}

void ControlInformation::setType(ControlInformationType type) {
    this->type = type;
}

bool ControlInformation::isDefined(const std::string& key) const {
	return map.count(key) && !(map.at(key).empty());
}

ControlInformationType ControlInformation::getType() const {
    return this->type;
}

}
