/*
 * AdjacencyList.hpp
 *
 *  Created on: 31/05/2011
 *      Author: mck
 */

#ifndef ADJACENCYLIST_HPP_
#define ADJACENCYLIST_HPP_

#include "StreamElements.hpp"

#include <libcdsBasics.h>
#include <BitSequenceRG.h>
#include <BitString.h>

namespace hdt {

class AdjacencyList {
private:
	StreamElements *elements;
	cds_static::BitSequence *bitmap;

public:
	AdjacencyList(StreamElements *el, cds_static::BitSequence *bit);
	virtual ~AdjacencyList();

	size_t find(size_t x);
	size_t find(size_t x, size_t y);

	size_t findListIndex(size_t globalpos);

	size_t last(size_t x);

	size_t countListsX();
	size_t countItemsY(size_t x);

	size_t binSearch(unsigned int element, size_t ini, size_t fin);
	size_t linSearch(unsigned int element, size_t ini, size_t fin);

	unsigned int get(size_t pos);

	size_t getSize();

	void dump();
};

}

#endif /* ADJACENCYLIST_HPP_ */
