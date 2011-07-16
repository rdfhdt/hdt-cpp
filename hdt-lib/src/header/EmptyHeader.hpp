/*
 * File EmptyHeader.hpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * Copyright (C) 2011, Javier D. Fernandez, Miguel A. Martinez-Prieto
 *                     Mario Arias, Alejandro Andres.
 * All rights reserved.
 *
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
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *   Mario Arias:               mario.arias@gmail.com
 *   Alejandro Andres:          fuzzy.alej@gmail.com
 *
 */


#ifndef EMPTYHEADER_H_
#define EMPTYHEADER_H_

#include <HDT.hpp>
#include <Header.hpp>

namespace hdt {

class EmptyHeader : public Header {

public:
	EmptyHeader() { }
	virtual ~EmptyHeader(){ }
	bool save(std::ostream &output, ControlInformation &ci, ProgressListener *listener = NULL){ return true; }
	void load(std::istream &input, ControlInformation &ci, ProgressListener *listener = NULL){ }

	/**
	 * Returns the number of triples
	 *
	 * @return
	 */
	unsigned int getNumberOfElements() {
		return 0;
	}

	IteratorTripleString *search(const char *subject, const char *predicate, const char *object){
		return new IteratorTripleString();
	}

	void insert(TripleString &triple){ }
	void insert(IteratorTripleString *triple){ }

	void remove(TripleString &triples){ }
	void remove(IteratorTripleString *triples){ }
};

}

#endif /* EMPTYEADER_H_ */
