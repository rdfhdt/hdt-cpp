/* BitmapTriples.cpp
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * Bitmap-based encoding for Triples
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
 
 
// DEBERIAMOS HACE 6 IMPLEMENTACIONES DIFERENTES DE ESTA CLASE, UNA PARA
// CADA TIPO DE PARSING. CON ESO NOS EVITARÍAMOS TODAS LAS COMPARACIONES
// QUE TENEMOS QUE HACER SIEMPRE Y QUE EN UN ENTORNO REAL DE PRUEBA
// DEGRADARÍA NUESTRA EFICIENCIA. AHORA LO DEJO ASÍ PARA QUE SEA MÁS
// SENCILLO DE VER, PERO HAY QUE CAMBIARLO!
 
#include "BitmapTriples.h"

using namespace std;

BitmapTriples::BitmapTriples()
{
}

BitmapTriples::BitmapTriples(Dictionary *dictionary, unsigned int ntriples, unsigned int parsing)
{
	this->dictionary = dictionary;
	this->parsing = parsing;
	this->ntriples = 0;
	
	graph.resize(ntriples);
}

BitmapTriples::BitmapTriples(Dictionary *dictionary, unsigned int ntriples, unsigned int parsing, string path)
{
	this->dictionary = dictionary;
	this->ntriples = ntriples;
	this->parsing = parsing;
	this->path = path;
	
	string input = path;
	fileY = fopen((input.append(".hdt.triples.Y")).c_str(), "r");
	
	input = path;
	fileZ = fopen((input.append(".hdt.triples.Z")).c_str(), "r");
	
	input = path;
	ifstream fileBY((input.append(".hdt.triples.BY")).c_str());
	
	input = path;
	ifstream fileBZ((input.append(".hdt.triples.BZ")).c_str());
	
	if (fileBY.good())
	{
		bitmapY = BitSequence::load(fileBY);
		fileBY.close();
	}
	
	if (fileBZ.good())
	{
		bitmapZ = BitSequence::load(fileBZ);
		fileBZ.close();
	}
}

void
BitmapTriples::console()
{
	cout.precision(4);
	vector<string> subcommand;
	string command;
	char commandChar[256] ;
	
	string format="ntriples";
	string output="output.rdf";
	int i=0;
	
	
	cout.precision(4);
	cout << endl << "**************************" << endl;
	cout << "  Bitmap Triples Console" << endl;
	cout << "  ---------------------" << endl;
	
	while (true){
		format="ntriples";
		output="salida.rdf";
		
		cout <<"  Available Operations: " << endl;
		cout <<"        serialize                      Serialize HDT to RDF (ntriples by default). " << endl;
		cout <<"            -f format                         Output format: ntriples (default), rdfxml, turtle, rdfxml-abbrev, rdfxml-xmp, atom, json-triples, json, dot"<< endl;	
	    cout <<"            -o --output <file>                Serialize output (output.rdf by default)"<< endl;
	    cout <<"        ask <BGP>                      SPARQL Ask operation over a given Basic Graph Pattern"<< endl;
		/*cout <<"        construct <BGP>            SPARQL construct operation over a given Basic Graph Pattern. "<< endl;
		cout <<"            -f format                         Output format: ntriples (default), rdfxml, turtle, rdfxml-abbrev, rdfxml-xmp, atom, json-triples, json, dot"<< endl;	
	    cout <<"            -o --output <file>                construct output (salida.rdf by default)"<< endl;
	    */
		cout <<"        select <BGP>                   SPARQL select operation over a given Basic Graph Pattern"<< endl;
	    cout <<"        vocab                          Show vocabulary stats (RDFS,OWL)"<< endl;	
	    cout <<"        help                           Show this help"<< endl;	
		cout <<"        exit                           Exit console mode"<< endl;		
		cout <<"   -----------" << endl;
		   	
		if (parsing == SPO) 
			cout << "    Due to the current SPO parsing, <BGP> must be <S P O> or <S ?P O?> or <S P ?O>" << endl;
		else if (parsing == SOP)
			cout << "    Due to the current SOP parsing, <BGP> must be <S P O> or <S ?P O?> or <S P ?O>" << endl;
		else if (parsing == POS) 
			cout << "    Due to the current POS parsing, <BGP> must be <S P O> or <?S P O?> or <?S P O>" << endl;
		else if (parsing == PSO)
			cout << "    Due to the current PSO parsing, <BGP> must be <S P O> or <?S P O?> or <?S P O> " << endl;
		else if (parsing == OSP)
			cout << "    Due to the current OSP parsing, <BGP> must be <S P O> or <?S ?P O> or <S ?P O>" << endl;
		else if (parsing == OPS)
			cout << "    Due to the current OPS parsing, <BGP> must be <S P O> or <?S ?P O> or <S ?P O>" << endl;
	
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
			}else if (subcommand[0]=="ask"){ 
				if (subcommand.size()>=4){
					DataTime t1, t2;
					string q; 
					q.append(subcommand[1]);
					q.append(" ");
					q.append(subcommand[2]);
					q.append(" ");
					q.append(subcommand[3]);
					q.append(" .");
					
					//example: q = "?y <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2006/vcard/ns#Name> .";
					
					getTime(&t1);
					bool ok = ask(q);
					getTime(&t2);
					
					if (ok) 
						cout<< endl << "   " << q << " EXISTS in the dataset";
					else 
						cout<< endl << "   " << q << " NOTEXISTS in the dataset";
					cout << " (" << t2.user - t1.user << " secs.)" << endl <<  endl;
				}
				else{
					cout<<"\n Please provide one available BGP.\n\n";
				}
				
				
			}else if (subcommand[0]=="select"){ 
				if (subcommand.size()>=4){
					DataTime t1, t2;
					vector<TripleString> *r;
					string q; 
					q.append(subcommand[1]);
					q.append(" ");
					q.append(subcommand[2]);
					q.append(" ");
					q.append(subcommand[3]);
					q.append(" .");
					
					//example: q = "?y <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2006/vcard/ns#Name> .";
					
					getTime(&t1);
					int ok = select(q,&r);
					getTime(&t2);
					
					if (ok != ERROR)
					{
						cout << "   " << q << " occurs " << ok << " times "<<  endl;

						// first 100 results
						if (ok > 100){
							 ok =100;
							 cout << "   showing the first 100 results:"<<  endl;
						 }
						
						for (uint i=0; i<ok; i++)
						{
							cout << "[" << i+1 << "] ";
							cout << (*r)[i].x << " ";
							cout << (*r)[i].y << " ";
							cout << (*r)[i].z << " ." << endl;
						}
					}
					else 
						cout<< endl << "   " << q << " NOTEXISTS in the dataset";
					cout << " (" << t2.user - t1.user << " secs.)" << endl <<  endl;
				}
				else{
					cout<<"\n Please provide a available BGP.\n\n";
				}
			}else if (subcommand[0]=="exit"){ 
				exit(0);
				}
			else if (subcommand[0]=="vocab"){
				vocabStats();
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
	
	
	
	
	

	
	/*
	
	
	getTime(&t1);
	ok = select(q,&r);
	getTime(&t2);
	
	if (ok != ERROR)
	{
		cout << "   " << q << " occurs " << ok << " times ";
		cout << "(" << t2.user - t1.user << " secs.)" << endl <<  endl;

		// Mostramos solo los 15 primeros
		if (ok > 15) ok =15;
		
		for (uint i=0; i<ok; i++)
		{
			cout << "[" << i+1 << "] ";
			cout << (*r)[i].x << " ";
			cout << (*r)[i].y << " ";
			cout << (*r)[i].z << " ." << endl;
		}
	}
		
	getTime(&t2);
	* */
}

