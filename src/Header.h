/* Dictionary.h
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * Implementation of the Header component
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

#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "Basics.h"

using namespace std;

class Header {
public:
	Header();
	Header(int pars, int map, int sep, int enc, int nsub, int npred, int nobj,
			int nsh, int ntrip, int nlit, int nent);
	void write(string path);
	void read(string path);
	int getParsing();
	int getMapping();
	int getD_separator();
	int getT_encoding();
	int getNsubjects();
	int getNpredicates();
	int getNobjects();
	int getSsubobj();
	int getNtriples();
	int getNliterals();
	int getNentries();
	~Header();
protected:
	int parsing; /**< Graph parsing: SPO ('spo'), SOP ('sop'), PSO ('pso'), POS ('pos'), OSP ('osp'), OPS ('ops') */
	int mapping; /**< ID mapping: MAPPING1 ('single'), MAPPING2 ('shared') */
	int d_separator; /**< Dictionary separator character */
	int t_encoding; /**< Dictionary encoding: PLAIN ('plain'), COMPACT ('compact'), BITMAP ('bitmap'), K2TREE ('k2tree') */
	int nsubjects; /**< Dictionary statistics: number of subjects */
	int npredicates; /**< Dictionary statistics: number of predicates */
	int nobjects; /**< Dictionary statistics: number of objects */
	int ssubobj; /**< Dictionary statistics: number of shared subject objects */
	int nliterals; /**< Dictionary statistics: number of literals */
	int nentries; /**< Dictionary statistics: number of entries */
	int ntriples; /**< Graph size in number of different triples */
};

#endif  /* _HEADER_H */
