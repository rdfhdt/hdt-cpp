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
	bool save(std::ostream &output){ return false; }
	void load(std::istream &input){ }

	IteratorTripleString *search(const char *subject, const char *predicate, const char *object){
		return new IteratorTripleString();
	}

	void insert(TripleString &triple){ }
	void insert(IteratorTripleString *triple){ }

	void remove(TripleString &triples){ }
	void remove(IteratorTripleString *triples){ }

	bool edit(TripleString &oldTriple, TripleString &newTriple){ return false; }
};

}

#endif /* EMPTYEADER_H_ */
