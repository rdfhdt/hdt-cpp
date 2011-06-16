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
	bool save(std::ostream &output, ControlInformation &ci, ProgressListener *listener = NULL){ return true; }
	void load(std::istream &input, ControlInformation &ci, ProgressListener *listener = NULL){ }

	/**
	 * Returns the number of triples
	 *
	 * @return
	 */
	unsigned int getNumberOfElements() {
		return 0;
	}

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
