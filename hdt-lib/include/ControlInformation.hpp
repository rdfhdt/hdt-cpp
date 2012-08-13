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

#ifndef CONTROLINFORMATION_H_
#define CONTROLINFORMATION_H_

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace hdt {

typedef std::map<std::string, std::string> PropertyMap;
typedef PropertyMap::iterator PropertyMapIt;

/**
 * Represents the header saved at the begining of each HDT block when serialized.
 */
class ControlInformation {

public:
	uint16_t version;
	uint16_t components;

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

	/** Get a property of the ControlInformation
	 * @param key
	 * @return
	 */
	std::string get(std::string key);

	/** Get a property of the ControlInformation as unsigned int
	 * @param key
	 * @return
	 */
	unsigned int getUint(std::string key);

	/**
	 * Set a property of the ControlInformation
	 * @param key
	 * @param value
	 */
	void set(std::string key, std::string value);

	/**
	 * Set a property of the ControlInformation as unsigned int
	 * @param key
	 * @param value
	 */
	void setUint(std::string key, uint32_t value);

	/** Clear the ControlInformation, removing all properties.
	 */
	void clear();

	/** Get the HDT version */
	uint16_t getVersion();
	/** Set the HDT version */
	void setVersion(unsigned short version);

	/** Get whether this ControlInformation represents a Header */
	bool getHeader();
	/** Set whether this ControlInformation represents a Header */
	void setHeader(bool dict);

	/** Get whether this ControlInformation represents a Dictionary */
	bool getDictionary();
	/** Set whether this ControlInformation represents a Dictionary */
	void setDictionary(bool dict);

	/** Get whether this ControlInformation represents Triples */
	bool getTriples();
	/** Set whether this ControlInformation represents Triples */
	void setTriples(bool trip);

	/** Get whether this ControlInformation represents Triples */
	bool getIndex();
	/** Set whether this ControlInformation represents Triples */
	void setIndex(bool trip);
};

}

#endif /* CONTROLINFORMATION_H_ */