Query
BitmapTriples::parseQ(string q)
{
	TripleString qstring = parseQuery(q);
	Query qid;
	qid.variables = 0;
	
	// Transforms strings in IDs: zero IDs are progressively assigned 
	// to each variable.
	// ...
	// Checks the value restricted by the chosen parsing
	// The ID values are organizad in the same parsing order.
	
	switch (parsing)
	{
		case SPO:
			if (qstring.x[0] != '?')
			{
				qid.q.x = dictionary->retrieveID(qstring.x, VOC_SUBJECT);
				
				
				if (qstring.y[0] != '?') 
				{
					qid.q.y = dictionary->retrieveID(qstring.y, VOC_PREDICATE);
				}
				else
				{
					qid.q.y = 0;
					qid.variables++;
				}

				if (qstring.z[0] != '?')
				{
					if (qid.variables == 0)
						qid.q.z = dictionary->retrieveID(qstring.z, VOC_OBJECT);
					else
						qid.variables = ERROR;	
				}
				else
				{
					qid.q.z = 0;
					qid.variables++;
				}
			}
			else
				qid.variables = ERROR;
				
				
			break;
		case SOP:
			if (qstring.x[0] != '?')
			{
				qid.q.x = dictionary->retrieveID(qstring.x, VOC_SUBJECT);
			
				if (qstring.z[0] != '?')
				{
					qid.q.y = dictionary->retrieveID(qstring.z, VOC_OBJECT);
				}
				else
				{
					qid.q.y = 0;
					qid.variables++;
				}
			
				if (qstring.y[0] != '?')
				{
					if (qid.variables == 0)
						qid.q.z = dictionary->retrieveID(qstring.y, VOC_PREDICATE);
					else
						qid.variables = ERROR;	
				}
				else
				{
					qid.q.z = 0;
					qid.variables++;
				}
			}
			else
				qid.variables = ERROR;

			break;
		case PSO:
			if (qstring.y[0] != '?')
			{
				qid.q.x = dictionary->retrieveID(qstring.y, VOC_PREDICATE);
				
				if (qstring.x[0] != '?')
				{
					qid.q.y = dictionary->retrieveID(qstring.x, VOC_SUBJECT);
				}
				else
				{
					qid.q.y = 0;
					qid.variables++;
				}

				if (qstring.z[0] != '?')
				{
					if (qid.variables == 0)
						qid.q.z = dictionary->retrieveID(qstring.y, VOC_OBJECT);
					else
						qid.variables = ERROR;
				}
				else
				{
					qid.q.z = 0;
					qid.variables++;
				}
			}
			else
				qid.variables = ERROR;
			
			break;
		case POS:
			if (qstring.y[0] != '?')
			{
				qid.q.x = dictionary->retrieveID(qstring.y, VOC_PREDICATE);
				
				if (qstring.z[0] != '?')
				{
					qid.q.y = dictionary->retrieveID(qstring.z, VOC_OBJECT);
				}
				else
				{
					qid.q.y = 0;
					qid.variables++;
				}
				
				if (qstring.x[0] != '?')
				{
					if (qid.variables == 0)
						qid.q.z = dictionary->retrieveID(qstring.x, VOC_SUBJECT);
					else
						qid.variables = ERROR;
				}
				else
				{
					qid.q.z = 0;
					qid.variables++;
				}
			}
			else
				qid.variables = ERROR;
					
			break;
		case OSP:
			if (qstring.z[0] != '?')
			{
				qid.q.x = dictionary->retrieveID(qstring.z, VOC_OBJECT);

				if (qstring.x[0] != '?')
				{
					qid.q.y = dictionary->retrieveID(qstring.x, VOC_SUBJECT);
				}
				else
				{
					qid.q.y = 0;
					qid.variables++;
				}
				
				if (qstring.y[0] != '?')
				{
					if (qid.variables == 0)
						qid.q.z = dictionary->retrieveID(qstring.y, VOC_PREDICATE);
					else
						qid.variables = ERROR;
				}
				else
				{
					qid.q.z = 0;
					qid.variables++;
				}
			}
			else
				qid.variables = ERROR;

			break;
		case OPS:
			if (qstring.z[0] != '?')
			{
				qid.q.x = dictionary->retrieveID(qstring.z, VOC_OBJECT);

				if (qstring.y[0] != '?')
				{
					qid.q.y = dictionary->retrieveID(qstring.y, VOC_PREDICATE);
				}
				else
				{
					qid.q.y = 0;
					qid.variables++;
				}

				if (qstring.x[0] != '?')
				{
					if (qid.variables == 0)
						qid.q.z = dictionary->retrieveID(qstring.x, VOC_SUBJECT);
					else
						qid.variables = ERROR;
				}
				else
				{
					qid.q.z = 0;
					qid.variables++;
				}
			}
			else
				qid.variables = ERROR;
						
			break;
		default: 
			cout << "   <ERROR> Parsing " << parsing << " is not supported" << endl;
	}
	
	return qid;
}

