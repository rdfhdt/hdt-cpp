/* PlainDictionary.cpp
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * Plain implementation of the Dictionary component
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
 
#include "PlainDictionary.h"
#include "Histogram.h"

PlainDictionary::PlainDictionary()
{
	this->sizeHash = 0;
	initialize();
}

PlainDictionary::PlainDictionary(unsigned int sizeHash)
{
	this->sizeHash = sizeHash;
	initialize();
}

void
PlainDictionary::initialize()
{	
    if(sizeHash > 0)
	{
		hashSubject.inithashtable(sizeHash,false);
		hashObject.inithashtable(sizeHash,false);
		//predicate Hash is initialized to 1 per 1000 of subjects and objects
		hashPredicate.inithashtable(sizeHash/1000,false);
       
		//prefixes is initialized to 10 per cent of size  
		prefixes.inithashtable(sizeHash/10);
    }
    else
	{
        hashSubject.inithashtable(false);
        hashObject.inithashtable(false);
        hashPredicate.inithashtable(INITIAL_SIZE/1000,false);
         
        //prefixes is initialized to 10 per cent of size
		prefixes.inithashtable(INITIAL_SIZE/10);
    }
}

void 
PlainDictionary::split()
{
	HASHREC* subjectRetrieve;
	HASHREC* predicateRetrieve;
	HASHREC* objectRetrieve;
	
	unsigned int i=0;
	
	//split shared subjects-objects from subjects
	for (i=0;i<hashSubject.TSIZE;i++)
	{
		subjectRetrieve = hashSubject.get(i);
		if (subjectRetrieve!=NULL)
		{
			if (subjectRetrieve->shared==true)
			{
				subjects_shared.push_back(subjectRetrieve);
			}
			else
				subjects_not_shared.push_back(subjectRetrieve);
			//verify next
			while (subjectRetrieve->next!=NULL)
			{
				subjectRetrieve = subjectRetrieve->next;
				
				if (subjectRetrieve->shared==true)
				{
					subjects_shared.push_back(subjectRetrieve);
				}
				else
					subjects_not_shared.push_back(subjectRetrieve);
			}	
		}
	}
		
	nliterals = 0;
	
	//split shared subjects-objects from objects	
	for (i=0;i<hashObject.TSIZE;i++)
	{
		objectRetrieve = hashObject.get(i);
	
		if (objectRetrieve!=NULL)
		{
			if(objectRetrieve->word[0]=='"') 
				nliterals++;

			if (objectRetrieve->shared==false)
				objects_not_shared.push_back(objectRetrieve);
			
			//verify next
			while (objectRetrieve->next!=NULL)
			{
				objectRetrieve = objectRetrieve->next;
				if(objectRetrieve->word[0]=='"') 
					nliterals++;
				
				if (objectRetrieve->shared==false)
					objects_not_shared.push_back(objectRetrieve);
			}	
		}
	}
	
	//push predicates
	for (i=0;i<hashPredicate.TSIZE;i++)
	{
		predicateRetrieve = hashPredicate.get(i);
		if (predicateRetrieve!=NULL){
			predicates.push_back(predicateRetrieve);
			//verify next
			while (predicateRetrieve->next!=NULL)
			{
				predicateRetrieve = predicateRetrieve->next;
				predicates.push_back(predicateRetrieve);
			}	
		}
	}
	
	nsubjects = subjects_not_shared.size()+subjects_shared.size();
	npredicates = predicates.size();
	nobjects = objects_not_shared.size()+subjects_shared.size();
	ssubobj = subjects_shared.size();
	
	cout<<"\n\t [Dictionary stats:\n";
	cout<<"\t   shared subjects-objects:"<<ssubobj<<"\n";
	cout<<"\t   not shared subjects:"<< nsubjects-ssubobj<<"\n";
	cout<<"\t   not shared objects:"<< nobjects-ssubobj<<"\n";
	cout<<"\t total subjects:"<<nsubjects<<"\n";
	cout<<"\t total objects:"<<nobjects<<"\n";
	cout<<"\t total predicates:"<<npredicates<<" ]\n\n";
}

void 
PlainDictionary::lexicographicSort(unsigned int mapping)
{
	this->mapping = mapping;
	
	//sort shared and not shared subjects
	sort(subjects_shared.begin(),subjects_shared.end(),PlainDictionary::cmpLexicographic);
	sort(subjects_not_shared.begin(),subjects_not_shared.end(),PlainDictionary::cmpLexicographic);
	   
	//sort not shared objects
	sort(objects_not_shared.begin(),objects_not_shared.end(),PlainDictionary::cmpLexicographic);
	   
	//sort predicates
	sort(predicates.begin(),predicates.end(),PlainDictionary::cmpLexicographic);	
	
	map(this->mapping);
}

void
PlainDictionary::map(unsigned int mapping)
{
	unsigned int i,j,k;
	
	//iterate over subjects shared and update their id and the id of objects_shared (directly in the hash)
	for (i=0;i<subjects_shared.size();i++)
	{
		subjects_shared[i]->id=i+1;
		hashObject.hashsearch(subjects_shared[i])->id = i+1;
	}
	
	for (j=0;j<subjects_not_shared.size();j++)
	{
		subjects_not_shared[j]->id = i+j+1;
	}
	
	switch (mapping)
	{
		case MAPPING1:
			// SINGLE mapping			
			for (k=0;k<objects_not_shared.size();k++)
			{
				objects_not_shared[k]->id = i+j+k+1;
			}
			break;
		case MAPPING2:
			// SHARED mapping			
			for (k=0;k<objects_not_shared.size();k++)
			{
				objects_not_shared[k]->id = i+k+1;
			}
			break;
		default:
			cout << "   <ERROR> Mapping " << mapping << " is not supported in Dictionary" << endl;
	}
	
	for (j=0;j<predicates.size();j++)
	{
		predicates[j]->id = j+1;
	}
}

void 
PlainDictionary::checkTriple(string *node)
{
    HASHREC* subjectInserted;
    HASHREC* predicateInserted;
    HASHREC* objectInserted;
    HASHREC* subjectRetrieve;
    HASHREC* objectRetrieve;
	
	unsigned int globalId=1;
	string substring="";
    unsigned int pos_separator=0;
	MINIHASHREC* prefixRetrieve;
	
	try
	{
		if (node[0]!="")
		{
			//test subject-object
			objectRetrieve = hashObject.hashsearch((char*)node[0].c_str());
			
			if (objectRetrieve!=NULL)
			{
				subjectInserted = hashSubject.hashinsert(objectRetrieve,globalId);
				objectRetrieve->shared=true;
			}
			else
			{ 
				//not shared
				//search for prefix
				pos_separator = node[0].find_last_of("/");
				if (pos_separator>0)
				{
					substring = node[0].substr(0,pos_separator+1);
					prefixRetrieve = prefixes.hashinsert((char*)substring.c_str()); //search or insert prefix
					subjectInserted = hashSubject.hashinsert((char*)node[0].c_str(),globalId,prefixRetrieve);
				}
				else
				{
					//insert with no prefix
					subjectInserted = hashSubject.hashinsert((char*)node[0].c_str(),globalId,NULL);
				}
			}
                      
			if (node[1]!="")
			{
				//search for prefix
				pos_separator = node[1].find_last_of("/");
				if (pos_separator>0)
				{
					substring = node[1].substr(0,pos_separator+1);
					prefixRetrieve = prefixes.hashinsert((char*)substring.c_str()); //search or insert prefix
					predicateInserted = hashPredicate.hashinsert((char*)node[1].c_str(),globalId,prefixRetrieve);
				}
				else
				{
					//insert with no prefix
					predicateInserted = hashPredicate.hashinsert((char*)node[1].c_str(),globalId,NULL);
				}
                  
				if (node[2]!="")
				{
					//test object-subject
					subjectRetrieve = hashSubject.hashsearch((char*)node[2].c_str());
					if (subjectRetrieve!=NULL)
					{
						objectInserted = hashObject.hashinsert(subjectRetrieve,globalId);
						subjectRetrieve->shared=true;
					}
					else
					{ 
						//search for prefix
						pos_separator = node[2].find_last_of("/");
						if (pos_separator>0)
						{
							substring = node[2].substr(0,pos_separator+1);
							prefixRetrieve = prefixes.hashinsert((char*)substring.c_str()); //search or insert prefix
							objectInserted = hashObject.hashinsert((char*)node[2].c_str(),globalId,prefixRetrieve);
						}
						else
						{
							//insert with no prefix
							objectInserted = hashObject.hashinsert((char*)node[2].c_str(),globalId,NULL);
						}
								
					}
				}
			}
		}
	}
	catch (char * s)
	{
		cout << "***" << s << endl;
	}
}

PlainDictionary::PlainDictionary(string path, unsigned int mapping, unsigned int separator, 
	unsigned int nsubjects, unsigned int npredicates, unsigned int nobjects, unsigned int ssubobj)
{
	this->mapping = mapping;
	this->nsubjects = nsubjects;
	this->npredicates = npredicates;
	this->nobjects = nobjects;
	this->ssubobj = ssubobj;	
	
	string input = path;
	input.append(".hdt.D");	
	
	char * symbol;
	unsigned int region=1;
    HASHREC* subjectInserted;
    HASHREC* predicateInserted;
    HASHREC* objectInserted;
   
    unsigned int id=1;
    char * suffix;
    MINIHASHREC* prefixRetrieve;

    hashSubject.inithashtable(nsubjects*2,true);
    hashObject.inithashtable(nobjects*2,true);
    hashPredicate.inithashtable(npredicates*2,true);
    prefixes.inithashtable((nsubjects+nobjects-ssubobj+npredicates/10)+1); //ten per cent of subjects(1 minimum)    
   
    SpacelessParser *SP = new SpacelessParser;
	
	int voc1=0, voc2=0, voc3=0, voc4=0;
    
	//HACER UN TEMP PARA NO CREAR Y LIBERAR TODO EL TIEMPO 
	char * prefix = (char *) malloc(1+MAX_ITEM_PARSED); //as big as the max long in SP
	
	if (SP->InitParser((char*)input.c_str(), separator))
	{
		// *** Inicio del proceso de compresión ***		
		while(SP->ParseWord())
		{
			symbol = SP->aWord;
			
			if (*symbol!='\0')
			{ //exists element
				
				if (region==1)
				{ //shared SO
					//search for prefix
					suffix = strrchr(symbol,'/'); //get substring suffix
					
					if (suffix!=NULL)
					{

						strncpy(prefix,symbol,suffix-symbol+1);
						prefix[suffix-symbol+1] = '\0';
						
						prefixRetrieve = prefixes.hashinsert(prefix); //search or insert prefix
						
						subjectInserted = hashSubject.hashinsert(symbol,id,prefixRetrieve);
					}
					else
					{
						//insert with no prefix
						subjectInserted = hashSubject.hashinsert(symbol,id,NULL);
					}
					
					subjectInserted->shared=true;
					//insert with the same Id (autoincrement on insert)
					id--;
					objectInserted = hashObject.hashinsert(subjectInserted,id);
					
					//insert auxiliary
					subjects_shared.push_back(subjectInserted);
					
					voc1++;
				}
				else if (region==2)
				{ //not shared Subjects
					suffix = strrchr(symbol,'/'); //get substring suffix
					
					if (suffix!=NULL)
					{
						strncpy(prefix,symbol,suffix-symbol+1);
						prefix[suffix-symbol+1] = '\0';
						prefixRetrieve = prefixes.hashinsert(prefix); //search or insert prefix
						subjectInserted = hashSubject.hashinsert(symbol,id,prefixRetrieve);
					}
					else
					{
						//insert with no prefix
						subjectInserted = hashSubject.hashinsert(symbol,id,NULL);
					}
					
					//insert auxiliary
					subjects_not_shared.push_back(subjectInserted);
					
					voc2++;
				}
				else if (region==3)
				{ //not shared Objects
					suffix = strrchr(symbol,'/'); //get substring suffix
					
					if (suffix!=NULL)
					{
						strncpy(prefix,symbol,suffix-symbol+1);
						prefix[suffix-symbol+1] = '\0';
						prefixRetrieve = prefixes.hashinsert(prefix); //search or insert prefix
						objectInserted = hashObject.hashinsert(symbol,id,prefixRetrieve);
					}
					else{
						//insert with no prefix
						objectInserted = hashObject.hashinsert(symbol,id,NULL);
					}					
					
					//insert auxiliary
					objects_not_shared.push_back(objectInserted);
					
					voc3++;
				}
				else if (region==4){ //predicates
					suffix = strrchr(symbol,'/'); //get substring suffix
					
					if (suffix!=NULL)
					{
						strncpy(prefix,symbol,suffix-symbol+1);
						prefix[suffix-symbol+1] = '\0';
						prefixRetrieve = prefixes.hashinsert(prefix); //search or insert prefix
						predicateInserted = hashPredicate.hashinsert(symbol,id,prefixRetrieve);
					}
					else
					{
						//insert with no prefix
						predicateInserted = hashPredicate.hashinsert(symbol,id,NULL);
					}
										
					//insert auxiliary
					predicates.push_back(predicateInserted);
					
					voc4++;
				}
				
			}
			else{
				region++; //change region
				if (region==3)
				{ //objects not shared
				  // COMPROBAR ESTO PARA EL MAPPING DE K2TREES
				  if (mapping == MAPPING2)
				  {
					  id = ssubobj+1; //restart id
					  
					  //cout << "MAPPING2 = " << id << endl;
				  }
				  else
				  {
					  id = nsubjects+1; //restart id
					  
					  //cout << "MAPPING1 = " << id << endl;
				  }	 
				}
				else if (region==4)
				{ //predicates
					id=1; //restart id
				}
			}
		}
	}
	else{
		cout<<" !! Not a valid Dictionary:" << endl;
		//return false;
	}
	free (prefix);
	
	//cout << "LEO: " << voc1 << ";" << voc2 << ";" << voc3 << ";" << voc4 << endl;
	
	//return true;
}

unsigned int 
PlainDictionary::retrieveID(string key, unsigned int vocabulary)
{
	unsigned int id=ERROR;
	HASHREC * ret=NULL;
	switch (vocabulary)
	{
		case VOC_SUBJECT:
			ret = hashSubject.hashsearch((char*)key.c_str());
			if (ret!=NULL)
				id = ret->id;
			break;
		case VOC_PREDICATE:
			ret = hashPredicate.hashsearch((char*)key.c_str());
			if (ret!=NULL)
				id = ret->id;
			break;
		case VOC_OBJECT:
			ret = hashObject.hashsearch((char*)key.c_str());
			if (ret!=NULL)
				id = ret->id;		
			break;
		default:
			cout << "   <ERROR> Mode " << vocabulary << " is not supported in Dictionary" << endl;
			id = ERROR;
	}
	
	return id;
}

void
PlainDictionary::updateID(unsigned int id, unsigned int new_id, unsigned int vocabulary)
{
	switch (vocabulary)
	{
		case VOC_SUBJECT:
			if (id <= subjects_shared.size())
			{
				subjects_shared[id-1]->new_id = new_id;
				cluster_subjects_shared[new_id-1] = subjects_shared[id-1];
				
				//cout << "SO[" << id << ">" << new_id << "] = " << cluster_subjects_shared[new_id-1]->word << endl;
			}	
			else
			{
				subjects_not_shared[id-subjects_shared.size()-1]->new_id = new_id;
				cluster_subjects_not_shared[new_id-subjects_shared.size()-1] = subjects_not_shared[id-subjects_shared.size()-1];
				
				//cout << "S[" << id << ">" << new_id << "] = " << cluster_subjects_not_shared[new_id-subjects_shared.size()-1]->word << endl;
			}	
			
			break;

		case VOC_OBJECT:
			if (id <= subjects_shared.size())
			{
				subjects_shared[id-1]->new_id = new_id;
				cluster_subjects_shared[new_id-1] = subjects_shared[id-1];
				
				//cout << "SO[" << id << ">" << new_id << "] = " << cluster_subjects_shared[new_id-1]->word << endl;
			}
			else
			{
				unsigned int pos;
				
				if (mapping == MAPPING2) pos = subjects_shared.size()+1;
				else pos = nsubjects+1;
				
				objects_not_shared[id-pos]->new_id = new_id;
				cluster_objects_not_shared[new_id-pos] = objects_not_shared[id-pos];
				
				//cout << "O[" << id << ">" << new_id << "] = " << cluster_objects_not_shared[new_id-subjects_shared.size()-1]->word << endl;
			}
		
			break;
		default:
			cout << "   <ERROR> Mode " << vocabulary << " is not supported in Dictionary" << endl;
	}
}

string
PlainDictionary::retrieveString(unsigned int id, unsigned int vocabulary)
{	
	switch (vocabulary)
	{
		case VOC_SUBJECT:
			if (id <= subjects_shared.size())
			{
				// Shared subject
				if (subjects_shared[id-1]->prefix==NULL)
				{
					return subjects_shared[id-1]->word;
				}
				else
				{
					string word;
					word.assign(subjects_shared[id-1]->prefix->word);
					word.append(subjects_shared[id-1]->word);
					return word.c_str();
				}
			}
			else
			{
				if (subjects_not_shared[id-subjects_shared.size()-1]->prefix==NULL)
				{
					return subjects_not_shared[id-subjects_shared.size()-1]->word;
				}
				else
				{
					string word;
					word.assign(subjects_not_shared[id-subjects_shared.size()-1]->prefix->word);
					word.append(subjects_not_shared[id-subjects_shared.size()-1]->word);
					return word.c_str();
				}
			}
			
			break;
		case VOC_PREDICATE:
			if (predicates[id-1]->prefix==NULL)
			{
				return predicates[id-1]->word;
			}
			else
			{
				string word;
				word.assign(predicates[id-1]->prefix->word);
				word.append(predicates[id-1]->word);
				return word.c_str();
			}
				
			break;
		case VOC_OBJECT:
			if (id <= subjects_shared.size())
			{
				// Shared object
				if (subjects_shared[id-1]->prefix==NULL)
				{
					return subjects_shared[id-1]->word;
				}
				else
				{
					string word;
					word.assign(subjects_shared[id-1]->prefix->word);
					word.append(subjects_shared[id-1]->word);
					return word.c_str();
				}
			}
			else
			{
				unsigned int pos;
				
				if (mapping == MAPPING2) {
					pos = id-subjects_shared.size()-1;
				} else {
					pos = id-nsubjects-1;
				}
				
				if(objects_not_shared.size()<=pos) {
					return "";
				}
				
				if (objects_not_shared[pos]->prefix==NULL)
				{
					return objects_not_shared[pos]->word;
				}
				else
				{
					string word;
					word.assign(objects_not_shared[pos]->prefix->word);
					word.append(objects_not_shared[pos]->word);
					return word.c_str();
				}
			}
		
			break;
		default:
			cout << "   <ERROR> Mode " << vocabulary << " is not supported in Dictionary" << endl;
			return NULL;
	}
}

void 
PlainDictionary::beginClustering()
{
	cluster_subjects_shared.resize(subjects_shared.size());
	cluster_subjects_not_shared.resize(subjects_not_shared.size());
	cluster_objects_not_shared.resize(objects_not_shared.size());
}

void 
PlainDictionary::endClustering()
{
	subjects_shared = cluster_subjects_shared;
	subjects_not_shared = cluster_subjects_not_shared;
	objects_not_shared = cluster_objects_not_shared;
	
	cluster_subjects_shared.~vector();
	cluster_subjects_not_shared.~vector();
	cluster_objects_not_shared.~vector();
}

unsigned int
PlainDictionary::isClusterized(unsigned int id, unsigned int vocabulary)
{
	switch (vocabulary)
	{
		case VOC_SUBJECT:
			if (id <= subjects_shared.size())
				id = subjects_shared[id-1]->new_id;
			else
				id = subjects_not_shared[id-subjects_shared.size()-1]->new_id;
			break;
		case VOC_PREDICATE:
			id = predicates[id-1]->new_id;				
			break;
		case VOC_OBJECT:
			if (id <= subjects_shared.size())
				id = subjects_shared[id-1]->new_id;
			else
			{
				unsigned int pos;
				
				if (mapping == MAPPING2) pos = id-subjects_shared.size()-1;
				else pos = id-nsubjects-1;
					
				
				// OJO AQUI CON EL MAPPING!!!!
				id = objects_not_shared[pos]->new_id;
			}
			break;
		default:
			cout << "   <ERROR> Mode " << vocabulary << " is not supported in Dictionary" << endl;
			id = ERROR;
	}

	return id;
}

void 
PlainDictionary::write(string path, unsigned int marker)
{
	unsigned int i=0;
	
	string output = path;
	output.append(".hdt.D");
	ofstream outFile(output.c_str()); 
	
	//shared subjects-objects from subjects
	for (i=0;i<subjects_shared.size();i++)
	{
		if (subjects_shared[i]->prefix!=NULL)
		{
			outFile<<subjects_shared[i]->prefix->word;
		}
		outFile<<subjects_shared[i]->word;
		outFile.put(marker); //character to split file
	}
	
	outFile.put(marker); //extra line to set the beggining of next part of dictionary	
	
	//not shared subjects
	for (i=0;i<subjects_not_shared.size();i++)
	{
		if (subjects_not_shared[i]->prefix!=NULL)
		{
			outFile<<subjects_not_shared[i]->prefix->word;
		}
		outFile<<subjects_not_shared[i]->word;
		outFile.put(marker); //characte to split file
	}
	
	outFile.put(marker); //extra line to set the beggining of next part of dictionary	
	
	//not shared objects
	for (i=0;i<objects_not_shared.size();i++)
	{
		if (objects_not_shared[i]->prefix!=NULL)
		{
			outFile<<objects_not_shared[i]->prefix->word;
		}
		outFile<<objects_not_shared[i]->word;
		outFile.put(marker); //character to split file
	}
	
	outFile.put(marker); //extra line to set the beggining of next part of dictionary	
	
	//predicates
	for (i=0;i<predicates.size();i++)
	{
		if (predicates[i]->prefix!=NULL)
		{
			outFile<<predicates[i]->prefix->word;
		}
		outFile<<predicates[i]->word;
		outFile.put(marker); //character  to split file
	}
	
	if (outFile.good()) outFile.close();
}

void 
PlainDictionary::dumpStats(string &output)
{
	unsigned int i=0;

	const int maxval = 50000;
	const int nbins = 50000;
	
	Histogram histoURI(0, maxval, nbins);
	Histogram histoLiteral(0, maxval, nbins);
	Histogram histoBlank(0, maxval, nbins);
	
	string tmp;
	//shared subjects-objects from subjects
	for (i=0;i<subjects_shared.size();i++)
	{
		tmp.clear();
	
		if (subjects_shared[i]->prefix!=NULL)
		{
			tmp.append(subjects_shared[i]->prefix->word);
		}
		tmp.append(subjects_shared[i]->word);
		
		if(tmp[0]=='<') {
			histoURI.Add(tmp.length());
		} else if(tmp[0]=='"') {
			histoLiteral.Add(tmp.length()); 
		} else if(tmp[0]=='_') {
			histoBlank.Add(tmp.length());
		} else {
			cout << "String not URI/Lit?: " << tmp << endl;
		}
	}
	
	//not shared subjects
	for (i=0;i<subjects_not_shared.size();i++)
	{
		tmp.clear();
		if (subjects_not_shared[i]->prefix!=NULL)
		{
			tmp.append(subjects_not_shared[i]->prefix->word);
		}
		tmp.append(subjects_not_shared[i]->word);

		if(tmp[0]=='<') {
			histoURI.Add(tmp.length());
		} else if(tmp[0]=='"') {
			histoLiteral.Add(tmp.length()); 
		} else if(tmp[0]=='_') {
			histoBlank.Add(tmp.length());
		} else {
			cout << "String not URI/Lit?: " << tmp << endl;
		}
	}
	
	//not shared objects
	for (i=0;i<objects_not_shared.size();i++)
	{
		tmp.clear();
		if (objects_not_shared[i]->prefix!=NULL)
		{
			tmp.append(objects_not_shared[i]->prefix->word);
		}
		tmp.append(objects_not_shared[i]->word);
		if(tmp[0]=='<') {
			histoURI.Add(tmp.length());
		} else if(tmp[0]=='"') {
			histoLiteral.Add(tmp.length()); 
		} else if(tmp[0]=='_') {
			histoBlank.Add(tmp.length());
		} else {
			cout << "String not URI/Lit?: " << tmp << endl;
		}
	}
	
	//predicates
	for (i=0;i<predicates.size();i++)
	{
		tmp.clear();
		if (predicates[i]->prefix!=NULL)
		{
			tmp.append(predicates[i]->prefix->word);
		}
		tmp.append(predicates[i]->word);
		if(tmp[0]=='<') {
			histoURI.Add(tmp.length());
		} else if(tmp[0]=='"') {
			histoLiteral.Add(tmp.length()); 
		} else if(tmp[0]=='_') {
			histoBlank.Add(tmp.length());
		} else {
			cout << "String not URI/Lit?: " << tmp << endl;
		}
	}
	
	histoURI.end();
	histoURI.dump(output.c_str(), "URI");
	
	histoLiteral.end();
	histoLiteral.dump(output.c_str(), "Literal");
	
	histoBlank.end();
	histoBlank.dump(output.c_str(), "Blank");
}

PlainDictionary::~PlainDictionary()
{
        hashSubject.~HashTable();
        hashObject.~HashTable();
        hashPredicate.~HashTable();
		prefixes.~MiniHashTable();
}
