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

#ifndef HDT_ITERATOR_HPP_
#define HDT_ITERATOR_HPP_

#include "SingleTriple.hpp"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

namespace hdt {

class IteratorUCharString {
public:
	virtual ~IteratorUCharString() { }

	virtual bool hasNext() {
		return false;
	}

    /**
      WARNING: the previous returned pointer is not valid after the next call to next()
      */
	virtual unsigned char *next() {
		return 0;
	}

    virtual size_t getNumberOfElements() {
		return 0;
	}

	virtual void freeStr(unsigned char* /*ptr*/) {

	}
};
/*
 * IteratorUCharString that consumes two IteratorUCharString in alphabetical order
 */
class MergeIteratorUCharString: public IteratorUCharString {
private:
	IteratorUCharString* it1;
	IteratorUCharString* it2;
	unsigned char *string1;
	unsigned char *string2;
	int prevString;

public:
	MergeIteratorUCharString(IteratorUCharString* iterator1, IteratorUCharString* iterator2):it1(iterator1), it2(iterator2){

		string1=NULL;
		string2=NULL;
		string str_my_txt1,str_my_txt2;
		if (it1->hasNext()){
			string1 = it1->next();
		}
		if (it2->hasNext()){
			string2=it2->next();
		}

		prevString=0;
	}
	virtual ~MergeIteratorUCharString() { }

	virtual bool hasNext() {
		if (prevString==1){
			return (string2||it1->hasNext());
		}
		else if (prevString==2){
			return (string1||it2->hasNext());
		}
		else return (string1 || string2);
	}

	unsigned char *next() {
		unsigned char * retString;
		// load strings
		if (prevString==1){
			string1=NULL;
			if (it1->hasNext()){
				string1=it1->next();
			}
		}
		else if (prevString==2){
			string2=NULL;
			if (it2->hasNext()){
				string2=it2->next();
			}
		}

		if (string1&&string2){
			int cmp = strcmp(reinterpret_cast<const char*>(string1),reinterpret_cast<const char*>(string2));
			if (cmp<=0){
				retString=string1;
				prevString=1;
			}
			else{
				retString=string2;
				prevString=2;
			}
		}
		else{
			if (string1){
				prevString=1;
				retString = string1;
			}
			else{
				prevString=2;
				retString = string2;
			}
		}
		return retString;
	}

    virtual size_t getNumberOfElements() {
		return it1->getNumberOfElements()+it2->getNumberOfElements();
	}

	virtual void freeStr(unsigned char* /*ptr*/) {

	}
};

class VectorIteratorUCharString : public IteratorUCharString {
private:
	std::vector<std::string> &vector;
    size_t pos;
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

class FileIteratorUCharString : public IteratorUCharString {
private:
    std::ifstream in;
    std::string line;
public:
    FileIteratorUCharString(std::string file) : in(file.c_str(), ios::binary) { }
    virtual ~FileIteratorUCharString() {
        in.close();
    }

    virtual bool hasNext() {
        return in.good();
    }

    virtual unsigned char *next() {
        std::getline(in, line);
        return (unsigned char*)line.c_str();
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
    virtual size_t estimatedNumResults() {
		return 0;
	}
	virtual ResultEstimationType numResultEstimation() {
		return UNKNOWN;
	}
	virtual bool canGoTo() {
		return false;
	}
	virtual void goTo(size_t /*pos*/) {
	/* Absolute repositioning of index: ie. go to the index set in the given argument (pos) */
	}
	virtual void skip(size_t /*pos*/) {
	/* Relative repositioning of index: ie. skip index by given argument places */
	}
    virtual bool findNextOccurrence(size_t /*value*/, unsigned char /*component*/) {
		return false;
	}
	virtual TripleComponentOrder getOrder() {
		return Unknown;
	}

    virtual bool isSorted(TripleComponentRole /*role*/) {
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
    virtual bool canGoTo(){
        return false;
    }
	virtual size_t estimatedNumResults() {
		return 0;
	}
	virtual ResultEstimationType numResultEstimation() {
		return UNKNOWN;
	}
	virtual void skip(size_t /*pos*/) {
	/* Relative repositioning of index: ie. skip index by given argument places */
	}
};

class IteratorUInt {
public:
	virtual ~IteratorUInt() { }

	virtual bool hasNext() {
		return false;
	}

	virtual size_t next() {
		return 0;
	}

	virtual void goToStart() {
	}
};

/*
 * IteratorUCharString that consumes two IteratorUInt, one after the other
 */
class SequentialIteratorUInt: public IteratorUInt {
private:
	IteratorUInt* it1;
	IteratorUInt* it2;
	size_t offset; //the offset to apply to it2

public:
	SequentialIteratorUInt(IteratorUInt* iterator1, IteratorUInt* iterator2, size_t offsetIt2):it1(iterator1), it2(iterator2), offset(offsetIt2){
	}
	virtual ~SequentialIteratorUInt() { }

	virtual bool hasNext() {
		return (it1->hasNext() || it2->hasNext());
	}

	size_t next() {
		if (it1->hasNext()){
			return it1->next();
		}
		else if (it2->hasNext()){
			return (offset+it2->next());
		}
		return 0;
	}
};

}

#endif /* HDT_ITERATOR_HPP_ */