bool 
BitmapTriples::ask(string q)
{	
	bool checkf = false;
	Query query = parseQ(q);
	
	if (query.q.x==ERROR){
		cout<<"  - First element was not found in the dictionary:"<<endl;
	}
	if (query.q.y==ERROR){
		cout<<"  - Second element was not found in the dictionary:"<<endl;
	}
	if (query.q.z==ERROR){
		cout<<"  - Third element was not found in the dictionary:"<<endl;
	}
	if ((query.q.x==ERROR)||(query.q.y==ERROR)||(query.q.z==ERROR)){
		return false;
	}
	
	if (query.variables != ERROR)
	{		
		unsigned int beginY = bitmapY->select1(query.q.x);
		unsigned int endY = bitmapY->select1(query.q.x+1);
		unsigned int sizeY = endY-beginY-1;
	
		if (sizeY > 0)
		{
			if (query.variables == 2)
			{
				// In this point we guarantee that exist at least one triple
				// (X, ?y, ?z) -> return true
				checkf = true;
			}
			else
			{
				fseek(fileY, (beginY-query.q.x+1)*INT32, SEEK_SET);
				unsigned int *bufferY = new unsigned int[sizeY];
				size_t s = fread(bufferY, INT32, sizeY, fileY);			
				unsigned int *ptr = lower_bound(bufferY, bufferY+sizeY, query.q.y);
				
				if ((ptr != NULL) && (*ptr == query.q.y))
				{				
					if (query.variables == 1)
					{					
						// In this point we guarantee that exist at least one 
						// triple (X, Y, ?z) -> return true
						checkf = true;
					}
					else
					{
						unsigned int pY = bitmapY->rank0(beginY) + (ptr-bufferY) + 1;
						unsigned int beginZ = bitmapZ->select1(pY)-pY+1;
						unsigned int endZ = bitmapZ->select1(pY+1)-pY;
						unsigned int sizeZ = endZ-beginZ;
						
						fseek(fileZ, beginZ*INT32, SEEK_SET);
						unsigned int *bufferZ = new unsigned int[sizeZ];
						s = fread(bufferZ, INT32, sizeZ, fileZ);					
						ptr = lower_bound(bufferZ, bufferZ+sizeZ, query.q.z);
						
						if ((ptr != NULL) && (*ptr == query.q.z))
						{
							// The triple (X, Y, Z) exists in the dataset
							checkf = true;
						}
						 
						delete [] bufferZ;
					}
				}
				
				delete [] bufferY;
			}
		}
	}
	else
	{
		cout << "   <WARNING> This query is not currently supported: we are currently working!" << endl;
	}
	
	return checkf;
}

