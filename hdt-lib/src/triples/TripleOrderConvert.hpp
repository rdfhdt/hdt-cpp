/*
 * File: TripleOrderConvert.hpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * Copyright (C) 2012, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
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

void swapComponentOrder(UnorderedTriple *triple, TripleComponentOrder parsing, TripleComponentOrder to);
void swapComponentOrder(TripleID *triple, TripleComponentOrder parsing, TripleComponentOrder to);

TripleComponentOrder invertOrder(TripleComponentOrder src);

}

#endif /* TRIPLEORDERCONVERT_HPP_ */
