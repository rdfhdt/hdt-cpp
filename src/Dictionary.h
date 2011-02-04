/* Dictionary.h
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * Abstract class to implement different forms of the Dictionary component
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
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A Martinez-Prieto:  migumar2@infor.uva.es
 */

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>

#include "Basics.h"

using namespace std;

class Dictionary {
public:
	virtual void checkTriple(string *node) = 0;
	virtual unsigned int isClusterized(unsigned int id, unsigned int mode) = 0;
	virtual unsigned int retrieveID(string key, unsigned int mode) = 0;
	virtual string retrieveString(unsigned int id, unsigned int mode) = 0;
	virtual void updateID(unsigned int id, unsigned int new_id,
			unsigned int mode) = 0;
	virtual void beginClustering() = 0;
	virtual void endClustering() = 0;
	virtual void lexicographicSort(unsigned int mapping) = 0;
	virtual void split() = 0;
	virtual void write(string path, unsigned int separator) = 0;
	unsigned int getMaxID();
	unsigned int getMapping();
	unsigned int getNsubjects();
	unsigned int getNpredicates();
	unsigned int getNobjects();
	unsigned int getSsubobj();
	unsigned int getNLiterals();
	virtual void dumpStats(string &output) = 0;
protected:
	unsigned int mapping;
	unsigned int nsubjects;
	unsigned int npredicates;
	unsigned int nobjects;
	unsigned int ssubobj;
	unsigned int nliterals;
	unsigned int nuris;
	unsigned int nblanks;
	virtual void initialize() = 0;
	virtual void map(unsigned int mapping) = 0;
};

#include "PlainDictionary.h"

#endif  /* _DICTIONARY_H */
