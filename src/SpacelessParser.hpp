/* SpacelessParser
Copyright (C) 2005 Joaquin Adiego

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Contacting the author:
Joaquin Adiego
ETIyT. Departamento de Informatica.
Camino del Cementerio s/n
47011 Valladolid (Spain)

jadiego@infor.uva.es
*/

/* Derived 1/2009 from sources by Gonzalo Navarro and Antonio Fariña */

#ifndef __SPACELESS_PARSER__
#define __SPACELESS_PARSER__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>


#define BUFFER_SIZE 10485760

//MAX_ITEM_PARSED = longitud maxima de una palabra + 5
#define MAX_ITEM_PARSED 524288

#define byteType unsigned char

/* Buffer management */
typedef struct sBuffer
{
	int size;               /* size of this buffer */
	int file;               /* associated file */
	int fpos;               /* pos of current buffer in file */
	byteType *data;             /* current text */
	int dsize;              /* size of current text */
	int minDataInBuffer;    /*  */
} *Buffer;

class SpacelessParser
{
private:
	int f;     // File handle
	Buffer B;  // Buffer over f
	unsigned char *text, *top, *nextbuf;
	unsigned char *pbeg, *pend, *wordstart;
	unsigned long size;

	/* Creates a new buffer */
	void bufCreate()
	{
		B = (Buffer) malloc (sizeof(struct sBuffer));
		B->size = BUFFER_SIZE;
		B->data = (byteType *) malloc (1+B->size);
		B->data++;
		B->dsize = 0;
		B->minDataInBuffer = MAX_ITEM_PARSED;
	}
	
	/* Terminates use of buffer B */
	void bufDestroy()
	{
		free (B->data-1);
		free (B);
	} 
	
	/* Assigns a file to B */
	void bufSetFile( int file )
	{
		B->file = file;
		B->fpos = 0;
		B->dsize = read (B->file,B->data,B->size);
	}
	
	/* Gets buffer size of B */
	int bufSize()
	{
		return B->size;
	}
	
	/* Reads a new buffer starting at position pnext */
	void bufLoad()
	{ 
		register byteType *src = nextbuf;
		register byteType *dst = B->data;
		register int mov = B->dsize - (src - dst);
	
		/* move unused part to the beginning (memcpy is not portable) */
		B->dsize = mov;
	
		while (mov--) *dst++ = *src++;
		mov = B->size - B->dsize;
		B->dsize += read (B->file, dst, mov);
		B->fpos += mov;

		////fprintf(stderr,"\n  %ld bytes remain in  buffer ",B->dsize);
	}
	
	/* Tells whether there is no more data in B + its file */
	int bufEmpty()
	{
		return B->dsize == 0;
	}
	
	/* Tells whether there is no more data to read in B's file */
	int bufEof()
	{ 
		return B->dsize < B->size;
	}
	
	/* Tells the file position of the current buffer */
	int bufTextPos()
	{ 
		return B->fpos;
	}
	
	/* Gives the area of memory of the current buffer */
	/* One text position before the given buffer can be touched */
	void bufCurrent()
	{ 
		text = B->data;
		if( !bufEof() )
		{
			top = B->data + B->dsize - B->minDataInBuffer;
		}
		else
		{
			top = B->data + B->dsize;
		}
	}

public:
	char * aWord;
	int separator;

	SpacelessParser() { aWord = (char *)malloc( 1+MAX_ITEM_PARSED ); B=NULL; f=-1; }
	~SpacelessParser() { bufDestroy(); free(aWord); if( f!=-1 ) close(f); }

	bool InitParser( char * infile, int separator )
	{
		this->separator = separator;
		
		// Initialization of read buffer
		f = open( infile, O_RDONLY );
		if( f == -1 )
		{
			fprintf( stderr, "Cannot read file %s\n", infile );
			return false;
		}
		bufCreate();
		bufSetFile( f );
		bufCurrent();
		pbeg = text; pend = top;
		return true;
	}


	bool ParseWord()
	{
		char marker = separator;
		while( B->size == B->dsize )
		{
			while( pbeg < pend )
			{
				//parsing a word or separator
				size=0;
				wordstart = pbeg;
				
				//alphanumerical data
				while( (size<MAX_ITEM_PARSED) && (*pbeg!=marker) )
				{
					size++;
					pbeg++;
				}
				
				
				memcpy( aWord, wordstart, size );
				aWord[size] = '\0';
				pbeg++;
				return true;
			}
	
			if( bufEof() )
				nextbuf = top;
			else
				nextbuf = pbeg;
	
			bufLoad();
			bufCurrent();
			pbeg = text; pend = top;
		}

		if( !bufEmpty() )
		{
			while ( pbeg < pend )
			{
				size=0;
				wordstart = pbeg;
				
				//alphanumerical data
				while( (size<MAX_ITEM_PARSED) && (*pbeg!=marker) && (pbeg < pend) )
				{
					size++;
					pbeg++;
				}
			
				if ( pbeg >= pend )
				{
					 //need in case of the last word been parsed further away of cend. 
	  				size -= (pbeg-pend);
	  			}

				memcpy( aWord, wordstart, size );
				aWord[size] = '\0';
				pbeg++;
				return true;
			}
		}
		return false;
	}
};


#endif