int 
BitmapTriples::construct(string q)
{
	// Esto hay que hacerlo de tal manera que no devuelva un resultado
	// plano sino un grafo en HDT (Proyecto de Alberto San Martin)
	// Ahora mismo la función "checkAndFindConstruct" no está resuelta
	// utilizo SELECT para dar la sensación de resultado :D
	// Igualmente SELECT ahora saca por pantalla el resultado, pero
	// tendríamos que devolverlo en alguna estructura de datos más o 
	// menos eficiente.
	vector<TripleString> *triples;
	
	select(q,&triples);
}

	
	
int
BitmapTriples::select(string q, vector<TripleString> **triples)
{	
	uint results = 0;
	Query query = parseQ(q);
	
	if (query.q.x==ERROR){
		cout<<"  - First element was not found in the dictionary:"<<endl;
	}
	if (query.q.y==ERROR){
		cout<<"  - Second element was not found in the dictionary:"<<endl;
	}
	if (query.q.z==ERROR){
		cout<<"  - Third element was not found in the dictionary:"<<endl;
	}
	if ((query.q.x==ERROR)||(query.q.y==ERROR)||(query.q.z==ERROR)){
		return 0;
	}
	
	if (query.variables != ERROR)
	{		
		vector<TripleString> *vresult;

		unsigned int beginY = bitmapY->select1(query.q.x);
		unsigned int endY = bitmapY->select1(query.q.x+1);
		int sizeY = endY-beginY-1;
		
		if (sizeY > 0)
		{			
			fseek(fileY, (beginY-query.q.x+1)*INT32, SEEK_SET);
			unsigned int *bufferY = new unsigned int[sizeY];
			size_t s = fread(bufferY, INT32, sizeY, fileY);
			
			unsigned int pY, beginZ, endZ;
			int sizeZ;
			
			if (query.variables == 1)
			{
				// Retrieving the coordinate Y
				unsigned int *ptr = lower_bound(bufferY, bufferY+sizeY, query.q.y);
				
				if ((ptr != NULL) && (*ptr == query.q.y))
				{
					pY = bitmapY->rank0(beginY) + (ptr-bufferY) + 1;
					beginZ = bitmapZ->select1(pY)-pY+1;
					endZ = bitmapZ->select1(pY+1)-pY;
					sizeZ = endZ-beginZ;
							
					bufferY[0] = *ptr;
					sizeY = 1;
				}
				else
				{
					return results;
				}
			}
			else
			{
				// Retrieving all coordinates Y
				pY = bitmapY->rank0(beginY) + 1;
				beginZ = bitmapZ->select1(pY)-pY+1;
				endZ = bitmapZ->select1(pY+sizeY)-(pY+sizeY-1);
				sizeZ = endZ-beginZ;
			}
			
			// Retrieving coordinates Z
			fseek(fileZ, beginZ*INT32, SEEK_SET);
			unsigned int *bufferZ = new unsigned int[sizeZ];
			s = fread(bufferZ, INT32, sizeZ, fileZ);					
			
			// Building a result table
			results = sizeZ;
			vresult = new vector<TripleString>(results);
			
			string subject, object, predicate;
			uint i=0, readZ=0;

			if (parsing <= SOP) subject = dictionary->retrieveString(query.q.x, VOC_SUBJECT);
			else if (parsing <= POS) predicate = dictionary->retrieveString(query.q.x, VOC_PREDICATE);
			else object = dictionary->retrieveString(query.q.x, VOC_OBJECT);
			
			for (uint y=1; y<=sizeY; y++)
			{
				if ((parsing == PSO) || (parsing == OSP)) subject = dictionary->retrieveString(bufferY[y-1], VOC_SUBJECT);
				else if ((parsing == SPO) || (parsing == OPS)) predicate = dictionary->retrieveString(bufferY[y-1], VOC_PREDICATE);
				else object = dictionary->retrieveString(bufferY[y-1], VOC_OBJECT);
				
				endZ = bitmapZ->select1(pY+y)-(pY+y-1);			
				sizeZ = endZ-beginZ;
				
				for (uint z=1; z<=sizeZ; z++)
				{
					if ((parsing == POS) || (parsing == OPS)) subject = dictionary->retrieveString(bufferZ[readZ], VOC_SUBJECT);
					else if ((parsing == SOP) || (parsing == OSP)) predicate = dictionary->retrieveString(bufferZ[readZ], VOC_PREDICATE);
					else object = dictionary->retrieveString(bufferZ[readZ], VOC_OBJECT);
					
					(*vresult)[readZ].x = subject;
					(*vresult)[readZ].y = predicate;
					(*vresult)[readZ].z = object;

					readZ++;
				}
			
				beginZ = endZ;
			}

			delete [] bufferZ;		
		}

		*triples = vresult;
	}
	else
	{
		results = ERROR;
		cout << "   <WARNING> This query is not currently supported: we are currently working!" << endl;
	}

	return results;
}

