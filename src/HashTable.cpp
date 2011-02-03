/* HashTable.h
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * Hash Table implementation with Linear Probbing
 * Based on J. Zobel implementation, April 2001.
 * ---------------------------------------------------------------------
 * Permission to use this code is freely granted, provided that this
 * statement is retained.
 * ---------------------------------------------------------------------
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

#include "HashTable.h"

/** HashTable
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
HashTable::HashTable()
{	
}

/** Get
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
HASHREC** 
HashTable::get()
{
	return hash;
}

/** get rec at position i
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
HASHREC*
HashTable::get(unsigned pos)
{
	return hash[pos];
}

/* Create hash table, initialise ptrs to NULL
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
void
HashTable::inithashtable(bool countOrder)
{
    int		i;
    TSIZE = INITIAL_SIZE;
    numNodes = 0;
    countOriginalOrder= countOrder;
    hash = (HASHREC **) malloc( sizeof(HASHREC *) * TSIZE );

    for( i=0 ; i<TSIZE ; i++ )
	hash[i] = (HASHREC *) NULL;
}

/* Create hash table with a given size, initialise ptrs to NULL
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
void
HashTable::inithashtable(unsigned int size,bool countOrder)
{
    int		i;
    TSIZE = size;
    numNodes = 0;
    countOriginalOrder= countOrder;
    hash = (HASHREC **) malloc( sizeof(HASHREC *) * TSIZE );

    for( i=0 ; i<TSIZE ; i++ )
	hash[i] = (HASHREC *) NULL;
}

/** Delete Key
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
void
HashTable::delete_key(char *w)
{	
    HASHREC	*htmp, *hprv;
    unsigned int hval = HASHFN(w, TSIZE, SEED);	

    for( hprv = NULL, htmp=hash[hval]
	    ; htmp != NULL && scmp(htmp, w) != 0
	    ; hprv = htmp, htmp = htmp->next )
    {
	;
    }
    
    if(hprv == NULL)hash[hval] = htmp->next;
    else hprv->next = htmp->next;
    
    free(htmp->word);
    numNodes--;
//    free(htmp);
    htmp = NULL;
}


/* Search hash table for given string, return record if found, else NULL
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
HASHREC *
HashTable::hashsearch(char *w)
{	
    HASHREC	*htmp, *hprv;
    unsigned int hval = HASHFN(w, TSIZE, SEED);
	
    for( hprv = NULL, htmp=hash[hval]
	    ; htmp != NULL && scmp(htmp, w) != 0
	    ; hprv = htmp, htmp = htmp->next )
    {
	;
    }

    if( hprv!=NULL && htmp!=NULL ) /* move to front on access */
    {
	hprv->next = htmp->next;
	htmp->next = hash[hval];
	hash[hval] = htmp;
    }

    return(htmp);
}

/* Search hash table for given rec to get another equivalent, return record if found, else NULL
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
HASHREC *
HashTable::hashsearch(HASHREC *rec)
{	
    HASHREC	*htmp, *hprv;
    
     char *w;
    
    if (rec->prefix==NULL){
		w= rec->word;
	}
	else{ //concat prefix and suffix in w
		w = (char *)malloc (strlen(rec->prefix->word)+strlen(rec->word)+1);
		strcpy(w,rec->prefix->word);
		strcat(w,rec->word);
	}
	
    unsigned int hval = HASHFN(w, TSIZE, SEED);
	
    for( hprv = NULL, htmp=hash[hval]
	    ; htmp != NULL && scmp(htmp, w) != 0
	    ; hprv = htmp, htmp = htmp->next )
    {
	;
    }

    if( hprv!=NULL && htmp!=NULL ) /* move to front on access */
    {
	hprv->next = htmp->next;
	htmp->next = hash[hval];
	hash[hval] = htmp;
    }
    
    if (rec->prefix!=NULL){
		free(w);
	}
    

    return(htmp);
}

/* Search hash table for given string, insert if not found, update information if found
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
HASHREC*
HashTable::hashinsert(HASHREC* shared, unsigned int &globalId)
{
    HASHREC	*htmp, *hprv;
    char *w;
    
    if (shared->prefix==NULL){
		w= shared->word;
	}
	else{ //concat prefix and suffix in w
		w = (char *)malloc (strlen(shared->prefix->word)+strlen(shared->word)+1);
		strcpy(w,shared->prefix->word);
		strcat(w,shared->word);
	}
    unsigned int hval = HASHFN(w, TSIZE, SEED);

    for( hprv = NULL, htmp=hash[hval]
	    ; htmp != NULL && scmp(htmp, w) != 0
	    ; hprv = htmp, htmp = htmp->next )
    {
	;
    }

    if( htmp==NULL )
    {	
   
    	htmp = (HASHREC *) malloc( sizeof(HASHREC) );
    	
    	htmp->shared=true;
    	
		if (shared->prefix==NULL){
			htmp->word=w; //get the same reference
			htmp->prefix=NULL;
		}
		else{
			htmp->word = shared->word;
			htmp->prefix = shared->prefix;
		}
    	
        if (countOriginalOrder==false)                  
			htmp->id = 0; //initialize to 0, asigned after the final sort
		else{
			htmp->id = globalId;
			globalId++;
		}
		htmp->new_id=0;
    	htmp->count=1;
    	numNodes++;
    	htmp->next = NULL;
    	    
    	if( hprv==NULL )
    	    hash[hval] = htmp;
    	else
    	    hprv->next = htmp;
    
    	/* new records are not moved to front */
    }
    else
    {
		htmp->count=htmp->count+1;
		htmp->shared = true;
    	if( hprv!=NULL ) /* move to front on access */
    	{
    	    hprv->next = htmp->next;
    	    htmp->next = hash[hval];
    	    hash[hval] = htmp;
    	}
    	
    }
    
    if (shared->prefix!=NULL)
		free(w);
   
    return htmp;
}

