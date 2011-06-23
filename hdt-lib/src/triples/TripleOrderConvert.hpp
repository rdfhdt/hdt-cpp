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
public:
	virtual void setSubject(unsigned int subject)=0;
	virtual void setPredicate(unsigned int predicate)=0;
	virtual void setObject(unsigned int object)=0;
	virtual unsigned int getSubject() const = 0;
	virtual unsigned int getPredicate() const = 0;
	virtual unsigned int getObject() const = 0;
};

#define UNORDEREDTRIPLE_CONTENT(a,b,c)\
		void setSubject(unsigned int item) {	a = item; }\
		void setPredicate(unsigned int item) {	b = item; }\
		void setObject(unsigned int item) { c = item; }\
		unsigned int getSubject() const { return a; }\
		unsigned int getPredicate() const {	return b; }\
		unsigned int getObject() const { return c; }

class UnorderedTripleSPO : public UnorderedTriple {
	UNORDEREDTRIPLE_CONTENT(x, y, z)
};
class UnorderedTripleSOP : public UnorderedTriple {
	UNORDEREDTRIPLE_CONTENT(x, z, y)
};
class UnorderedTriplePSO : public UnorderedTriple {
	UNORDEREDTRIPLE_CONTENT(y, x, z)
};
class UnorderedTriplePOS : public UnorderedTriple {
	UNORDEREDTRIPLE_CONTENT(y, z, x)
};
class UnorderedTripleOSP : public UnorderedTriple {
	UNORDEREDTRIPLE_CONTENT(z, x, y)
};
class UnorderedTripleOPS : public UnorderedTriple {
	UNORDEREDTRIPLE_CONTENT(z, y, x)
};

UnorderedTriple *getUnorderedTriple(TripleComponentOrder type);

TripleComponentOrder parseOrder(const char *str);
const char *getOrderStr(TripleComponentOrder order);
void swapComponentOrder(UnorderedTriple *triple, TripleComponentOrder parsing, TripleComponentOrder to);
void swapComponentOrder(TripleID *triple, TripleComponentOrder parsing, TripleComponentOrder to);


}

#endif /* TRIPLEORDERCONVERT_HPP_ */
