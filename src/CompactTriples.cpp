/* CompactTriples.cpp
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
  * Compact encoding (based on adjacency lists) for Triples
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
 
#include "CompactTriples.h"

using namespace std;

CompactTriples::CompactTriples()
{
}

CompactTriples::CompactTriples(Dictionary *dictionary, unsigned int ntriples, unsigned int parsing)
{
	this->dictionary = dictionary;
	this->parsing = parsing;
	this->ntriples = 0;
	
	graph.resize(ntriples);
}

CompactTriples::CompactTriples(Dictionary *dictionary, unsigned int ntriples, unsigned int parsing, string path)
{
	this->dictionary = dictionary;
	this->ntriples = ntriples;
	this->parsing = parsing;
	this->path = path;
}

void
CompactTriples::console()
{
	vector<string> subcommand;
	string command;
	char commandChar[256] ;
	
	string format="ntriples";
	string output="salida.rdf";
	int i=0;
	
	
	cout.precision(4);
	cout << endl << "**************************" << endl;
	cout << "  Compact Triples Console" << endl;
	cout << "  ---------------------" << endl;
	
	while (true){
		format="ntriples";
		output="salida.rdf";
		
		cout <<"  Available Operations: " << endl;
		cout <<"        serialize                      Serialize HDT to RDF (ntriples by default). " << endl;
		cout <<"            -f format                         Output format: ntriples (default), rdfxml, turtle, rdfxml-abbrev, rdfxml-xmp, atom, json-triples, json, dot"<< endl;	
	    cout <<"            -o --output <file>                Serialize output (salida.rdf by default)"<< endl;	
		cout <<"        help                           Show this help"<< endl;	
		cout <<"        exit                           Exit console mode"<< endl;	
	
		cout << "**************************" << endl;
		cout << endl;
		cout<<"\n\n$>";

		cin.getline(commandChar,256);
		command = commandChar;
		
		if (command=="")
			cout<<"\n Please provide one available command.\n\n";
		else
		{
			//split as space separator
			stringExplode(command, " ", &subcommand);
			
			if (subcommand[0]=="serialize"){ 
				
				for (i=1;i<subcommand.size();){
					//verify arguments				
					if (subcommand[i]=="-f"){
						
						i=i+1;
						if (i<(subcommand.size())){
							if (subcommand[i][0]!='-') //other option
							{
								format = subcommand[i];
								if (format!="ntriples" && format!="turtle" && format!="rdfxml-xmp" && format!="rdfxml-abbrev"
									&& format!="rdfxml" && format!="rss-1.0" && format!="atom" && format!="dot"
									&& format!="rss-1.0" && format!="json-triples" && format!="json"){
									cout<<   "<WARNING> The format:"<<format<<" is possibly not allowed by the serializer.\n\n";
								} 
								i=i+1;
							}	
						}
					}
					else if(subcommand[i]=="-o"){
						i=i+1;
						if (i<(subcommand.size())){
							if (subcommand[i][0]!='-') //other option
							{
								output = subcommand[i];
								i=i+1;
							}
						}
					}
					else
						i=i+1;
						
				}
				DataTime t1, t2;		
				getTime(&t1);
				bool ok = serialize((char*)output.c_str(),(char*)format.c_str());
				getTime(&t2);
				if (ok)
				{
					cout << "   <OK> The file '" << output << " has been successfully created in ";
					cout << t2.user - t1.user << " seconds." << endl <<  endl;
				}
			}else if (subcommand[0]=="exit"){ 
				exit(0);
				}
			else if (subcommand[0]=="help"){
				//nothing to do
			}
			else {
				cout<<"\n"<<subcommand[0]<<": command not found.\n\n";
			}
		}
		subcommand.clear();
	}
}

bool
CompactTriples::transformToN3()
{
	unsigned int x = 1, y, z;
	size_t read_y = 0, read_z = 0;
	string subject, predicate, object;
	TripleID triple;
	
	string input = path;
	FILE *fileY = fopen((input.append(".hdt.triples.Y")).c_str(), "r");
	
	input = path;
	FILE *fileZ = fopen((input.append(".hdt.triples.Z")).c_str(), "r");
	
	string output = path;
	output.append(".fromHDT.n3");
	ofstream outFile(output.c_str()); 
	
	unsigned int i = 1;
	
	while (i <= ntriples)
	{
		read_y += fread(&y,INT32,1,fileY);
		
		// Check if the current 'y' value marks (with 0) the end of an
		// adjacency list in X
		if (y == 0)
		{
			// Increment 'x' value and read a new 'y' value
			x++;
			read_y += fread(&y,INT32,1,fileY);
		}
		
		// Retrieve the adjacency list of (x,y)
		read_z += fread(&z,INT32,1,fileZ);
		
		while (z != 0)
		{
			triple = retrieveTriple(x,y,z);
			
			subject = dictionary->retrieveString(triple.x, VOC_SUBJECT);
			predicate = dictionary->retrieveString(triple.y, VOC_PREDICATE);
			object = dictionary->retrieveString(triple.z, VOC_OBJECT);
			
			outFile << subject << " " << predicate << " " << object << " ." << endl;
			
			i++;
			read_z += fread(&z,INT32,1,fileZ);
		}
	}
	
	return true;
}

bool
CompactTriples::serialize(char *output, char *format)
{
	unsigned int x = 1, y, z;
	RDFSyntaxMediator rdfmediator;
	ofstream outFile;
	
	bool n3=true;
	if (strcmp(format,"ntriples")!=0){
		n3=false;
		rdfmediator.init_serialize(output,format);
	}
	else
	{
		outFile.open(output); 
	}
	
	size_t read_y = 0, read_z = 0;
	string subject, predicate, object;
	TripleID triple;
	
	string input = path;
	FILE *fileY = fopen((input.append(".hdt.triples.Y")).c_str(), "r");
	
	input = path;
	FILE *fileZ = fopen((input.append(".hdt.triples.Z")).c_str(), "r");
	
	unsigned int i = 1;
	
	while (i <= ntriples)
	{
		read_y += fread(&y,INT32,1,fileY);
		
		// Check if the current 'y' value marks (with 0) the end of an
		// adjacency list in X
		if (y == 0)
		{
			// Increment 'x' value and read a new 'y' value
			x++;
			read_y += fread(&y,INT32,1,fileY);
		}
		
		// Retrieve the adjacency list of (x,y)
		read_z += fread(&z,INT32,1,fileZ);
		
		while (z != 0)
		{
			triple = retrieveTriple(x,y,z);
			
			subject = dictionary->retrieveString(triple.x, VOC_SUBJECT);
			predicate = dictionary->retrieveString(triple.y, VOC_PREDICATE);
			object = dictionary->retrieveString(triple.z, VOC_OBJECT);
			
			//outFile 
			if (n3==true){
				outFile << subject << " " << predicate << " " << object << " ." << endl;
			}
			else{
				rdfmediator.serialize_statement(subject,predicate,object);
			}
			
			i++;
			read_z += fread(&z,INT32,1,fileZ);
		}
	}
	
	if (n3==true){
		if (outFile.good()) outFile.close();
	}
	else
		rdfmediator.end_serialize();
		
	return true;
}
	
unsigned int 
CompactTriples::write(string path)
{	
	string output = path;
	FILE *fileY = fopen((output.append(".hdt.triples.Y")).c_str(), "w+");
	
	output = path;
	FILE *fileZ = fopen((output.append(".hdt.triples.Z")).c_str(), "w+");
	
	unsigned int repeated = 0;
	unsigned int x = graph[0].x, y = graph[0].y, z = 0;
	
	fwrite(&y,INT32,1,fileY);

	for (unsigned int i=0; i<ntriples; i++)
	{
		if (x == graph[i].x)
		{
			if (y == graph[i].y)
			{
				if (z != graph[i].z)
				{
					z = graph[i].z;
					fwrite(&z,INT32,1,fileZ);
				}
				else
				{
					repeated++;
				}
			}
			else
			{
				// Change of Y coordinate: write a 0 in the stream Z
				y = 0;
				fwrite(&y,INT32,1,fileZ);
				
				y = graph[i].y; z = graph[i].z;
				
				fwrite(&y,INT32,1,fileY);
				fwrite(&z,INT32,1,fileZ);
			}
		}
		else
		{
			// Change of X and Y coordinates: write both 0s in stream Y,Z
			x = 0; y = 0;
			
			fwrite(&x,INT32,1,fileY);
			fwrite(&y,INT32,1,fileZ);
			
			x = graph[i].x; y = graph[i].y; z = graph[i].z;
			
			fwrite(&y,INT32,1,fileY);
			fwrite(&z,INT32,1,fileZ);
		}
	}

	x = 0; fwrite(&x,INT32,1,fileY);
	y = 0; fwrite(&y,INT32,1,fileZ);

	fclose(fileY);
	fclose(fileZ);
	
	return ntriples-repeated;
}


CompactTriples::~CompactTriples()
{
}
