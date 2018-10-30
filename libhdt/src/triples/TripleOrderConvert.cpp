/*
 * File: TripleOrderConvert.cpp
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
#include <stdexcept>
#include "TripleOrderConvert.hpp"

namespace hdt {

static char TripleComponentOrderStr[7][4] = { "Unk", "SPO", "SOP", "PSO", "POS", "OSP", "OPS" };

TripleComponentOrder parseOrder(const char *str){
    for(size_t i=0;i<7;i++) {
		if(strcmp(TripleComponentOrderStr[i], str)==0) {
			return (TripleComponentOrder)(i);
            }
        }

        return Unknown;
    }

const char *getOrderStr(TripleComponentOrder order) {
	return TripleComponentOrderStr[order];
}

    /** Swap
 * @param a Description of the param.
 * @param b Description of the param.
 * @return void
 */
    inline void swap(char & a, char & b)
    {
        size_t tmp;
        tmp = a;
        a = b;
        b = tmp;
    }

    /** Swap
 * @param a Description of the param.
 * @param b Description of the param.
 * @return void
 */
    inline void swap(size_t & a, size_t & b)
    {
    	cout << "\tSWAP: " << a << ", " << b << endl;
        size_t tmp;
        tmp = a;
        a = b;
        b = tmp;
    }

    /*

This table reflect the swaps needed to convert from
a SRC ComponentOrder to a DST ComponentOrder.

There are three swaps possible:
swap1 (x and y)
swap2 (y and z)
swap3 (x and z)

The order of swaps is important, there are five different:
 xy   12
 xz   13
 yz   23
 xyz1 12 13
 xyz2 12 23

 SRC
 v
      SPO  SOP  PSO  POS  OSP  OPS   <DEST
 SPO       yz   xy   xyz2 xyz1 xz
 SOP  yz        xyz1 xz   xy   xyz2
 PSO  xy   xyz2      yz   xz   xyz1
 POS  xyz1 xz   yz        xyz2
 OSP  xyz2 xy   xz   xyz1      yz
 OPS  xz   xyz1 xyz2 xy   yz

 */
    // swap 1-2
    //xy xyz1 xyz2
    bool swap1tab[6][6] = {
    		{0, 0, 1, 1, 1, 0},
    		{0, 0, 1, 0, 1, 1},
    		{1, 1, 0, 0, 0, 1},
    		{1, 0, 0, 0, 1, 0},
    		{1, 1, 0, 1, 0, 0},
    		{0, 1, 1, 1, 0, 0}
    };

    // swap 1-3
    // xz xyz1
    bool swap2tab[6][6] = {
    		{0, 0, 0, 0, 1, 1},
    		{0, 0, 1, 1, 0, 0},
    		{0, 0, 0, 0, 1, 1},
    		{1, 1, 0, 0, 0, 0},
    		{0, 0, 1, 1, 0, 0},
    		{1, 1, 0, 0, 0, 0}
    };

    // swap 2-3
    // yz xyz2
    bool swap3tab[6][6] = {
    		{0, 1, 0, 1, 0, 0},
    		{1, 0, 0, 0, 0, 1},
    		{0, 1, 0, 1, 0, 0},
    		{0, 0, 1, 0, 1, 0},
    		{1, 0, 0, 0, 0, 1},
    		{0, 0, 1, 0, 1, 0}
    };

    /**
 * Convert parsing
 * @param triple Triple to convert
 * @param from Source TripleComponentOrder
 * @param to Destination TripleComponentOrder
 */
    void swapComponentOrder(UnorderedTriple *triple, TripleComponentOrder from, TripleComponentOrder to)
    {
        if(from == to)
            return;

        if(from==Unknown || to==Unknown){
        	throw std::runtime_error("Cannot swap Unknown orders");
        }

        bool swap1 = swap1tab[from - 1][to - 1];
        bool swap2 = swap2tab[from - 1][to - 1];
        bool swap3 = swap3tab[from - 1][to - 1];
        if(swap1){
            swap(triple->x, triple->y);
        }
        if(swap2){
            swap(triple->x, triple->z);
        }
        if(swap3){
            swap(triple->y, triple->z);
        }
    }

/**
 * Convert parsing
 * @param triple Triple to convert
 * @param from Source TripleComponentOrder
 * @param to Destination TripleComponentOrder
 */
void swapComponentOrder(TripleID *triple, TripleComponentOrder from, TripleComponentOrder to) {
	//cout << "Order from: " << getOrderStr(from) << " to " << getOrderStr(to) << " = >" << *triple << endl;
    if(from == to)
        return;

    if(from==Unknown || to==Unknown){
    	throw std::runtime_error("Cannot swap Unknown orders");
    }

    bool swap1 = swap1tab[from - 1][to - 1];
    bool swap2 = swap2tab[from - 1][to - 1];
    bool swap3 = swap3tab[from - 1][to - 1];
    if(swap1){
        size_t tmp = triple->getSubject();
    	triple->setSubject(triple->getPredicate());
    	triple->setPredicate(tmp);
        //swap(triple->x, triple->y);
    }
    if(swap2){
        size_t tmp = triple->getSubject();
    	triple->setSubject(triple->getObject());
    	triple->setObject(tmp);
        //swap(triple->x, triple->z);
    }
    if(swap3){
        size_t tmp = triple->getPredicate();
    	triple->setPredicate(triple->getObject());
    	triple->setObject(tmp);
        //swap(triple->y, triple->z);
    }
	//swapComponentOrder(reinterpret_cast<UnorderedTriple *>(triple), from, to);
	//cout << "Result: " << *triple << endl;
}

UnorderedTriple *getUnorderedTriple(TripleComponentOrder type)
{
	switch(type) {
	case Unknown:
	case SPO:
		return new UnorderedTripleSPO();
	case SOP:
		return new UnorderedTripleSOP();
	case PSO:
		return new UnorderedTriplePSO();
	case POS:
		return new UnorderedTriplePOS();
	case OSP:
		return new UnorderedTripleOSP();
	case OPS:
		return new UnorderedTripleOPS();
	}
	throw std::runtime_error("Invalid TripleComponentOrder type");
}

TripleComponentOrder invertOrder(TripleComponentOrder src) {
	const static TripleComponentOrder orderInverse[7] = { Unknown, OPS, POS, OSP, SOP, PSO, SPO };
	return orderInverse[src];
}

}
