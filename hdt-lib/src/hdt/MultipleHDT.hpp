/*
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
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#ifndef MULTIPLEHDT_HPP_
#define MULTIPLEHDT_HPP_

#include <HDTSpecification.hpp>
#include <HDT.hpp>
#include <stdio.h>
#include <string.h>
#include <string>
#include <ext/hash_map>
#include "BasicHDT.hpp"

#include "../util/filemap.h"

#ifdef GOOGLE_HASH
#include <sparsehash/sparse_hash_map>

using google::sparse_hash_map;      // namespace where class lives by default
using __gnu_cxx::hash;// or __gnu_cxx::hash, or maybe tr1::hash, depending on your OS

#else

namespace std {
using namespace __gnu_cxx;
}

#endif

namespace hdt {

struct string_cmp {

	bool operator()(const char* s1, const char* s2) const {
		return strcmp(s1, s2) == 0;
	}
};

#ifdef GOOGLE_HASH
typedef sparse_hash_map<const char *, HDT *, hash<const char *>, string_cmp> ContextEntryHash;
#else
typedef std::hash_map<const char *, HDT *, __gnu_cxx ::hash<const char *>,
		string_cmp> ContextEntryHash;
#endif

typedef ContextEntryHash::const_iterator ContextEntryIt;

class MultipleHDT: public BasicHDT {
private:

	ContextEntryHash HDTMultiple;

public:
	MultipleHDT() :
			BasicHDT() {
	}
	;

	virtual ~MultipleHDT() {
	}
	;

	/**
	 * loads several HDTs from files, using memory mapping
	 * @param input
	 */
	void mapHDTs(vector<string> files, ProgressListener *listener = NULL);

	/**
	 * @param subject
	 * @param predicate
	 * @param object
	 * @return
	 */
	/*IteratorTripleString *search(const char *subject, const char *predicate,
				const char *object){
		return search(subject, predicate, object, std::vector<const char *>());
	}*/
	IteratorTripleString *search(const char *subject, const char *predicate,
			const char *object,
			vector<const char *> contexts = std::vector<const char *>());

	IteratorTripleString *search(const char *subject, const char *predicate,
			const char *object, const char *context) {
		vector<const char *> contexts;
		if (context!=NULL)
			contexts.push_back(context);
		return search(subject, predicate, object, contexts);
	}

	IteratorTripleString *search(TripleString &pattern,	vector<const char *> contexts = std::vector<const char *>()) {
		return search(pattern.getSubject().c_str(),
				pattern.getPredicate().c_str(), pattern.getObject().c_str(),contexts);
	}
	IteratorTripleString *search(TripleString &pattern, const char *context) {
		vector<const char *> contexts;
				contexts.push_back(context);
			return search(pattern.getSubject().c_str(),
					pattern.getPredicate().c_str(), pattern.getObject().c_str(),contexts);
		}

};

class VectorIteratorTripleString: public IteratorTripleString {
private:
	std::vector<IteratorTripleString*> vect;
	size_t pos;
public:
	VectorIteratorTripleString(
			std::vector<IteratorTripleString*> vectorResults) {
		vect.assign(vectorResults.begin(), vectorResults.end());
		pos = 0;
	}
	virtual ~VectorIteratorTripleString() {
		for (int i = 0; i < vect.size(); i++)
			delete vect[i];
	}

	virtual bool hasNext() {
		return (pos < (vect.size() - 1) || vect[pos]->hasNext());
	}

	virtual TripleString *next() {
		if (vect[pos]->hasNext()) {
			return vect[pos]->next();
		} else {
			pos++;
			return vect[pos]->next();
		}
	}
};

}

#endif /* MultipleHDT_HPP_ */
