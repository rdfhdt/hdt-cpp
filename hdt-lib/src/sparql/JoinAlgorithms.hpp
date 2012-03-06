/*
 * JoinAlgorithms.hpp
 *
 *  Created on: 13/09/2011
 *      Author: mck
 */

#ifndef JOINALGORITHMS_HPP_
#define JOINALGORITHMS_HPP_

#include <SingleTriple.hpp>

namespace hdt {

#define SUBJA_SUBJB 0
#define SUBJA_PREDB 1
#define SUBJA_OBJB 2
#define PREDA_SUBJB 3
#define PREDA_PREDB 4
#define PREDA_OBJB 5
#define OBJA_SUBJB 6
#define OBJA_PREDB 7
#define OBJA_OBJB 8
#define SUBJA_PREDA 9
#define SUBJA_OBJA 10
#define PREDA_OBJA 11
#define SUBJB_PREDB 12
#define SUBJB_OBJB 13
#define PREDB_OBJB 14


unsigned short getComparisons(TripleString &a, TripleString &b);
bool checkComparisons(TripleID &a, TripleID &b, unsigned short cond);
void dumpComparisons(unsigned short cond);

TripleID getSecondPattern(TripleID &pattern, TripleID &tid, unsigned short cond);
TripleID getFirstPattern(TripleID &pattern, TripleID &tid, unsigned short cond);


}


#endif /* JOINALGORITHMS_HPP_ */
