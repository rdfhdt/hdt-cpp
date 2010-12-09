/* RDFParser.cpp
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * HDT Parser from RDF syntaxes to compact representations.
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

#include "RDFParser.h"
#include "fdstream.hpp"

RDFParser::RDFParser()
{
}

bool
hasEnding (std::string const &full, std::string const &ending)
{
    return (full.length() > ending.length()) && (!full.compare (full.length() - ending.length(), ending.length(), ending));
}

bool 
RDFParser::parse(char *pathFile)
{
	DataTime t1, t2;
	getTime(&t1);
	cout.precision(4);
	
	// Parsing the config file
	cout << "[" << showpoint << t1.user - t1.user << "]  Parsing config file: " << pathFile << endl;
	bool parsed = parseConfig(pathFile);
	
	map<string,string>::iterator it;
	string line;
	int ntriples = 0;
	
	// Input File/Pipe management
	FILE *filePipe;
	std::string pipeCommand;
	ifstream *fileStream = NULL; 
	boost::fdistream *pipeStream = NULL;
	
	// RDF Parser
	RDFSyntaxMediator rdfmediator;
	DictionaryTriples dictionaryTriples;

	if (parsed)
	{
		// Performing HDT on the set of properties
		// ***************************************
		
		// 1) Opening the dataset
		it = properties.find(DATASET);
		string &datasetName = it->second;

		if(hasEnding(datasetName, ".gz")) {
			pipeCommand = "gunzip -c ";
		} else if(hasEnding(datasetName, ".bz2")) {
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
			cout << "   <ERROR> Please check the dataset in " << it->second << endl;
			return false;
		}
		
		// 2) Testing dictionary options: it is built on a first pass
		//    which gathers statistics and performs the chosen ordering.
		//    First, the ordering parameter is retrieved and next the
		//    chosen dictionary class is instantiated.
		getTime(&t2);
		cout << "[" << showpoint << t2.user - t1.user << "]  Building Dictionary" << endl;
		
		it = properties.find(D_ORDERING);
		
		if ((it->second).compare(VD_ORDERING_LEX) == 0)
		{
			// ** Lexicographic ordering **
			it = properties.find(D_ENCODING);
			if ((it->second).compare(VPLAIN) == 0)
			{
				// PLAIN dictionary on hash tables: checking (optional) 
				// size configuration.
				it = properties.find(HASH_SIZE);
				
				if (it != properties.end())
				{
					// New plain dictionary with the chosen size.					
					dictionary = new PlainDictionary(atoi((it->second).c_str()));
				}
				else
				{					
					// New plain dictionary with the by default size.
					dictionary = new PlainDictionary();
				}
			}
			else if ((it->second).compare(VD_ENCODING_IND) == 0)
			{
				// INDEXED dictionary (for future)
				cout << "   <WARNING> Indexed Dictionary encoding is not currently supported" << endl;
				return false;
			}
			else
			{
				cout << "   <ERROR> Dictionary encoding " << it->second << " is not supported" << endl;
				return false;
			}
		}
		else
		{
			cout << "   <ERROR> Dictionary ordering " << it->second << " is not supported" << endl;
			return false;
		}
	
		//@javi
		// Checks RDF syntax; only n3 is currently supported.
		
		it = properties.find(RDF_PARSER);
		if ((it != properties.end()) && (it->second).compare(RDF_PARSER_OWN) == 0)
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
			it = properties.find(DATASET);
			
			dictionaryTriples.dictionary = dictionary;
			dictionaryTriples.triples = NULL;
			dictionaryTriples.ntriples= new int();
			*dictionaryTriples.ntriples=0;
			rdfmediator.parsing(const_cast<char*> (it->second.c_str()),dictionaryTriples);
			ntriples = *dictionaryTriples.ntriples;
		}

		dictionary->split();
		
		// 3) Sorting Dictionary (LEXICOGRAPHIC)
		//    Retrieve the id mapping choice
		getTime(&t2);
		cout << "[" << showpoint << t2.user - t1.user << "]  Sorting Dictionary" << endl;
		
		it = properties.find(MAPPING);
		if ((it->second).compare(VMAPPING_1) == 0)
		{
			mapping = MAPPING1;
		} 
		else if ((it->second).compare(VMAPPING_2) == 0)
		{
			mapping = MAPPING2;
		}
		else
		{
			cout << "   <ERROR> Mapping " << it->second << " is not supported" << endl;
			return false;
		}

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

		it = properties.find(PARSING);
		if ((it->second).compare(VPARSING_SPO) == 0)
		{
			parsing = SPO;
		} 
		else if ((it->second).compare(VPARSING_SOP) == 0)
		{
			parsing = SOP;
		}
		else if ((it->second).compare(VPARSING_PSO) == 0)
		{
			parsing = PSO;
		}
		else if ((it->second).compare(VPARSING_POS) == 0)
		{
			parsing = POS;
		}
		else if ((it->second).compare(VPARSING_OSP) == 0)
		{
			parsing = OSP;

			if (mapping == MAPPING1)
			{
				// *********** NOT SUPPORTED
				// Estas dos no las soportamos porque al utilizar este
				// mapeo se rompe la continuidad en la asignaciÃ³n de
				// identificadores para los objetos.
				// ***********
				cout << "   <ERROR> Configuration with mapping = " << parsing << " and parsing = " << parsing << " is not supported" << endl;
				return false;
			}

		}
		else if ((it->second).compare(VPARSING_OPS) == 0)
		{
			parsing = OPS;
			if (mapping == MAPPING1)
			{	
				// *********** NOT SUPPORTED
				// Estas dos no las soportamos porque al utilizar este
				// mapeo se rompe la continuidad en la asignaciÃ³n de
				// identificadores para los objetos.
				// ***********
				cout << "   <ERROR> Configuration with mapping = " << parsing << " and parsing = " << parsing << " is not supported" << endl;
				return false;
			}

		}
		else 
		{
			cout << "   <ERROR> Mapping " << it->second << " is not supported" << endl;
			return false;
		}
				
		it = properties.find(T_ENCODING);
		if ((it->second).compare(VPLAIN) == 0)
		{
			// PLAIN triples
			triples = new PlainTriples(dictionary, ntriples, parsing);
			t_encoding = PLAIN;
		}
		else if ((it->second).compare(VT_ENCODING_COMP) == 0)
		{
			// COMPACT triples	
			triples = new CompactTriples(dictionary, ntriples, parsing);
			t_encoding = COMPACT;
		}
		else if ((it->second).compare(VT_ENCODING_BIT) == 0)
		{
			// BITMAP triples	
			triples = new BitmapTriples(dictionary, ntriples, parsing);
			t_encoding = BITMAP;
		}
		else if ((it->second).compare(VT_ENCODING_K2) == 0)
		{
			// K2TREE triples (for future)
			t_encoding = K2TREE;
			cout << "   <WARNING> K2-tree Triples encoding is not currently supported" << endl;
		}
		else
		{
			cout << "   <ERROR> Triples encoding " << it->second << " is not supported" << endl;
		}

		int subject, predicate, object;
		
		//@javi
		// Checks RDF syntax; only n3 is currently supported.
		
		
		it = properties.find(RDF_PARSER);
		if ((it != properties.end()) && (it->second).compare(RDF_PARSER_OWN) == 0)
		{
			while (getline(*input,line)) 
			{
				parseTripleN3(line);
				
				subject = dictionary->retrieveID(node[0], VOC_SUBJECT);
				predicate = dictionary->retrieveID(node[1], VOC_PREDICATE);
				object = dictionary->retrieveID(node[2], VOC_OBJECT);
				
				triples->insert(subject, predicate, object);
			}	
		} else {
			it = properties.find(DATASET);
			rdfmediator;
			dictionaryTriples.triples = triples;
			rdfmediator.parsing(const_cast<char*> (it->second.c_str()),dictionaryTriples);
			
		}

		
		triples->graphSort();
		
		// 5) Clustering is only available for parsing 'pso'.
		if (parsing == PSO)
		{
			it = properties.find(CLUSTER);
			if (it != properties.end())
			{
				if ((it->second).compare(VYES) == 0)
				{
					getTime(&t2);
					cout << "[" << showpoint << t2.user - t1.user << "]  Clustering" << endl;
					triples->clustering();
				}
			}			
		}
		
		// 6) Writing Dictionary
		getTime(&t2);
		cout << "[" << showpoint << t2.user - t1.user << "]  Writing HDT" << endl;

		string outputPath;
		int d_separator;
		
		it = properties.find(OUTPUT);
		if (it != properties.end())
		{
			outputPath = it->second;
		}
		else
		{
			cout << "   <WARNING> Not found output information -> HDT results stored in current location" << endl;
			outputPath = "./default";
		}
		
		it = properties.find(D_SEPARATOR);
		if (it != properties.end())
		{
			d_separator = atoi((it->second).c_str());
		}
		else
		{
			cout << "   <WARNING> By default '\2' character is used in the Dictionary encoding" << endl;
			d_separator = 2;
		}		
		
		dictionary->write(outputPath, d_separator);
		/*					
			if (property.compare(D_COMPRESSION) == 0)
			{
				d_compression = "";
				if (value.compare(VD_COMPRESSION_PPM) == 0) d_compression = VD_COMPRESSION_PPM;
				if (value.compare(VD_COMPRESSION_LZ) == 0) d_compression = VD_COMPRESSION_LZ;
				if (value.compare(VNONE) == 0) d_compression = VNONE;

				if (d_compression.compare("") == 0)
				{
					cout << "   <ERROR> Dictionary compression " << value << " is not supported" << endl;
					parsed = false;
				}
			}
		*/
		
		// 7) Writing Triples
        //    Returns the number of triples effectively writen (note
        //    that the original dataset can contain repeated triples)
		ntriples = triples->write(outputPath);		
		
		/* CompresiÃ³n de los triples
		 *
			if (property.compare(T_COMPRESSION) == 0)
			{
				t_compression = "";
				if (value.compare(VD_COMPRESSION_HUF) == 0) t_compression = VD_COMPRESSION_HUF;
				if (value.compare(VNONE) == 0) t_compression = VNONE;

				if (t_compression.compare("") == 0)
				{
					cout << "   <ERROR> Triples compression " << value << " is not supported" << endl;
					parsed = false;
				}
			}
		*/	

		
		// 8) Writing Header
		header = new Header();
		header->write(outputPath, parsing, mapping, d_separator, t_encoding, 
					  dictionary->getNsubjects(), dictionary->getNpredicates(), dictionary->getNobjects(), dictionary->getSsubobj(), ntriples);
					  
		// 9) Checking optional gnuplot generation (only for parsings 
		//    'pso' and 'pos')
		if ((parsing == PSO) || (parsing == POS))
		{
			it = properties.find(GNUPLOT_PATH);
			if (it != properties.end())
			{
				getTime(&t2);
				cout << "[" << showpoint << t2.user - t1.user << "]  Processing Gnuplot" << endl;

				int max = dictionary->getMaxID();
				int npredicates = dictionary->getNpredicates();
				vector <string> predicates(npredicates+1);
				
				for (int i = 1; i<=npredicates; i++)
				{
					string p = dictionary->retrieveString(i, VOC_PREDICATE);
					predicates[i] = p;
				}
				triples->gnuplot(npredicates, predicates, max, (it->second).c_str());
			}
		}

		// 10) In and Out Degree
		it = properties.find(DEGREE_PATH);
		if(it != properties.end() && (parsing==SPO)) {
			getTime(&t2);
			cout << "[" << showpoint << t2.user - t1.user << "]  Calculate Degree Metrics" << endl;
		
			triples->calculateDegrees(it->second);
		}
		
		// All done
		getTime(&t2);
		cout << "[" << showpoint << t2.user - t1.user << "]  All done" << endl;
	}
	else
	{
		cout << endl << "   <ERROR> Revised parsing errors" << endl;
		return false;
	}
	
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
RDFParser::parseConfig(char* pathFile)
{
	bool parsed = true;
	
	string line;
	string property;
	string value;
	
	map<string,string>::iterator it;
	
	config.open(pathFile, ifstream::in);
	
	if (config.good())
	{		
		// Load a map with all pair property-value
		while (getline(config,line))
		{
			if (line.length() > 0)
			{
				size_t pos=line.find("=");
				
				if (pos != string::npos)
				{			
					// Read property-value
					property = line.substr(0,pos);
					value = line.substr(pos+1);
					
					// Remove spaces
					property.erase(remove_if(property.begin(), property.end(), isblank), property.end());
					value.erase(remove_if(value.begin(), value.end(), isblank), value.end());
					
					it = properties.begin();
					properties.insert(it, pair<string,string>(property,value));
				}
			}
		}
		
		parsed = true;			
	}
	else
	{
		cout << "   <ERROR> Checks the config file in " << pathFile << endl;
		parsed = false;
	}

	return parsed;
}

void 
RDFParser::parseTripleN3(string t)
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


RDFParser::~RDFParser()
{
	delete header;
	delete dictionary;
	delete triples;
}
