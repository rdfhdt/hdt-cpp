/* Dictionary.cpp
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * Abstract class to implement different forms of the Dictionary component
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

#include "Dictionary.h"

unsigned int 
Dictionary::getMaxID()
{
	int s = nsubjects;
	int o;
	
	if (mapping == MAPPING1)
		o = nsubjects + nobjects - ssubobj;
	else
		o = nobjects;
		
	if (s > o) return s;
	else return o;
}

unsigned int
Dictionary::getMapping()
{
	return mapping;
}

unsigned int 
Dictionary::getNsubjects()
{
	return nsubjects;
}

unsigned int 
Dictionary::getNpredicates()
{
	return npredicates;
}

unsigned int 
Dictionary::getNobjects()
{
	return nobjects;
}

unsigned int 
Dictionary::getSsubobj()
{
	return ssubobj;
}

void
Dictionary::dumpStats(string file) 
{
	ofstream out;
	
	out.open(file.c_str(), ios::out);
	out<<"Subjects: "<<nsubjects<<endl;
	out<<"Objects: "<<nobjects<<endl;
	out<<"Predicates: "<<npredicates << endl;
	out<<"Shared: "<<ssubobj<<endl;

	
	out.close();
}
