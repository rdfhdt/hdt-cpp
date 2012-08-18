/*
 * File: Iterator.hpp
 * Last modified: $Date: 2012-03-06 16:08:16 +0000 (mar, 06 mar 2012) $
 * Revision: $Revision: 183 $
 * Last modified by: $Author: mario.arias@gmail.com $
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

#ifndef ITERATOR_HPP_
#define ITERATOR_HPP_

#include <vector>
#include <string>

#include <SingleTriple.hpp>

namespace hdt {

class IteratorUCharString {
public:
	virtual ~IteratorUCharString() { }

	virtual bool hasNext() {
		return false;
	}

	virtual unsigned char *next() {
		return 0;
	}

	virtual unsigned int getNumberOfElements() {
		return 0;
	}

	virtual void freeStr(unsigned char *ptr) {

	}
};

class VectorIteratorUCharString : public IteratorUCharString {
private:
	std::vector<std::string> &vector;
	unsigned int pos;
public:
	VectorIteratorUCharString(std::vector<std::string> &vector) : vector(vector), pos(0) { }
	virtual ~VectorIteratorUCharString() { }

	virtual bool hasNext() {
		return pos<vector.size();
	}

	virtual unsigned char *next() {
		return (unsigned char *)vector[pos++].c_str();
	}
};

class FileteratorUCharString : public IteratorUCharString {
private:
    ifstream in;
    string line;
public:
    FileteratorUCharString(string file) : in(file, ios::binary) { }
    virtual ~FileteratorUCharString() {
        in.close();
    }

    virtual bool hasNext() {
        return in.good();
    }

    virtual unsigned char *next() {
        std::getline(in, line);
        return line.c_str();
    }
};




class IteratorTripleID {
public:
    virtual ~IteratorTripleID() { }

	virtual bool hasNext() {
		return false;
	}

	virtual TripleID *next() {
		return NULL;
	}

	virtual bool hasPrevious() {
		return false;
	}

	virtual TripleID *previous() {
		return NULL;
	}
	virtual void goToStart() {
	}
	virtual unsigned int estimatedNumResults() {
		return 0;
	}
	virtual ResultEstimationType numResultEstimation() {
		return UNKNOWN;
	}
	virtual bool canGoTo() {
		return false;
	}
	virtual void goTo(unsigned int pos) {
	}
	virtual bool findNextOccurrence(unsigned int value, unsigned char component) {
		return false;
	}
	virtual TripleComponentOrder getOrder() {
		return Unknown;
	}

    virtual bool isSorted(TripleComponentRole role) {
	return false;
    }
};


class IteratorTripleString {
public:
    virtual ~IteratorTripleString() { }

	virtual bool hasNext() {
		return false;
	}
	virtual TripleString *next() {
		return NULL;
	}
	virtual bool hasPrevious() {
		return false;
	}
	virtual TripleString *previous() {
		return NULL;
	}
	virtual void goToStart() {

	}
};

}

#endif /* ITERATOR_HPP_ */