bool
BitmapTriples::transformToN3()
{	
	unsigned int x = 1, y, z;
	size_t read_y = 0, read_z = 0;
	string subject, predicate, object;
	TripleID triple;
	
	string output = path;
	output.append(".fromHDT.n3");
	ofstream outFile(output.c_str()); 
	
	// Variable for traversing the bitmaps
	unsigned int xL=0, xR=0, yL=0, yR=0;	
	unsigned int yy=1, zz=1, cy, cz;
	
	while (zz <= ntriples)
	{
		unsigned int begin = yy;
		
		// Processing a new X:
		xR = bitmapY->select1(x+1);
		cy = xR-xL-1;
		
		unsigned int *Vy = new unsigned int[cy];
		read_y += fread(Vy, INT32, cy, fileY);
		yy += cy;
		
		// Processing the set of Ys in X:
		yR = bitmapZ->select1(yy);
		cz = yR-yL-cy;
		
		unsigned int *Vz = new unsigned int[cz];
		read_z += fread(Vz, INT32, cz, fileZ);
		zz += cz;
		
		// Read values for all pairs (x,y)
		unsigned int travL;
		unsigned int travR = yL;		
		unsigned int count = 0;
		
		for (int i=0; i<cy; i++)
		{
			travL = travR;
			travR = bitmapZ->select1(begin+i+1);
			
			for (unsigned int j=0; j<travR-travL-1; j++)
			{
				triple = retrieveTriple(x,Vy[i],Vz[count+j]);
			
				subject = dictionary->retrieveString(triple.x, VOC_SUBJECT);
				predicate = dictionary->retrieveString(triple.y, VOC_PREDICATE);
				object = dictionary->retrieveString(triple.z, VOC_OBJECT);
		
				outFile << subject << " " << predicate << " " << object << " ." << endl;
			}
			
			count += travR-travL-1;
		}
		
		x++;
		xL = xR;
		yL = yR;
		
		delete [] Vy;
		delete [] Vz;
	}
	
	return true;	
}

bool
BitmapTriples::serialize(char *output, char *format)
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
	
	// Variable for traversing the bitmaps
	unsigned int xL=0, xR=0, yL=0, yR=0;	
	unsigned int yy=1, zz=1, cy, cz;
	
	while (zz <= ntriples)
	{
		unsigned int begin = yy;
		
		// Processing a new X:
		xR = bitmapY->select1(x+1);
		cy = xR-xL-1;
		
		unsigned int *Vy = new unsigned int[cy];
		read_y += fread(Vy, INT32, cy, fileY);
		yy += cy;
		
		// Processing the set of Ys in X:
		yR = bitmapZ->select1(yy);
		cz = yR-yL-cy;
		
		unsigned int *Vz = new unsigned int[cz];
		read_z += fread(Vz, INT32, cz, fileZ);
		zz += cz;
		
		// Read values for all pairs (x,y)
		unsigned int travL;
		unsigned int travR = yL;		
		unsigned int count = 0;
		
		for (int i=0; i<cy; i++)
		{
			travL = travR;
			travR = bitmapZ->select1(begin+i+1);
			
			for (unsigned int j=0; j<travR-travL-1; j++)
			{
				triple = retrieveTriple(x,Vy[i],Vz[count+j]);
			
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
			}
			
			count += travR-travL-1;
		}
		
		x++;
		xL = xR;
		yL = yR;
		
		delete [] Vy;
		delete [] Vz;
	}
	
	if (n3==true){
		if (outFile.good()) outFile.close();
	}
	else
		rdfmediator.end_serialize();
	
	return true;	
}
	
