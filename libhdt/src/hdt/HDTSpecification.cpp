/*
 * File: HDTSpecification.cpp
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

#include <sstream>

#include <HDTSpecification.hpp>
#include "../util/propertyutil.h"

namespace hdt {

HDTSpecification::HDTSpecification() {}

HDTSpecification::HDTSpecification(const std::string& filename) {
	if(!filename.empty()){
		try {
			PropertyUtil::read(filename.c_str(), map);
		} catch (std::exception& e) {
			std::cerr << "WARNING: Could not read config file, using default options." << std::endl;
		}
	}
}

void HDTSpecification::setOptions(const std::string& options) {
	std::istringstream strm(options);
	std::string singleOption;
	while(getline(strm, singleOption, ';') ){
		size_t pos = singleOption.find('=');

		if(pos!=std::string::npos) {
			std::string property = singleOption.substr(0, pos);
			std::string value = singleOption.substr(pos+1);
			//std::cout << "Property= "<< property << "\tValue= " << value << std::endl;
			map[property] = value;
		}
	}
}

const std::string& HDTSpecification::get(const std::string& key) {
	return map.at(key);
}

const std::string emptyString = "";

const std::string& HDTSpecification::getOrEmpty(const std::string& key) {

	/* Webassembly does not play nice with C++ 
	
	https://emscripten.org/docs/porting/exceptions.html
	https://stackoverflow.com/questions/69608789/c-exception-to-exception-less
	
	*/
	#ifdef __EMSCRIPTEN__
	auto it = map.find(key);
	return it == map.end() ? emptyString : it->second;

	#else
	try {
		return map.at(key);
	}catch (std::exception& e) {
		return emptyString;
	}
	#endif
}

void HDTSpecification::set(const std::string& key, const std::string& value) {
	map[key] = value;
}

} // namespace hdt
