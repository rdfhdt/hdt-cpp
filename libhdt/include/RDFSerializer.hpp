/*
 * File: RDFSerializer.hpp
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

#ifndef HDT_RDFSERIALIZER_HPP_
#define HDT_RDFSERIALIZER_HPP_

#include "SingleTriple.hpp"
#include "Iterator.hpp"
#include "HDTEnums.hpp"
#include "HDTListener.hpp"

#include <iostream>

namespace hdt {

class RDFSerializer {
protected:
	RDFNotation notation;

public:
	RDFSerializer(RDFNotation notation) : notation(notation) { }
	virtual ~RDFSerializer() { }
    virtual void serialize(IteratorTripleString *it, ProgressListener *listener=NULL, size_t totalTriples=0)=0;

	static RDFSerializer *getSerializer(const char *fileName, RDFNotation notation);
	static RDFSerializer *getSerializer(std::ostream &output, RDFNotation notation);
};

}

#endif /* HDT_RDFSERIALIZER_HPP_ */
