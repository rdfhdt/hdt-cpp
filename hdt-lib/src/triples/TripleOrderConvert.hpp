/*
 * TripleOrderConvert.hpp
 *
 *  Created on: 11/05/2011
 *      Author: mck
 */

#ifndef TRIPLEORDERCONVERT_HPP_
#define TRIPLEORDERCONVERT_HPP_

#include <HDTEnums.hpp>
#include <SingleTriple.hpp>

namespace hdt {

class UnorderedTriple  {
public:
	unsigned int x;
	unsigned int y;
	unsigned int z;
};

void swapComponentOrder(UnorderedTriple *triple, TripleComponentOrder parsing, TripleComponentOrder to);



}

#endif /* TRIPLEORDERCONVERT_HPP_ */
