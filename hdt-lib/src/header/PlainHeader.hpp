/*
 * PlainHeader.h
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifndef PLAINHEADER_H_
#define PLAINHEADER_H_

#include <HDT.hpp>
#include <Header.hpp>

namespace hdt {

class PlainHeader : public Header {
private:
	HDTSpecification spec;
	vector<TripleString> triples;

public:
	PlainHeader();
	PlainHeader(HDTSpecification &specification);
	virtual ~PlainHeader();
	bool save(std::ostream &output, ControlInformation &ci);
	void load(std::istream &input, ControlInformation &ci);

	/**
	 * @param subject
	 * @param predicate
	 * @param object
	 * @return
	 */
	IteratorTripleString *search(const char *subject, const char *predicate, const char *object);

	/**
	 *
	 * @param triples
	 */
	void insert(TripleString &triple);

	void insert(IteratorTripleString *triple);

	/**
	 * Deletes with pattern matching
	 *
	 * @param triples
	 */
	void remove(TripleString &triples);

	void remove(IteratorTripleString *triples);
};

}

#endif /* PLAINHEADER_H_ */
