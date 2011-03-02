/*
 * HDTFactory.cpp
 *
 *  Created on: 02/03/2011
 *      Author: mck
 */

#include <HDT.hpp>
#include "BasicHDT.hpp"
#include <HDTFactory.hpp>

using namespace hdt;

HDT *hdt::HDTFactory::createDefaultHDT()
{
	BasicHDT *h = new BasicHDT();
	return h;
}

HDT *hdt::HDTFactory::createBasicHDT(HDTSpecification &spec)
{
	BasicHDT *h = new BasicHDT(spec);
	return h;
}

