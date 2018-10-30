/* CSD.cpp
 * Copyright (C) 2011, Rodrigo Canovas & Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Abstract class for implementing Compressed String Dictionaries following:
 *
 *   ==========================================================================
 *     "Compressed String Dictionaries"
 *     Nieves R. Brisaboa, Rodrigo Canovas, Francisco Claude,
 *     Miguel A. Martinez-Prieto and Gonzalo Navarro.
 *     10th Symposium on Experimental Algorithms (SEA'2011), p.136-147, 2011.
 *   ==========================================================================
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
 *   Rodrigo Canovas:  rcanovas@dcc.uchile.cl
 *   Miguel A. Martinez-Prieto:  migumar2@infor.uva.es
 */
#include <stdexcept>
#include "CSD.h"
#include "CSD_PFC.h"

#ifdef HAVE_CDS
#include "CSD_HTFC.h"
#include "CSD_FMIndex.h"

#include <libcdsBasics.h>

using namespace cds_utils;

#endif

namespace csd
{

CSD::CSD() : numstrings(0) {

}

CSD * CSD::load(istream & fp)
{
    int type = fp.get();
    if(!fp.good()) {
	throw std::runtime_error("Error reading stream");
    }
    switch(type)
    {
    case PFC:
	return CSD_PFC::load(fp);
#ifdef HAVE_CDS
    case FMINDEX:
	return CSD_FMIndex::load(fp);
    case HTFC:
	return CSD_HTFC::load(fp);
#endif
    default:
	throw std::logic_error("No implementation for CSD");
    }
    return NULL;
}

CSD *CSD::create(unsigned char type)
{
    switch(type)
    {
    case PFC: return new CSD_PFC();
#ifdef HAVE_CDS
    case HTFC: return new CSD_HTFC();
    case FMINDEX: return new CSD_FMIndex();
#endif
    default: throw std::logic_error("No implementation for CSD");
    }

    return NULL;
}

size_t CSD::getLength()
{
	return numstrings;
}

}
