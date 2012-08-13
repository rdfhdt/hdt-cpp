/*
 * TripleIDStringIterator.hpp
 *
 *  Created on: 11/08/2012
 *      Author: mck
 */

#ifndef TRIPLEIDSTRINGITERATOR_HPP_
#define TRIPLEIDSTRINGITERATOR_HPP_

#include <HDT.hpp>
#include <Iterator.hpp>

namespace hdt {

class TripleIDStringIterator : public IteratorTripleString {

private:
	Dictionary *dict;
	IteratorTripleID *iterator;
	TripleString result;
public:
	TripleIDStringIterator(Dictionary *dict, IteratorTripleID *iterator);
	bool hasNext();
	TripleString *next();
	bool hasPrevious();
	TripleString *previous();
	void goToStart();
};

} /* namespace hdt */
#endif /* TRIPLEIDSTRINGITERATOR_HPP_ */
