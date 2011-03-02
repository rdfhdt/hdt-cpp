/*
 * HDTSpecification.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDTSpecification.hpp>
#include "propertyutil.h"

namespace hdt {


HDTSpecification::HDTSpecification() {

}

HDTSpecification::HDTSpecification(std::string &filename) {
	PropertyUtil::read(filename.c_str(), map);
}

HDTSpecification::~HDTSpecification() {

}

std::string HDTSpecification::get(std::string key) {
	return map[key];
}


}
