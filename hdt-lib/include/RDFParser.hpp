/*
 * File: RDFParser.hpp
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


#ifndef RDFPARSER_H_
#define RDFPARSER_H_

#include <string>
#include <istream>
#include <stdint.h>

#include <SingleTriple.hpp>
#include <Iterator.hpp>
#include <HDTEnums.hpp>


namespace hdt {

class RDFCallback {
public:
	virtual ~RDFCallback() { }

	virtual void processTriple(TripleString &triple, unsigned long long pos)=0;
};

class RDFParserCallback {
public:
	virtual ~RDFParserCallback() { }

	virtual void doParse(const char *fileName, const char *baseUri, RDFNotation notation, RDFCallback *callback)=0;

	static RDFParserCallback *getParserCallback(RDFNotation notation);
};

class RDFParserPull: public IteratorTripleString {

protected:
	RDFNotation notation;

public:
	RDFParserPull(RDFNotation notation) : notation(notation) { }
	virtual ~RDFParserPull() { }

	virtual bool hasNext()=0;
	virtual TripleString *next()=0;
	virtual void reset()=0;
	virtual uint64_t getPos()=0;
	virtual uint64_t getSize()=0;

	static RDFParserPull *getParserPull(const char *filename, RDFNotation notation);
	static RDFParserPull *getParserPull(std::istream &stream, RDFNotation notation);
};

}

#endif /* RDFPARSER_H_ */
