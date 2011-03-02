/*
 * PlainDictionary.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include "PlainDictionary.hpp"

namespace hdt {

PlainDictionary::PlainDictionary() {

}

PlainDictionary::~PlainDictionary() {

}

std::string PlainDictionary::idToString(unsigned int id, TriplePosition position)
{
}

unsigned int PlainDictionary::stringToId(std::string & str, TriplePosition position)
{
}

void PlainDictionary::startProcessing()
{
}

void PlainDictionary::load(std::istream & input)
{
}

unsigned int PlainDictionary::numberOfElements()
{
}

void PlainDictionary::stopProcessing()
{
}

bool PlainDictionary::save(std::ostream & output)
{
}

unsigned int PlainDictionary::insert(std::string & str, TriplePosition position)
{
}

TripleString PlainDictionary::tripleIDtoTripleString(TripleID tripleID)
{
}

TripleID PlainDictionary::tripleStringtoTripleID(TripleString tripleString)
{
}

}
