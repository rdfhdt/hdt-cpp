#ifndef CONTROLINFORMATION_H_
#define CONTROLINFORMATION_H_

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
	unsigned short version;
	unsigned short components;

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
	void setUint(std::string key, unsigned int value);

	/** Clear the ControlInformation, removing all properties.
	 */
	void clear();

	/** Get the HDT version */
	unsigned short getVersion();
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
};

}

#endif /* CONTROLINFORMATION_H_ */
