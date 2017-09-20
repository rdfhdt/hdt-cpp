/*
 * File: HDTFactory.hpp
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

#ifndef HDT_FACTORY_
#define HDT_FACTORY_

#include <HDTSpecification.hpp>
#include <HDT.hpp>
#include <string>

namespace hdt {

/**
 * Factory of HDT classes
 */
class HDTFactory {

public:
	/**
	 * Creates a default HDT
	 * @return HDT
	 */
	static HDT *createDefaultHDT();

	/**
	 * Creates a BasicHDT with the suplied specification
	 * @return HDT
	 */
	static HDT *createHDT(HDTSpecification &specification);

	/**
	 * Creates a default HDT
	 * @return HDT
	 */
	static ModifiableHDT *createDefaultModifiableHDT();

	/** Create a ModifiableHDT that satisfies the specified specification.
	 *
	 * @param specification
	 * @return
	 */
	static ModifiableHDT *createModifiableHDT(HDTSpecification &specification);

	/** Returns the instance of Header as specified in the ControlInformation
	 *
	 * @param controlInformation
	 * @return
	 */
	static Header *readHeader(ControlInformation &controlInformation);

	/** Returns the instance of Dictionary as specified in the ControlInformation
	 *
	 * @param controlInformation
	 * @return
	 */
	static Dictionary *readDictionary(ControlInformation &controlInformation);

	/** Returns the instance of Triples as specified in the ControlInformation
	 *
	 * @param controlInformation
	 * @return
	 */
	static Triples *readTriples(ControlInformation &controlInformation);
};
}

#endif


