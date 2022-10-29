/*
 * File: HDTSpecification.hpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * Copyright (C) 2012, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
 * All rights reserved.
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
 *
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#ifndef HDT_HDTSPECIFICATION_HPP_
#define HDT_HDTSPECIFICATION_HPP_

#include <string>
#include <map>

typedef std::map<std::string, std::string> PropertyMap;

namespace hdt {

/**
 * Provides a specification of all the properties of a HDT to be built.
 */

class HDTSpecification
{
private:
	PropertyMap map;

public:
	/**
	 * Create empty specification.
	 * @return
	 */
	HDTSpecification();

	/**
	 * Create new specification, reading the properties from a file name.
	 * @param filename
	 * @return
	 */
	HDTSpecification(const std::string& filename);

	/** Add a set of options using the syntax property1:value1;property2:value2 */
	void setOptions(const std::string& options);

	/**
	  * Get the value of a property.
	  *
	  * @throws std::out_of_range if the key is not found
	  */
	const std::string& get(const std::string& key);


	/**
	  * Get the value of a property, or "" if the key is not found.
	  */
	const std::string& getOrEmpty(const std::string& key);


	/** Set the value of a property */
	void set(const std::string& key, const std::string& value);
};

}

#endif /* HDT_HDTSPECIFICATION_HPP_ */
