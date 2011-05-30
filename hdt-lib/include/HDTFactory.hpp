/*
 * HDTFactory.hpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

/**
 * Factory that creates HDT objects
 *
 */

#ifndef HDT_FACTORY_
#define HDT_FACTORY_

#include <HDTSpecification.hpp>
#include <HDT.hpp>

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
	static HDT *createBasicHDT(HDTSpecification &specification);

	/** Create a ModifiableHDT that satisfies the specified specification.
	 *
	 * @param specification
	 * @return
	 */
	static ModifiableHDT *createModifiableHDT(HDTSpecification &specification);

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


