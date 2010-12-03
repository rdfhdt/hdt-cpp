/* RDFSyntaxMediator.h
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * RDF syntaxes mediator (parser,serializar) to compact representations.
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
 
#ifndef RDFSYNTAXMEDIATOR_H
#define RDFSYNTAXMEDIATOR_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <raptor.h>
#include "Dictionary.h"
#include "Triples.h"

struct DictionaryTriples
{
	Dictionary *dictionary;
	Triples *triples;
	int *ntriples;
}
;

using namespace std;

/** RDF syntaxMediator
 *   It parses a RDF syntax to elements
 *   It serializes to RDF syntaxes.  
 *
 *  @author Javier D. Fernandez
 */
class RDFSyntaxMediator
{
public:
	
	/** Generic constructor */
	RDFSyntaxMediator();
		
	/** Prepares the RDF file to parse */
	bool parsing(char* pathFile,DictionaryTriples dictionaryTriples);
	
	/** Serialize to a format */
	bool init_serialize(char *pathFile, char *format);
	bool serialize_statement(string subject,string predicate,string object);
	bool end_serialize();
	
	/** Destructor */
	~RDFSyntaxMediator();
			
protected:
	string *node;
	raptor_parser* rdf_parser;
	raptor_serializer* rdf_serializer;
	vector<raptor_namespace*> namespaces;

};

#endif  /* _RDFSYNTAXMEDIATOR_H */