void
BitmapTriples::vocabStats()
{	
	unsigned int x = 1, y, z;
	size_t read_y = 0, read_z = 0;
	string subject, predicate, object;
	TripleID triple;
	
	string rdfs="http://www.w3.org/2000/01/rdf-schema#";
	string rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#";
	string owl="http://www.w3.org/2002/07/owl#";
	
	string rdf_type = "<http://www.w3.org/1999/02/22-rdf-syntax-ns#type>";
	
	string subClass="<http://www.w3.org/2000/01/rdf-schema#subClassOf>";
	string subProp="<http://www.w3.org/2000/01/rdf-schema#subPropertyOf>";
	
	vector<MINIPAIR> triplesSubClass;
	vector<MINIPAIR> triplesSubProp;
	
	MiniHashTable countVocab;
	countVocab.inithashtable((ntriples/10)+1,true);//ten per cent of number of triples(1 minimum)
	
	int pos_separator,i;
	string substring="";
	
		
	// Variable for traversing the bitmaps
	unsigned int xL=0, xR=0, yL=0, yR=0;	
	unsigned int yy=1, zz=1, cy, cz;
	
	while (zz <= ntriples)
	{
		unsigned int begin = yy;
		
		// Processing a new X:
		xR = bitmapY->select1(x+1);
		cy = xR-xL-1;
		
		unsigned int *Vy = new unsigned int[cy];
		read_y += fread(Vy, INT32, cy, fileY);
		yy += cy;
		
		// Processing the set of Ys in X:
		yR = bitmapZ->select1(yy);
		cz = yR-yL-cy;
		
		unsigned int *Vz = new unsigned int[cz];
		read_z += fread(Vz, INT32, cz, fileZ);
		zz += cz;
		
		// Read values for all pairs (x,y)
		unsigned int travL;
		unsigned int travR = yL;		
		unsigned int count = 0;
		
		for (int i=0; i<cy; i++)
		{
			travL = travR;
			travR = bitmapZ->select1(begin+i+1);
			
			for (unsigned int j=0; j<travR-travL-1; j++)
			{
				triple = retrieveTriple(x,Vy[i],Vz[count+j]);
			
				//subject = dictionary->retrieveString(triple.x, VOC_SUBJECT);
				predicate = dictionary->retrieveString(triple.y, VOC_PREDICATE);
				//object = dictionary->retrieveString(triple.z, VOC_OBJECT);
			
				
				pos_separator = predicate.find(rdf);
				
				if (pos_separator>0)
				{
					countVocab.hashupdate((char*)predicate.c_str()); //update or insert
				}
				else{
					pos_separator = predicate.find(rdfs);
					if (pos_separator>0)
					{
						countVocab.hashupdate((char*)predicate.c_str()); //update or insert
						
						//check for subclass, suproperty
						if (predicate==subClass){
							MINIPAIR pairFind;
							pairFind.source = triple.x;
							pairFind.target = triple.z;
							triplesSubClass.push_back(pairFind);
						}
						else if (predicate == subProp){
							MINIPAIR pairFind;
							pairFind.source = triple.x;
							pairFind.target = triple.z;
							triplesSubProp.push_back(pairFind);
						}
					}
					else{
						pos_separator = predicate.find(owl);
						if (pos_separator>0)
						{
							countVocab.hashupdate((char*)predicate.c_str()); //update or insert
						}
					}
				}
				
			}
			
			count += travR-travL-1;
		}
		
		x++;
		xL = xR;
		yL = yR;
		
		delete [] Vy;
		delete [] Vz;
	}
	
	//iterate over hash and print results
	
	cout<<"    +List of Vocab Predicates:\n";
	MINIHASHREC* predicateRetrieve;
	int countRDF=0, countRDFS=0,countOWL=0, countRDF_type=0;
	for (i=0;i<countVocab.TSIZE;i++)
	{
		predicateRetrieve = countVocab.get(i);
		if (predicateRetrieve!=NULL){
			cout<<"       " << predicateRetrieve->word<<" ; " << predicateRetrieve->id <<"\n"; 
			predicate = predicateRetrieve->word;
			pos_separator = predicate.find(rdf);
			if (pos_separator>0)
			{
				countRDF+= predicateRetrieve->id ;
				if (predicate==rdf_type){
					countRDF_type+= predicateRetrieve->id ;
				}
			}
			else{
				pos_separator = predicate.find(rdfs);
				if (pos_separator>0)
				{
					countRDFS+= predicateRetrieve->id ;
				}
				else{
					pos_separator = predicate.find(owl);
					if (pos_separator>0)
					{
						countOWL+= predicateRetrieve->id ;
					}
				}
			}
			
			//verify next
			while (predicateRetrieve->next!=NULL)
			{
				predicateRetrieve = predicateRetrieve->next;
				cout<<"       " << predicateRetrieve->word<<" ; " << predicateRetrieve->id <<"\n"; 
				predicate = predicateRetrieve->word;
				pos_separator = predicate.find(rdf);
				if (pos_separator>0)
				{
					countRDF+= predicateRetrieve->id ;
					if (predicate==rdf_type){
						countRDF_type+= predicateRetrieve->id ;
					}
				}
				else{
					pos_separator = predicate.find(rdfs);
					if (pos_separator>0)
					{
						countRDFS+= predicateRetrieve->id ;
					}
					else{
						pos_separator = predicate.find(owl);
						if (pos_separator>0)
						{
							countOWL+= predicateRetrieve->id ;
						}
					}
				}
			}	
		}
	}
	
	cout<<"    +Count of Vocab Predicates:\n";
	cout<<"       +RDF (not type):"<<countRDF-countRDF_type<<"\n";
	cout<<"          -ratio(rdf/ntriples):"<<(float)(countRDF-countRDF_type)/ntriples<<"\n";
	cout<<"       +RDF:type:"<<countRDF_type<<"\n";
	cout<<"          -ratio(rdf:type/ntriples):"<<(float)countRDF_type/ntriples<<" (rdftype/ntriples"<<countRDF_type<<" of them are rdf:type)\n";
	cout<<"       +RDFS:"<<countRDFS<<"\n";
	cout<<"          -ratio(rdfs/ntriples):"<<(float)countRDFS/ntriples<<"\n";
	cout<<"       +OWL:"<<countOWL<<"\n";
	cout<<"          -ratio(owl/ntriples):"<<(float)countOWL/ntriples<<"\n";
	cout<<"    +Total triples: "<<ntriples<<"\n";
	cout<<"    +Different Predicates: "<<dictionary->getNpredicates()<<"\n\n\n";
	
	//check subprop and subclass
	cout<<"    +Count of Subclass Predicates (would crash with cycles):\n";
	
	if (triplesSubClass.size()>0){
		sort(triplesSubClass.begin(),triplesSubClass.end(), BitmapTriples::pairSort);
	
		
		
		MiniHashTable subclassLengths;
		subclassLengths.inithashtable(triplesSubClass.size());
		int length=0, maxlength=0, totallength=0, minlength=500000,numelements=0; 
		
				
		for (i=0;i< triplesSubClass.size();i++){
			if (i ==0){			
				length = getLength(&triplesSubClass,triplesSubClass[i].source,&subclassLengths);
				//cout<<triplesSubClass[i].source <<" length:"<< length<<endl;	
				numelements++;		
			}else if (triplesSubClass[i].source!=triplesSubClass[i-1].source){
				length = getLength(&triplesSubClass,triplesSubClass[i].source,&subclassLengths);
				//cout<<triplesSubClass[i].source <<" length:"<< length<<endl;			
				numelements++;
			}
			if (length>maxlength)
				maxlength=length;
			if (length<minlength)
				minlength=length;
			totallength += length;
		}
		cout<<"       -Max length:"<<maxlength<<"\n";
		cout<<"       -Min length:"<<minlength<<"\n";
		cout<<"       -Mean length:"<<(float)totallength/numelements<<"\n";

		
	}
	cout<<"    +Count of SubProp Predicates (would crash with cycles):\n";
	if (triplesSubProp.size()>0){
		sort(triplesSubProp.begin(),triplesSubProp.end(), BitmapTriples::pairSort);
		int length=0, maxlength=0, totallength=0, minlength=500000,numelements=0; 
		MiniHashTable subpropLengths;
		subpropLengths.inithashtable(triplesSubProp.size());
		for (i=0;i< triplesSubProp.size();i++){
			if (i ==0){			
				length = getLength(&triplesSubProp,triplesSubProp[i].source,&subpropLengths);
				//cout<<triplesSubClass[i].source <<" length:"<< length<<endl;	
				numelements++;		
			}else if (triplesSubProp[i].source!=triplesSubProp[i-1].source){
				length = getLength(&triplesSubProp,triplesSubProp[i].source,&subpropLengths);
				//cout<<triplesSubClass[i].source <<" length:"<< length<<endl;			
				numelements++;
			}
			if (length>maxlength)
				maxlength=length;
			if (length<minlength)
				minlength=length;
			totallength += length;
		}
		cout<<"       -Max length:"<<maxlength<<"\n";
		cout<<"       -Min length:"<<minlength<<"\n";
		cout<<"       -Mean length:"<<(float)totallength/numelements<<"\n";
	}
	
	
}	

