/*
 * EmptyHeader.h
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifndef EMPTYHEADER_H_
#define EMPTYHEADER_H_

#include <HDT.hpp>
#include <Header.hpp>

namespace hdt {

class EmptyHeader : public Header {

public:
	EmptyHeader() { }
	virtual ~EmptyHeader(){ }
	bool save(std::ostream &output, ControlInformation &ci){ return true; }
	void load(std::istream &input, ControlInformation &ci){ }

	IteratorTripleString *search(const char *subject, const char *predicate, const char *object){
		return new IteratorTripleString();
	}

	void insert(TripleString &triple){ }
	void insert(IteratorTripleString *triple){ }

	void remove(TripleString &triples){ }
	void remove(IteratorTripleString *triples){ }
};

}

#endif /* EMPTYEADER_H_ */
