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
	virtual ~TripleIDStringIterator();
	bool hasNext();
	TripleString *next();
	bool hasPrevious();
	TripleString *previous();
	bool canGoTo();
	void goToStart();
	size_t estimatedNumResults();
	ResultEstimationType numResultEstimation();
	void skip(size_t pos);
};

} /* namespace hdt */
#endif /* TRIPLEIDSTRINGITERATOR_HPP_ */