/* Search hash table for given string, insert if not found, update information if found
 * Insert not shared, prefix or not.
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
HASHREC*
HashTable::hashinsert(char *w, unsigned int &globalId,MINIHASHREC* prefix)
{
    HASHREC	*htmp, *hprv;
    unsigned int hval = HASHFN(w, TSIZE, SEED);
	
	
    for( hprv = NULL, htmp=hash[hval]
	    ; htmp != NULL && scmp(htmp, w) != 0
	    ; hprv = htmp, htmp = htmp->next )
    {
	;
    }
   
    if( htmp==NULL )
    {
	
    	
    	htmp = (HASHREC *) malloc( sizeof(HASHREC) );
    	
    	htmp->shared=false;
    	
    	if (prefix!=NULL){
			htmp->prefix = prefix;
			htmp->word = (char *) malloc( strlen(w) - strlen(prefix->word) + 1 );
			strcpy(htmp->word, w+strlen(prefix->word)); //copy the suffix value
			
		}
		else{
			htmp->prefix=NULL;
			htmp->word = (char *) malloc( strlen(w) + 1 );
			strcpy(htmp->word, w); //copy the value
		}
    	
        if (countOriginalOrder==false)                  
			htmp->id = 0; //initialize to 0, asigned after the final sort
		else{
			htmp->id = globalId;
			globalId++;
		}
		htmp->new_id=0;
    	htmp->count=1;
    	numNodes++;
    	htmp->next = NULL;
    	    
    	if( hprv==NULL )
    	    hash[hval] = htmp;
    	else
    	    hprv->next = htmp;
    
    	/* new records are not moved to front */
    }
    else
    {
		
		htmp->count=htmp->count+1;
		htmp->shared = false;
    	if( hprv!=NULL ) /* move to front on access */
    	{
    	    hprv->next = htmp->next;
    	    htmp->next = hash[hval];
    	    hash[hval] = htmp;
    	}
    	
    }
   
    return htmp;
}

/** Dump
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
void
HashTable::dump()
{
  int i;
  HASHREC *ptr;

  for( i=0; i<TSIZE; i++ )
  {
      for( ptr=hash[i]; ptr!=NULL; ptr=ptr->next )
      {
          // ptr stores current element in the iteration 

      }
  }
}

/* Bitwise hash function.  Note that tsize does not have to be prime.
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
unsigned int
HashTable::bitwisehash(char *word, unsigned int tsize, unsigned int seed)
{
    char	c;
    unsigned int h;

    h = seed;
    for( ; ( c=*word )!='\0' ; word++ )
    {
	h ^= ( (h << 5) + c + (h >> 2) );
    }
    return((unsigned int)((h&0x7fffffff) % tsize));
}

/* SCMP
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
int
HashTable::scmp( HASHREC* htmp, char *s2 )
{
	char* it;
	
	if (htmp->prefix==NULL){
		
		it = htmp->word;
		
		while( *it != '\0' && *it == *s2 )
		{
		it++;
		s2++;
		}
		return( *it-*s2 );
	}	
	
	else{
		it = htmp->prefix->word;
		
		// compare with prefix and suffix. 
		while(  it!= '\0' && *it== *s2)
		{
			it++;
			s2++;
		}
		if (*it != '\0'){
			//not match
			return( *it-*s2 );
		}
		else{
			//match prefix, test suffix (from the last pointer of s2)
			
			
			it = htmp->word;
			while( *it != '\0' && *it == *s2 )
			{
			it++;
			s2++;
			}
			
			return( *it-*s2 );
			
		}
	}
}

/** Destructor for HashTable */
HashTable::~HashTable()
{	
      for(int i=0 ; i<TSIZE ; i++ )
      {
          HASHREC* ptr;
          while (hash[i] != NULL)
          {
              ptr = hash[i]->next;
              // MIRARLO EN ALGÚN MOMENTO!!!
	      //	if (hash[i]->word != NULL)
	      //		free(hash[i]->word);

              free(hash[i]);
              hash[i] = ptr;   
          }
      }

      free(hash);
}

/** Size
 * @param param_a Description of the param.
 * @param param_b Description of the param.
 * @return The expected result
 */
unsigned int
HashTable::size()
{
return numNodes;                 
}