int 
BitmapTriples::getLength(vector<minipair> * triplesSub,int current, MiniHashTable *subLengths){

	//cout<<"Entering getlength for:"<<current<<endl;
	//fflush(stdout);
	unsigned int first=0;
	unsigned int next = 0;
	string firstString="",nextString="";
	int pos = FindTransition(*triplesSub,current); 	//binary search
	int maxlength=0, length=0 ;
	MINIHASHREC *recordRetrieve;
	if (pos<0) 
		return 0; //no transition
	while (pos<triplesSub->size()){
		first = triplesSub->at(pos).source;
		if (first!=current)
			break;
		next = triplesSub->at(pos).target;
		//cout<<"next:"<<next<<endl;
		//fflush(stdout);
		sprintf((char*)firstString.c_str(),"%d",first) ;
		sprintf((char*)nextString.c_str(),"%d",next) ;
		recordRetrieve = subLengths->hashsearch((char*)nextString.c_str());
		if (recordRetrieve!=NULL){
			length = recordRetrieve->id;
		}
		else{
			length = getLength(triplesSub,next,subLengths)+1;
		}
		if (length>maxlength){
			subLengths->hashinsert((char*)firstString.c_str(),length);
			maxlength = length;
		}
		pos++;
	}
	return maxlength;
}
	
