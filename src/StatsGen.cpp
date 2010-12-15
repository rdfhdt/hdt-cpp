/* StatsGen.cpp
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * HDT Statistics generator.
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
 *
 */

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <stdio.h>

#include <iostream>

#include "StatsGen.h"


#include "fdstream.hpp"
#include "Utils.h"

StatsGen::StatsGen()
{
}

bool
StatsGen::readFromDataset(char *dataset, char *output)
{
	
	getTime(&t1);
	cout.precision(4);
	
	string line;
	int ntriples=0;
	
	// Input File/Pipe management
	FILE *filePipe;
	std::string pipeCommand;
	istream *input;
	ifstream *fileStream = NULL; 
	boost::fdistream *pipeStream = NULL;
	
	// RDF Parser
	RDFSyntaxMediator rdfmediator;
	DictionaryTriples dictionaryTriples;
	
	t_encoding = PLAIN;
	mapping = MAPPING1;
	parsing = PSO;
	
	// Performing HDT on the set of properties
	// ***************************************
	
	// 1) Opening the dataset
	string datasetName(dataset);
	
	if(stringHasEnding(datasetName, ".gz")) {
		pipeCommand = "gunzip -c ";
	} else if(stringHasEnding(datasetName, ".bz2")) {
		pipeCommand = "bunzip2 -c ";
	} 
	
	if(pipeCommand.length()>0) {
		pipeCommand.append(datasetName);
		
		if ((filePipe=popen(pipeCommand.c_str(),"r")) == NULL) {
			throw "popen() failed";
		}
		
		input = pipeStream = new boost::fdistream(fileno(filePipe));
	} else {		
		input = fileStream = new ifstream(datasetName.c_str());
	}
	if (!input->good())
	{
		cout << "   <ERROR> Please check the dataset in " << datasetName << endl;
		return false;
	}
	
	// 2) Testing dictionary options: it is built on a first pass
	//    which gathers statistics and performs the chosen ordering.
	//    First, the ordering parameter is retrieved and next the
	//    chosen dictionary class is instantiated.
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Building Dictionary from " << datasetName << endl;
	
	// New plain dictionary with the by default size.
	dictionary = new PlainDictionary();
	
	// Parse
	if (1)
	{
		node = new string[3];
		
		while (getline(*input,line))
		{
			//cout << "> " << line << endl;
			ntriples++;
			parseTripleN3(line);
			dictionary->checkTriple(node);
		}
	} else {
		dictionaryTriples.dictionary = dictionary;
		dictionaryTriples.triples = NULL;
		dictionaryTriples.ntriples= new int();
		*dictionaryTriples.ntriples=0;
		rdfmediator.parsing(const_cast<char*> (dataset),dictionaryTriples);
		ntriples = *dictionaryTriples.ntriples;
	}
	
	// 3) Sorting Dictionary (LEXICOGRAPHIC)
	//    Retrieve the id mapping choice
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Split Dictionary" << endl;
	
	dictionary->split();
	
	// 3) Sorting Dictionary (LEXICOGRAPHIC)
	//    Retrieve the id mapping choice
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Sorting Dictionary" << endl;
	
	
	dictionary->lexicographicSort(mapping);
	
	
	// 4) Testing triples options: it is built on a second pass.
	//    The graph parsing is retrieved and the object is 
	//    instantiated by according with the encoding choice. The
	//    resultant structure is finally sorted on parsing choice.
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Building Triples" << endl;
	
	// Needed?
	if(pipeCommand.length()>0) { // Cannot seek on pipes
		// Close old pipe
		delete pipeStream;
		pclose(filePipe);
		
		// Create new one from the begining
		if ((filePipe=popen(pipeCommand.c_str(),"r")) == NULL) {
			throw "popen() failed";
		}
		
		input = pipeStream = new boost::fdistream(fileno(filePipe));
		
	} else {
		input->clear();
		input->seekg (0, ios::beg);
	}
	
	// PLAIN triples
	triples = new PlainTriples(dictionary, ntriples, parsing);
	
	
	// COMPACT triples	
	//triples = new CompactTriples(dictionary, ntriples, parsing);
	//t_encoding = COMPACT;
	
	// BITMAP triples	
	//triples = new BitmapTriples(dictionary, ntriples, parsing);
	//t_encoding = BITMAP;
	
	
	int subject, predicate, object;
	
	if (1)
	{
		while (getline(*input,line)) 
		{
			parseTripleN3(line);
			
			subject = dictionary->retrieveID(node[0], VOC_SUBJECT);
			predicate = dictionary->retrieveID(node[1], VOC_PREDICATE);
			object = dictionary->retrieveID(node[2], VOC_OBJECT);
			
			triples->insert(subject, predicate, object);
			
			//			cout << subject << "," << predicate << "," << object <<endl;
		}	
	} else {
		rdfmediator;
		dictionaryTriples.triples = triples;
		rdfmediator.parsing(const_cast<char*> (dataset),dictionaryTriples);
		
	}
	ntriples = triples->size();
	
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Sort triples" << endl;
	
	triples->graphSort();
	
	// 6) Writing Dictionary
	getTime(&t2);
	int separator = '\n';
	string dictPath(output);
	dictPath.append("/hdt");
	cout << "[" << showpoint << t2.user - t1.user << "]  Writing Dictionary to: " << dictPath << endl;
	
	mkpathfile(dictPath.c_str(),0744);
	dictionary->write(dictPath, separator);
	
	// 7) Writing Triples
	//    Returns the number of triples effectively writen (note
	//    that the original dataset can contain repeated triples)
	getTime(&t2);
	string triplePath(output);
	triplePath.append("/hdt");
	cout << "[" << showpoint << t2.user - t1.user << "]  Writing Triples to " << triplePath << endl;
	ntriples = triples->write(triplePath);
	
	// 8) Writing Header
	getTime(&t2);
	string headerPath(output);
	headerPath.append("/hdt");
	cout << "[" << showpoint << t2.user - t1.user << "]  Writing Header to " << headerPath << endl;
	
	header = new Header(parsing, mapping, separator, t_encoding, 
						dictionary->getNsubjects(), dictionary->getNpredicates(), dictionary->getNobjects(), dictionary->getSsubobj(),
						ntriples, dictionary->getNLiterals(), dictionary->getMaxID());
	header->write(headerPath);
		
	// Close
	if(pipeCommand.length()>0) {
		delete pipeStream;
		input = NULL;
		pclose(filePipe);
	} else {
		fileStream->close();
		delete fileStream;
		input = NULL;	
	}
	
	delete [] node;
	
	return true;
}

