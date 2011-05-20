/*
 * HDTSpecification.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDTSpecification.hpp>
#include "util/propertyutil.h"

namespace hdt {


HDTSpecification::HDTSpecification() {

}

HDTSpecification::HDTSpecification(std::string &filename) {
	if(filename!=""){
		try {
			PropertyUtil::read(filename.c_str(), map);
		} catch (char *except) {
			std::cerr << "WARNING: Could not read config file, using default options." << std::endl;
		}
	}
}

HDTSpecification::~HDTSpecification() {

}

void HDTSpecification::setOptions(std::string options) {

}

std::string HDTSpecification::get(std::string key) {
	return map[key];
}

void HDTSpecification::set(std::string key, std::string value) {
	map[key] = value;
}


}
