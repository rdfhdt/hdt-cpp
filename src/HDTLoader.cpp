/* HDTLoader.h
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * HDT Loader for efficient and effective RDF-based applications.
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
 
#include "HDTLoader.h"

HDTLoader::HDTLoader()
{
}

bool 
HDTLoader::load(char* location)
{
	DataTime t1, t2;
	getTime(&t1);

	cout.precision(4);
	
	cout << "[" << showpoint << t1.user - t1.user << "]  Loading HDT" << endl;
	
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Reading Header" << endl;
	header = new Header();
	header->read(location);
	
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Loading Dictionary" << endl;
	
	int mapping = header->getMapping();
	
	if ((mapping != MAPPING1) && (mapping != MAPPING2))
			cout << "   <ERROR> Mapping " << mapping << " is not supported" << endl;

	dictionary = new PlainDictionary(location, mapping, header->getD_separator(), header->getNsubjects(), header->getNpredicates(), header->getNobjects(), header->getSsubobj());
	
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Loading Console" << endl;
	switch(header->getT_encoding())
	{
		case PLAIN:
			triples = new PlainTriples(dictionary, header->getNtriples(), header->getParsing(), location);
			break;
		case COMPACT:
			triples = new CompactTriples(dictionary, header->getNtriples(), header->getParsing(), location);
			break;
		case BITMAP:
			triples = new BitmapTriples(dictionary, header->getNtriples(), header->getParsing(), location);
			break;
		case K2TREE:
			cout << "   <WARNING> K2-tree Triples encoding is not currently supported" << endl;
			return false;
			break;
		default:
			cout << "   <ERROR> Current triples encoding  is not supported" << endl;
			return false;
	}
	
	return true;
}

void
HDTLoader::console()
{
	triples->console();
}

void
HDTLoader::serialize(char *output, char *format)
{
	triples->serialize(output,format);
}

HDTLoader::~HDTLoader()
{
	delete header;
}