bool
StatsGen::readFromHDT(char *hdt, char*output)
{
	DataTime t1, t2;
	getTime(&t1);
	cout.precision(4);
	
	cout << "[" << showpoint << t1.user - t1.user << "]  Loading HDT" << endl;
	
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Reading Header from: " << hdt << endl;
	header = new Header();
	header->read(hdt);
	
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Loading Dictionary from: " << hdt << endl;
	
	int mapping = header->getMapping();
	
	dictionary = new PlainDictionary(hdt, mapping, header->getD_separator(), header->getNsubjects(), header->getNpredicates(), header->getNobjects(), header->getSsubobj());
	
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Loading Triples from: " << hdt << endl;
	switch(header->getT_encoding())
	{
		case PLAIN:
			triples = new PlainTriples(dictionary, header->getNtriples(), header->getParsing(), hdt);
			break;
		case COMPACT:
			triples = new CompactTriples(dictionary, header->getNtriples(), header->getParsing(), hdt);
			break;
		case BITMAP:
			triples = new BitmapTriples(dictionary, header->getNtriples(), header->getParsing(), hdt);
			break;
		case K2TREE:
			cout << "   <WARNING> K2-tree Triples encoding is not currently supported" << endl;
			return false;
			break;
		default:
			cout << "   <ERROR> Current triples encoding  is not supported" << endl;
			return false;
	}
	
	triples->loadGraphMemory();
	
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "] HDT Succesfully loaded" << endl;
	
	return true;
}

bool 
StatsGen::process(char *output)
{

#if 0
	// 9) Checking optional gnuplot generation (only for parsings 
	//    'pso' and 'pos')
	
	string gnuplotPath(output);
	gnuplotPath.append("/nocluster/");
	mkpathfile(gnuplotPath.c_str(), 0744);
	
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Generating Gnuplot on " << gnuplotPath << endl;
	
	if(parsing==PSO || parsing==POS) {
		int max = dictionary->getMaxID();
		int npredicates = dictionary->getNpredicates();
		vector <string> predicates(npredicates+1);
		
		for (int i = 1; i<=npredicates; i++)
		{
			string p = dictionary->retrieveString(i, VOC_PREDICATE);
			predicates[i] = p;
		}
		triples->gnuplot(npredicates, predicates, max, gnuplotPath.c_str());
	}
	
	// 11) Cluster
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Clustering" << endl;
	triples->clustering();
	
	// 12) Writing Dictionary
	getTime(&t2);
	string dictPath = output;
	dictPath.append("/hdt_cluster");
	cout << "[" << showpoint << t2.user - t1.user << "]  Writing Clustered Dictionary to: " << dictPath << endl;
	dictionary->write(dictPath, header->getD_separator());
	
	// 13) Writing Triples
	//    Returns the number of triples effectively writen (note
	//    that the original dataset can contain repeated triples)
	getTime(&t2);
	string triplePath = output;
	triplePath.append("/hdt_cluster");
	cout << "[" << showpoint << t2.user - t1.user << "]  Writing Clustered Triples to " << triplePath << endl;
	int ntriples = triples->write(triplePath);
	
	// 14) Writing Header
	getTime(&t2);
	string headerPath = output;
	headerPath.append("/hdt_cluster");
	cout << "[" << showpoint << t2.user - t1.user << "]  Writing Clustered Header to " << headerPath << endl;
	
/*	header = new Header(parsing, mapping, separator, t_encoding, 
						dictionary->getNsubjects(), dictionary->getNpredicates(), dictionary->getNobjects(), dictionary->getSsubobj(),
						ntriples, dictionary->getMaxID());*/
	header->write(headerPath);
	
	// 15) Clustered GNUPLOT
	gnuplotPath=output;
	gnuplotPath.append("/cluster/");
	mkpathfile(gnuplotPath.c_str(), 0744);
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Generating Clustered Gnuplot on " << gnuplotPath << endl;
	
	if(parsing==PSO || parsing==POS) {
		int max = dictionary->getMaxID();
		int npredicates = dictionary->getNpredicates();
		vector <string> predicates(npredicates+1);
		
		for (int i = 1; i<=npredicates; i++)
		{
			string p = dictionary->retrieveString(i, VOC_PREDICATE);
			predicates[i] = p;
		}
		triples->gnuplot(npredicates, predicates, max, gnuplotPath.c_str());
	}
#endif
	
	// Dictionary stats
	// 10) In and Out Degree
	getTime(&t2);
	string dictStats(output);
	dictStats.append("/dictstats");
	cout << "[" << showpoint << t2.user - t1.user << "]  Calculate Dictionary Statistics on " << dictStats <<endl;
	mkpathfile((dictStats).c_str(), 0744);
	dictionary->dumpStats(dictStats);
	
#if 0
	// 10) In and Out Degree
	getTime(&t2);
	string degreePath(output);
	degreePath.append("/degree");
	cout << "[" << showpoint << t2.user - t1.user << "]  Calculate Degree Metrics on " << degreePath <<endl;
	mkpathfile((degreePath).c_str(), 0744);
	triples->calculateDegrees(degreePath);
#endif
	
	// All done
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  All done" << endl;
	
	return true;
}