unsigned int 
BitmapTriples::write(string path)
{	
	string output = path;
	fileY = fopen((output.append(".hdt.triples.Y")).c_str(), "w+");
	
	output = path;
	fileZ = fopen((output.append(".hdt.triples.Z")).c_str(), "w+");
	
	unsigned int repeated = 0;
	unsigned int x = graph[0].x, y = graph[0].y, z = 0;
	
	unsigned int ys = 1, zs = 0;
	unsigned int tys = 0, tzs = 0;
	vector<unsigned int> v_ys, v_zs;
	
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
					zs++;
				}
				else
				{
					repeated++;
				}
			}
			else
			{
				// Change of Y coordinate
				y = graph[i].y; z = graph[i].z;
				
				fwrite(&y,INT32,1,fileY);
				fwrite(&z,INT32,1,fileZ);
				
				ys++;
				v_zs.push_back(zs);
				tzs += zs; zs = 1;
			}
		}
		else
		{
			// Change of X and Y coordinates
			x = graph[i].x; y = graph[i].y; z = graph[i].z;
			
			fwrite(&y,INT32,1,fileY);
			fwrite(&z,INT32,1,fileZ);
			
			v_ys.push_back(ys);
			tys += ys; ys = 1;
			v_zs.push_back(zs);
			tzs += zs; zs = 1;
		}
	}

	v_ys.push_back(ys); tys += ys; 
	v_zs.push_back(zs); tzs += zs;
	
	fclose(fileY);
	fclose(fileZ);

	BitString *bsy = new BitString(tys+v_ys.size()+1);
	buildBitString(&bsy, &v_ys, v_ys.size());

	BitString *bsz = new BitString(tzs+v_zs.size()+1);
	buildBitString(&bsz, &v_zs, v_zs.size());
	
	// Building bitsequences
	bitmapY = new BitSequenceRG(*bsy, 20);
	bitmapZ = new BitSequenceRG(*bsz, 20);
	
	delete bsy; delete bsz;
	
	output = path;
	output.append(".hdt.triples.BY");
	ofstream fp1(output.c_str());
	
	output = path;
	output.append(".hdt.triples.BZ");
	ofstream fp2(output.c_str());

	if (fp1.good())
	{
		bitmapY->save(fp1);		
		fp1.close();
	}

	if (fp2.good())
	{
		bitmapZ->save(fp2);
		fp2.close();
	}
	
	return ntriples-repeated;

}

void 
BitmapTriples::buildBitString(BitString **bs, vector<uint> *v, uint elems)
{
	(*bs)->setBit(0, true);
	uint pos = 0;
		
	for (uint i=0; i<elems; i++)
	{
		(*bs)->setBit((*v)[i]+pos+1, true);
		pos += (*v)[i]+1;
	}
}

bool
BitmapTriples::loadGraphMemory(){
	cout << "BitmapTriples::loadGraphMemory() not implemented" <<endl;
	return false;
}

BitmapTriples::~BitmapTriples()
{
	delete bitmapY;
	delete bitmapZ;
}
	
