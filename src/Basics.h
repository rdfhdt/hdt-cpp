/* Basics.h
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * Basic functions and datatype definitions
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
 
#ifndef BASICS_H
#define BASICS_H

#include <sys/resource.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

#define INT32 4

// Points a variable value in a query
#define QVARIABLE 0
// Points out some error
#define ERROR -1

// Graph parsing constant
enum ParsingType {
	SPO = 1,
	SOP,
	PSO,
	POS,
	OSP,
	OPS
};

static const char *parsingTypeStr[] = { "", "SPO","SOP","PSO","POS","OSP","OPS" }; 

#define SPO 1
#define SOP 2
#define PSO 3
#define POS 4
#define OSP 5
#define OPS 6

// Dictionary mapping constants
#define MAPPING1 1
#define MAPPING2 2

// Dictionary encoding constants
#define PLAIN 1
#define COMPACT 2
#define BITMAP 3
#define K2TREE 4

// Modes for accessing a specific vocabulary in the dictionary
#define VOC_SUBJECT 0
#define VOC_PREDICATE 1
#define VOC_OBJECT 2

struct DataTime
{
	double user;
	double sys;
	double total;
};

struct TripleID
{
    int x;
    int y;
    int z;
};

struct Query
{
    int variables;
    TripleID q;
};

struct TripleString
{
    string x;
    string y;
    string z;
};

/** Parses a Query
/*
/*/
TripleString
static parseQuery(string q)
{
	size_t pos_a = 0, pos_b;
	TripleString qstring;

	// Reads the subject
	pos_b = q.find(" ", pos_a);
	qstring.x = q.substr(pos_a, pos_b-pos_a);
	pos_a = pos_b+1;

	// Reads the predicate
	pos_b = q.find(" ", pos_a);
	qstring.y = q.substr(pos_a, pos_b-pos_a);
	pos_a = pos_b+1;

	// Reads the predicate
	pos_b = q.find(" ", pos_a);
	qstring.z = q.substr(pos_a, pos_b-pos_a);
	pos_a = pos_b;

	return qstring;
}

/** Get the time
/*
/*/
void 
static getTime(DataTime *t)
{
	struct rusage buffer;
	struct timeval tp;
	struct timezone tzp;

	getrusage( RUSAGE_SELF, &buffer );
	gettimeofday( &tp, &tzp );

	t->user = (double) buffer.ru_utime.tv_sec + 1.0e-6 * buffer.ru_utime.tv_usec;
	t->total = (double) tp.tv_sec + 1.0e-6 * tp.tv_usec;
}

/** Print the credits
/*
/*/
void static printCredits()
{
	cout<< "\n *** HDT IT! prototype v.0.7 2010. All rights reserved.***";
	cout<< "\n    University of Valladolid (Spain)";
	cout<< "\n\n    This is an experimental prototype. Contacting the authors:";
	cout<< "\n    Javier D. Fernandez & Miguel A. Martinez-Prieto";
	cout<< "\n    ETIyT. Departamento de Informatica.";
	cout<< "\n    Camino del Cementerio s/n";
	cout<< "\n    47011 Valladolid (Spain)";
	cout<< "\n\n    jfergar@infor.uva.es;migumar2@infor.uva.es";
	cout<< "\n\n";
}

/** Prints the help
/*
/*/
void static printHelp(char *exe)
{
	cout<<"\n"<<exe<<"[Options]\n";
	cout<<"        -p --parse <config_file>              Parse data set within the given <config_file>\n";	
	cout<<"        -l --load <file>             	      Load HDT structure into memory. Access triples console if serialize command is not given\n";	
	cout<<"            -s --serialize [format]           Serialize the loaded HDT representation to a given format: ntriples (default), rdfxml, turtle, rdfxml-abbrev, rdfxml-xmp, atom, json-triples, json, dot\n";	
	cout<<"            -o --output <file>           	 Serialize output (output.rdf by default)\n";	
	cout<<"        -h --help                             Show this help\n";	
	cout<<"        -c --console                          Entering console mode\n";	
}

/** Prints the help of the console
/*
/*/
void static printHelpConsole()
{
	cout<<"        parse <config_file>            Parse data set within the given <config_file>\n";	
	cout<<"        load <file>                    Load HDT structure into memory and access triples console\n";	
	cout<<"        help                           Show this help\n";	
	cout<<"        exit                           Exit console mode\n";	
}

/** Splits a string with an specific separator
/*
/*/
void static stringExplode(string str, string separator, vector<string>* results){
	
    int found;
    found = str.find_first_of(separator);
    while(found != string::npos){
        if(found > 0){
			
            results->push_back(str.substr(0,found));
        }
        str = str.substr(found+1);
        found = str.find_first_of(separator);
    }
    if(str.length() > 0){
        results->push_back(str);
    }
}
#endif  /* _BASICS_H */