void 
StatsGen::parseTripleN3(string t)
{
	string value = t;
	int pos = 0;
	int firstIndex=0;
	int lastIndex=0;
	bool errorParsing=false;
	
	node[0]="";
	node[1]="";
	node[2]="";
	
	while (true)
	{
		value = value.substr(firstIndex);
		
		if (value=="." || value=="\n" || value == "" || value.at(0)=='#')
			break;
		
		//obvious space 
		if (value.at(0)==' ')
		{
			//do nothing
			lastIndex=0; 
		}
		//URI   
		else if (value.at(0)=='<')
		{
			lastIndex = value.find(">");    
			//check size of pos
			if (pos>2)
			{
				errorParsing=true;  
				break;      
			}
			node[pos] = value.substr(0,lastIndex+1);
			pos++;
		}
		//Literal  
		else if (value.at(0)=='"')
		{
			lastIndex = value.find('"',1);
			//check if literal is escaped
			while (true)
			{
				bool escaped = false;
				int temp = lastIndex-1;
				
				while (temp>0 && value.at(temp)=='\\')
				{
					if (escaped) escaped=false;
					else escaped = true;
					temp--;
				}
				
				if (!escaped) break;
				lastIndex++;
				if (lastIndex==value.length())
					//Cannot find the (unescaped) end
					errorParsing=true; 
				lastIndex = value.find('"',lastIndex);
				if (lastIndex==string::npos)
					//Cannot find the (unescaped) end
					errorParsing=true;        
			} 
			
			// literal can extend to a bit more than just the ",
			// also take into account lang and datatype strings
			if (value.at(lastIndex+1) == '@') 
			{
				// find end of literal/lang tag
				lastIndex = value.find(' ', lastIndex+1) - 1;
			} 
			else if (value.at(lastIndex+1) == '^') 
			{
				lastIndex = value.find('>', lastIndex+1);
			}  
			//check size of pos
			if (pos>2)
			{	
				errorParsing=true;  
				break;      
			} 
			node[pos]= value.substr(0,lastIndex+1);
			pos++;
		}
		//blank, a variable, a relative predicate
		else if (value.at(0)=='_' || (value.find(":")!=string::npos))
		{
			lastIndex = value.find(" ");    
			//check size of pos
			if (pos>2)
			{
				errorParsing=true;  
				break;      
			}  
			node[pos]= value.substr(0,lastIndex);
			pos++;
		}
		//parameter or variable ---> obviate for Triples. In future, add to a Hash
		else if (value.at(0)=='@'|| value.at(0)=='?')
		{
			break;
		}
		// test if number 
		else
		{
			// else it is a parsing error
			lastIndex = value.find(" ");
			for (int j=0;j<lastIndex;j++)
			{
				if (!isdigit(value.at(j))&&value.at(j)!='.'&&value.at(j)!=','){
					errorParsing=true;  
				}
			}
			
			if (errorParsing==false)
			{
				node[pos]= value.substr(0,lastIndex);
				pos++;
			}
			
			break;   
		}
		
		firstIndex = lastIndex+1;
	}
	
	if (errorParsing==true)
	{
		char* except = (char*)" :*********** FORMAT ERROR (NOT N3?) ***********"; 
		throw except;    
	}
	else if (pos!=0 && pos!=3)
	{
		char* except = (char*)" :*********** FORMAT ERROR (NOT N3?) ***********"; 
		throw except;
	}
}


StatsGen::~StatsGen()
{
	delete header;
	delete dictionary;
	delete triples;
}
