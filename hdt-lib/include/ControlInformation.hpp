/*
 * ControlInformation.h
 *
 *  Created on: 29/04/2011
 *      Author: mck
 */

#ifndef CONTROLINFORMATION_H_
#define CONTROLINFORMATION_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace hdt {

typedef std::map<std::string, std::string> PropertyMap;
typedef PropertyMap::iterator PropertyMapIt;

class ControlInformation {

public:
	unsigned short version;
	unsigned short components;

	PropertyMap map;

public:
	ControlInformation();
	virtual ~ControlInformation();

	void save(std::ostream &out);
	void load(std::istream &in);
	void clear();

	std::string get(std::string key);
	unsigned int getUint(std::string key);
	void set(std::string key, std::string value);
	void setUint(std::string key, unsigned int value);

	void setVersion(unsigned short version);
	unsigned short getVersion();

	void setDictionary(bool dict);
	bool getDictionary();

	void setTriples(bool trip);
	bool getTriples();
};

}

#endif /* CONTROLINFORMATION_H_ */
