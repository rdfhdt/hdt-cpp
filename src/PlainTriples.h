/* PlainTriples.h
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * Plain encoding for Triples
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
 
#ifndef PlainTriples_H
#define PlainTriples_H

#include "Triples.h"
#include "RDFSyntaxMediator.h"

using namespace std;

/** PlainTriples
 *   It implements the abstract class Triples on a plain encoding..
 *
 *  @author Miguel A. Martinez-Prieto
 */
class PlainTriples : public Triples
{
public:	
	/** Generic constructor */
	PlainTriples();
	/** Constructor for parsing*/
	PlainTriples(Dictionary *dictionary, unsigned int ntriples, unsigned int parsing);
	/** Constructor for loading*/
	PlainTriples(Dictionary *dictionary, unsigned int ntriples, unsigned int parsing, string path);

	void console();
	bool transformToN3();
	unsigned int write(string path);
	/** Serialize HDT to a given format*/
	bool serialize(char *output, char *format);

	/** Destructor */
	~PlainTriples();
};

#endif  /* _PlainTriples_H */
