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
class HDTFactory {

public:
	/**
	 * Creates a default HDT (BasicHDT)
	 *
	 * @return HDT
	 */
	static HDT *createDefaultHDT();

	/**
	 * Creates a BasicHDT
	 *
	 * @return HDT
	 */
	static HDT *createBasicHDT(HDTSpecification &specification);
};
}

#endif


