/*
 * PlainDictionary.h
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#ifndef PLAINDICTIONARY_H_
#define PLAINDICTIONARY_H_

#include <Dictionary.hpp>

namespace hdt {

class PlainDictionary : public Dictionary {
public:
	PlainDictionary();
	~PlainDictionary();

	std::string idToString(unsigned int id, TriplePosition position);
	unsigned int stringToId(std::string &str, TriplePosition position);

	/**
	 * Converts a TripleString to a TripleID
	 *
	 * @param tripleString
	 *            The Triple to convert from
	 * @return TripleID
	 */
	TripleID tripleStringtoTripleID(TripleString tripleString);

	/**
	 * Converts a TripleID to a TripleString
	 *
	 * @param tripleID
	 *            The Triple to convert from
	 * @return TripleString
	 */
	TripleString tripleIDtoTripleString(TripleID tripleID);

	bool save(std::ostream &output);
	void load(std::istream &input);
	unsigned int insert(std::string &str, TriplePosition position);
	unsigned int numberOfElements();
	void startProcessing();
	void stopProcessing();
};

}

#endif /* PLAINDICTIONARY_H_ */
