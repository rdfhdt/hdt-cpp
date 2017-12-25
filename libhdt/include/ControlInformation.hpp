/*
 * File: ControlInformation.cpp
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

#ifndef HDT_CONTROLINFORMATION_HPP_
#define HDT_CONTROLINFORMATION_HPP_

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace hdt {

typedef std::map<std::string, std::string> PropertyMap;
typedef PropertyMap::iterator PropertyMapIt;

enum ControlInformationType {
    UNKNOWN_CI,
    GLOBAL,
    HEADER,
    DICTIONARY,
    TRIPLES,
    INDEX
};

/**
 * Represents the header saved at the begining of each HDT block when serialized.
 */
class ControlInformation {

public:
	std::string format;
    ControlInformationType type;
	PropertyMap map;

public:
	/**
	 * Create empty ControlInformation
	 * @return
	 */
	ControlInformation();
	virtual ~ControlInformation();

	/** Serialize a ControlInformation to a stream
	 * @param out
	 */
	void save(std::ostream &out);

	/**
	 * Load ControlInformation from a stream.
	 * @param in
	 */
	void load(std::istream &in);

    size_t load(const unsigned char *ptr, const unsigned char *maxPtr);

    const std::string& getFormat() const;

    void setFormat(const std::string& format);

	/** Get a property of the ControlInformation
	 * @param key
	 * @return the value, or the empty string if the key does not exist
	 */
	const std::string& get(const std::string& key) const;

	/** Get a property of the ControlInformation as unsigned int
	 * @param key
	 * @return the value, or 0 if the key does not exist
	 */
    uint64_t getUint(const std::string& key) const;

	/**
	 * Set a property of the ControlInformation
	 * @param key
	 * @param value
	 */
	void set(const std::string& key, const std::string& value);

	/**
	 * Set a property of the ControlInformation as unsigned int
	 * @param key
	 * @param value
	 */
    void setUint(const std::string& key, uint64_t value);

	/** Clear the ControlInformation, removing all properties.
	 */
	void clear();

    void setType(ControlInformationType type);

    bool isDefined(const std::string& key) const;

    ControlInformationType getType() const;
};

}

#endif /* HDT_CONTROLINFORMATION_HPP_ */
