/*
 * RDFParserNtriples.cpp
 *
 *  Created on: 08/07/2011
 *      Author: mck
 */

#include <fstream>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>

#include "RDFParserNtriples.hpp"
#include "../util/fileUtil.hpp"

namespace hdt {

RDFParserNtriples::RDFParserNtriples(const char *fileName, RDFNotation notation) :
		RDFParser(notation),
		input(new std::ifstream(fileName, ios::binary | ios::in))
{
	lineNum = 1;
	column = 0;
	byte = 0;

	isNQuad = false; // FIXME: Adjust

	size = fileUtil::getSize(*input);
}

RDFParserNtriples::RDFParserNtriples(std::istream &in, RDFNotation notation) :
		RDFParser(notation),
		input(&in)
{
	lineNum = 1;
	column = 0;
	byte = 0;

	isNQuad = false; // FIXME: Adjust

	size = fileUtil::getSize(*input);
}

RDFParserNtriples::~RDFParserNtriples() {

}


/* These are for 7-bit ASCII and not locale-specific */
#define IS_ASCII_ALPHA(c) (((c) > 0x40 && (c) < 0x5B) || ((c) > 0x60 && (c) < 0x7B))
#define IS_ASCII_UPPER(c) ((c) > 0x40 && (c) < 0x5B)
#define IS_ASCII_DIGIT(c) ((c) > 0x2F && (c) < 0x3A)
#define IS_ASCII_PRINT(c) ((c) > 0x1F && (c) < 0x7F)
#define TO_ASCII_LOWER(c) ((c)+0x20)

typedef unsigned long raptor_unichar;

const raptor_unichar raptor_unicode_max_codepoint = 0x10FFFF;

/**
 * raptor_unicode_utf8_string_put_char:
 * @c: Unicode character
 * @output: UTF-8 string buffer or NULL
 * @length: length of output buffer
 *
 * Encode a Unicode character to a UTF-8 string
 *
 * If @output is NULL, then will calculate the length rather than
 * perform the encoding.  This can be used by the called to allocate
 * space and then re-call this function with the new buffer.
 *
 * Return value: number of bytes encoded to output buffer or <0 on failure
 **/
int raptor_unicode_utf8_string_put_char(raptor_unichar c,
		unsigned char *output, size_t length)
{
	size_t size = 0;

	/* check for illegal code positions:
	 * U+D800 to U+DFFF (UTF-16 surrogates)
	 * U+FFFE and U+FFFF
	 */
	if((c > 0xD7FF && c < 0xE000) || c == 0xFFFE || c == 0xFFFF)
		return -1;

	if      (c < 0x00000080)
		size = 1;
	else if(c < 0x00000800)
		size = 2;
	else if(c < 0x00010000)
		size = 3;
	else if(c < 0x00200000)
		size = 4;
	else if(c < 0x04000000)
		size = 5;
	else if(c < 0x80000000)
		size = 6;
	else
		return -1;

	/* when no buffer given, return size */
	if(!output)
		return size;

	if(size > length)
		return -1;

	switch(size) {
	case 6:
		output[5] = 0x80 | (unsigned char)(c & 0x3F);
		c= c >> 6;
		/* set bit 2 (bits 7,6,5,4,3,2 less 7,6,5,4,3 set below) on last byte */
		c |= 0x4000000; /* 0x10000 = 0x04 << 24 */
		/* FALLTHROUGH */
	case 5:
		output[4] = 0x80 | (unsigned char)(c & 0x3F);
		c= c >> 6;
		/* set bit 3 (bits 7,6,5,4,3 less 7,6,5,4 set below) on last byte */
		c |= 0x200000; /* 0x10000 = 0x08 << 18 */
		/* FALLTHROUGH */
	case 4:
		output[3] = 0x80 | (unsigned char)(c & 0x3F);
		c= c >> 6;
		/* set bit 4 (bits 7,6,5,4 less 7,6,5 set below) on last byte */
		c |= 0x10000; /* 0x10000 = 0x10 << 12 */
		/* FALLTHROUGH */
	case 3:
		output[2] = 0x80 | (unsigned char)(c & 0x3F);
		c= c >> 6;
		/* set bit 5 (bits 7,6,5 less 7,6 set below) on last byte */
		c |= 0x800; /* 0x800 = 0x20 << 6 */
		/* FALLTHROUGH */
	case 2:
		output[1] = 0x80 | (unsigned char)(c & 0x3F);
		c= c >> 6;
		/* set bits 7,6 on last byte */
		c |= 0xc0;
		/* FALLTHROUGH */
	case 1:
		output[0] = (unsigned char)c;
	}

	return size;
}


/**
 * raptor_unicode_utf8_string_get_char:
 * @input: UTF-8 string buffer
 * @length: buffer size
 * @output: Pointer to the Unicode character or NULL
 *
 * Decode a UTF-8 encoded string to get a Unicode character.
 *
 * If output is NULL, then will calculate the number of bytes that
 * will be used from the input buffer and not perform the conversion.
 *
 * Return value: bytes used from input buffer or <0 on failure: -1 input buffer too short or length error, -2 overlong UTF-8 sequence, -3 illegal code positions, -4 code out of range U+0000 to U+10FFFF.  In cases -2, -3 and -4 the coded character is stored in the output.
 */
int
raptor_unicode_utf8_string_get_char(const unsigned char *input, size_t length,
		raptor_unichar *output)
{
	unsigned char in;
	size_t size;
	raptor_unichar c = 0;

	if(length < 1)
		return -1;

	in=*input++;
	if((in & 0x80) == 0) {
		size = 1;
		c= in & 0x7f;
	} else if((in & 0xe0) == 0xc0) {
		size = 2;
		c= in & 0x1f;
	} else if((in & 0xf0) == 0xe0) {
		size = 3;
		c= in & 0x0f;
	} else if((in & 0xf8) == 0xf0) {
		size = 4;
		c = in & 0x07;
	} else if((in & 0xfc) == 0xf8) {
		size = 5;
		c = in & 0x03;
	} else if((in & 0xfe) == 0xfc) {
		size = 6;
		c = in & 0x01;
	} else
		return -1;


	if(!output)
		return size;

	if(length < size)
		return -1;

	switch(size) {
	case 6:
		in=*input++ & 0x3f;
		c= c << 6;
		c |= in;
		/* FALLTHROUGH */
	case 5:
		in=*input++ & 0x3f;
		c= c << 6;
		c |= in;
		/* FALLTHROUGH */
	case 4:
		in=*input++ & 0x3f;
		c= c << 6;
		c |= in;
		/* FALLTHROUGH */
	case 3:
		in=*input++ & 0x3f;
		c= c << 6;
		c |= in;
		/* FALLTHROUGH */
	case 2:
		/* '*input' used here since we never need to use new value of input [CLANG] */
		in = *input & 0x3f;
		c= c << 6;
		c |= in;
		/* FALLTHROUGH */
	default:
		break;
	}

	*output=c;

	/* check for overlong UTF-8 sequences */
	switch(size) {
	case 2:
		if(c < 0x00000080)
			return -2;
		break;
	case 3:
		if(c < 0x00000800)
			return -2;
		break;
	case 4:
		if(c < 0x00010000)
			return -2;
		break;

	default: /* 1 */
	break;
	}


	/* check for illegal code positions:
	 * U+D800 to U+DFFF (UTF-16 surrogates)
	 * U+FFFE and U+FFFF
	 */
	if((c > 0xD7FF && c < 0xE000) || c == 0xFFFE || c == 0xFFFF)
		return -3;

	if(c > raptor_unicode_max_codepoint)
		return -4;

	return size;
}


int RDFParserNtriples::ntriples_term_valid(unsigned char c, int position, term_class term_class)
{
	int result = 0;

	switch(term_class) {
	case TERM_CLASS_URI:
		/* ends on > */
		result = (c != '>');
		break;

	case TERM_CLASS_BNODEID:
		/* ends on first non [A-Za-z][A-Za-z0-9]* */
		result = IS_ASCII_ALPHA(c);
		if(position)
			result = (result || IS_ASCII_DIGIT(c));
		break;

	case TERM_CLASS_STRING:
		/* ends on " */
		result = (c != '"');
		break;

	case TERM_CLASS_LANGUAGE:
		/* ends on first non [a-z0-9]+ ('-' [a-z0-9]+ )? */
		result = (IS_ASCII_ALPHA(c) || IS_ASCII_DIGIT(c));
		if(position)
			result = (result || c == '-');
		break;

	case TERM_CLASS_FULL:
		result = 1;
		break;

	default:
		throw "Unknown N-Triples term class" + term_class;
	}

	return result;
}


/*
 * raptor_ntriples_term:
 * @parser: NTriples parser
 * @start: pointer to starting character of string (in)
 * @dest: destination of string (in)
 * @lenp: pointer to length of string (in/out)
 * @dest_lenp: pointer to length of destination string (out)
 * @end_char: string ending character
 * @class: string class
 * @allow_utf8: Non-0 if UTF-8 chars are allowed in the term
 *
 * Parse an N-Triples term with escapes.
 *
 * Relies that @dest is long enough; it need only be as large as the
 * input string @start since when UTF-8 encoding, the escapes are
 * removed and the result is always less than or equal to length of
 * input.
 *
 * N-Triples strings/URIs are written in ASCII at present; characters
 * outside the printable ASCII range are discarded with a warning.
 * See the grammar for full details of the allowed ranges.
 *
 * If the class is TERM_CLASS_FULL, the end_char is ignored.
 *
 * UTF-8 is only allowed if allow_utf8 is non-0, otherwise the
 * string is US-ASCII and only the \u and \U esapes are allowed.
 * If enabled, both are allowed.
 *
 * Return value: Non 0 on failure
 **/
int RDFParserNtriples::ntriples_term(const unsigned char **start, unsigned char *dest,
		unsigned int *lenp, unsigned int *dest_lenp,
		char end_char,
		term_class term_class,
		int allow_utf8)
{
	const unsigned char *p = *start;
	unsigned char c = '\0';
	unsigned int ulen = 0;
	unsigned long unichar = 0;
	unsigned int position = 0;
	int end_char_seen = 0;

	if(term_class == TERM_CLASS_FULL)
		end_char = '\0';

	/* find end of string, fixing backslashed characters on the way */
	while(*lenp > 0) {
		c = *p;

		p++;
		(*lenp)--;
		column++;
		byte++;

		if(allow_utf8) {
			if(c > 0x7f) {
				/* just copy the UTF-8 bytes through */
				unsigned int unichar_len;
				unichar_len = raptor_unicode_utf8_string_get_char(p - 1, 1 + *lenp, NULL);
				if(unichar_len > *lenp) {
					//raptor_parser_error("UTF-8 encoding error at character %d (0x%02X) found.", c, c);
					/* UTF-8 encoding had an error or ended in the middle of a string */
					return 1;
				}
				memcpy(dest, p-1, unichar_len);
				dest += unichar_len;

				unichar_len--; /* p, *lenp were moved on by 1 earlier */

				p += unichar_len;
				(*lenp) -= unichar_len;
				column += unichar_len;
				byte += unichar_len;
				continue;
			}
		} else if(!IS_ASCII_PRINT(c)) {
			/* This is an ASCII check, not a printable character check
			 * so isprint() is not appropriate, since that is a locale check.
			 */
			//raptor_parser_error("Non-printable ASCII character %d (0x%02X) found.", c, c);
			continue;
		}

		if(c != '\\') {
			/* finish at non-backslashed end_char */
			if(end_char && c == end_char) {
				end_char_seen = 1;
				break;
			}

			if(!ntriples_term_valid(c, position, term_class)) {
				if(end_char) {
					/* end char was expected, so finding an invalid thing is an error */
					//raptor_parser_error("Missing terminating '%c' (found '%c')", end_char, c);
					return 0;
				} else {
					/* it's the end - so rewind 1 to save next char */
					p--;
					(*lenp)++;
					column--;
					byte--;
					break;
				}
			}

			/* otherwise store and move on */
			*dest++ = c;
			position++;
			continue;
		}

		if(!*lenp) {
			if(term_class != TERM_CLASS_FULL) {
				//raptor_parser_error("\\ at end of lineNum");
			}
			return 0;
		}

		c = *p;

		p++;
		(*lenp)--;
		column++;
		byte++;

		switch(c) {
		case '"':
		case '\\':
			*dest++ = c;
			break;
		case 'n':
			*dest++ = '\n';
			break;
		case 'r':
			*dest++ = '\r';
			break;
		case 't':
			*dest++ = '\t';
			break;
		case 'u':
		case 'U':
			ulen = (c == 'u') ? 4 : 8;

			if(*lenp < ulen) {
				//raptor_parser_error("%c over end of lineNum", c);
				return 0;
			}

			if(1) {
				int n;

				n = sscanf((const char*)p, ((ulen == 4) ? "%04lx" : "%08lx"), &unichar);
				if(n != 1) {
					//raptor_parser_error("Illegal Uncode escape '%c%s...'", c, p);
					break;
				}
			}

			p += ulen;
			(*lenp) -= ulen;
			column += ulen;
			byte += ulen;

			if(unichar > raptor_unicode_max_codepoint) {
				//raptor_parser_error(rdf_parser, "Illegal Unicode character with code point #x%lX (max #x%lX).", unichar, raptor_unicode_max_codepoint);
				break;
			}

			/* The destination length is set here to 4 since we know that in
			 * all cases, the UTF-8 encoded output sequence is always shorter
			 * than the input sequence, and the buffer is edited in place.
			 *   \uXXXX: 6 bytes input - UTF-8 max 3 bytes output
			 *   \uXXXXXXXX: 10 bytes input - UTF-8 max 4 bytes output
			 */
			dest += raptor_unicode_utf8_string_put_char(unichar, dest, 4);
			break;

		default:
			//raptor_parser_error(rdf_parser, "Illegal string escape \\%c in \"%s\"", c, (char*)start);
			return 0;
		}

		position++;
	} /* end while */


	if(end_char && !end_char_seen) {
		//raptor_parser_error("Missing terminating '%c' before end of lineNum.", end_char);
		return 1;
	}

	/* terminate dest, can be shorter than source */
	*dest = '\0';

	if(dest_lenp)
		*dest_lenp = p - *start;

	*start = p;

	return 0;
}

string getTermString(unsigned char *buffer, unsigned int length, term_type type, unsigned char *language, unsigned char *datatype) {
	string str;
	switch(type) {
	case TERM_TYPE_URI:
		str.append("<");
		str.append((char *)buffer, length);
		str.append(">");
		break;
	case TERM_TYPE_BLANK:
		str.append((char *)buffer, length);
		break;
	case TERM_TYPE_LITERAL:
		str.append("\"");
		str.append((char *)buffer,length);
		str.append("\"");
		if(datatype){
			str.append("^^");
			str.append((char *)datatype);
		}
		if(language) {
			str.append("@");
			str.append((char *)language);
		}
		break;
	}
	return str;
}


#define MAX_NTRIPLES_TERMS 4

int RDFParserNtriples::parse_line(unsigned char *buffer, unsigned int len, int max_terms)
{
	int i;
	unsigned char *p;
	unsigned char *dest;
	unsigned char *terms[MAX_NTRIPLES_TERMS] = { NULL, NULL, NULL, NULL};
	unsigned int term_lengths[MAX_NTRIPLES_TERMS] = {0, 0, 0, 0};
	term_type term_types[MAX_NTRIPLES_TERMS] = {TERM_TYPE_UNKNOWN, TERM_TYPE_UNKNOWN, TERM_TYPE_UNKNOWN, TERM_TYPE_UNKNOWN};
	unsigned int term_length = 0;
	unsigned char *object_literal_language = NULL;
	unsigned char *object_literal_datatype = NULL;
	int rc = 0;

	/* ASSERTION:
	 * p always points to first char we are considering
	 * p[len-1] always points to last char
	 */

	/* Handle empty  lineNums */
	if(!len)
		return 0;

	p = buffer;

	while(len > 0 && isspace((int)*p)) {
		p++;
		column++;
		byte++;
		len--;
	}

	/* Handle empty - all whitespace lineNums */
	if(!len)
		return 0;

	/* Handle comment lineNums */
	if(*p == '#')
		return 0;

	/* Remove trailing spaces */
	while(len > 0 && isspace((int)p[len-1])) {
		p[len-1] = '\0';
		len--;
	}

	/* can't be empty now - that would have been caught above */

	/* Check for terminating '.' */
	if(p[len-1] != '.') {
		/* Move current location to point to problem */
		column += len-2;
		byte += len-2;
		//raptor_parser_error("Missing . at end of lineNum");
		return 0;
	}

	p[len-1] = '\0';
	len--;


	/* Must be triple */

	for(i = 0; i < max_terms; i++) {
		if(!len) {
			//raptor_parser_error("Unexpected end of lineNum");
			goto cleanup;
		}


		if(i == 2) {
			/* object term (2): expect either <URI> or _:name or literal */
			if(*p != '<' && *p != '_' && *p != '"' && *p != 'x') {
				//raptor_parser_error("Saw '%c', expected <URIref>, _:bnodeID or \"literal\"", *p);
				goto cleanup;
			}
		} else if(i == 1) {
			/* predicate term (1): expect URI only */
			if(*p != '<') {
				//raptor_parser_error("Saw '%c', expected <URIref>", *p);
				goto cleanup;
			}
		} else {
			/* subject (0) or graph (3) terms: expect <URI> or _:name */
			if(*p != '<' && *p != '_') {
				//raptor_parser_error("Saw '%c', expected <URIref> or _:bnodeID", *p);
				goto cleanup;
			}
		}


		switch(*p) {
		case '<':
			term_types[i] = TERM_TYPE_URI;

			dest = p;

			p++;
			len--;
			column++;
			byte++;

			if(ntriples_term((const unsigned char**)&p, dest, &len, &term_length, '>', TERM_CLASS_URI, 0)) {
				rc = 1;
				goto cleanup;
			}
			break;

		case '"':
			term_types[i] = TERM_TYPE_LITERAL;

			dest = p;

			p++;
			len--;
			column++;
			byte++;

			if(ntriples_term((const unsigned char**)&p,	dest, &len, &term_length, '"', TERM_CLASS_STRING, 0)) {
				rc = 1;
				goto cleanup;
			}

			if(len && (*p == '-' || *p == '@')) {
				if(*p == '-') {
					//raptor_parser_error("Old N-Triples language syntax using \"string\"-lang rather than \"string\"@lang.");
				}
				object_literal_language = p;

				/* Skip - */
				p++;
				len--;
				column++;
				byte++;

				if(!len) {
					//raptor_parser_error("Missing language after \"string\"-");
					goto cleanup;
				}


				if(ntriples_term(
						(const unsigned char**)&p,
						object_literal_language, &len, NULL,
						'\0', TERM_CLASS_LANGUAGE, 0)) {
					rc = 1;
					goto cleanup;
				}
			}

			if(len >1 && *p == '^' && p[1] == '^') {

				object_literal_datatype = p;

				/* Skip ^^ */
				p += 2;
				len -= 2;
				column += 2;
				byte += 2;

				if(!len || (len && *p != '<')) {
					//raptor_parser_error("Missing datatype URI-ref in\"string\"^^<URI-ref> after ^^");
					goto cleanup;
				}

				p++;
				len--;
				column++;
				byte++;

				if(ntriples_term(
						(const unsigned char**)&p,
						object_literal_datatype, &len, NULL,
						'>', TERM_CLASS_URI, 0)) {
					rc = 1;
					goto cleanup;
				}

			}

			if(object_literal_datatype && object_literal_language) {
				//raptor_parser_warning("Typed literal used with a language - ignoring the language");
				object_literal_language = NULL;
			}

			break;


		case '_':
			term_types[i] = TERM_TYPE_BLANK;

			/* store where _ was */
			dest = p;

			p++;
			len--;
			column++;
			byte++;

			if(!len || (len > 0 && *p != ':')) {
				//raptor_parser_error("Illegal bNodeID - _ not followed by :");
				goto cleanup;
			}

			/* Found ':' - move on */

			p++;
			len--;
			column++;
			byte++;

			if(ntriples_term(
					(const unsigned char**)&p,
					dest, &len, &term_length,
					'\0', TERM_CLASS_BNODEID, 0)) {
				rc = 1;
				goto cleanup;
			}

			if(!term_length) {
				//raptor_parser_error("Bad or missing bNodeID after _:");
				goto cleanup;
			}

			break;

		default:
			//raptor_parser_fatal_error("Unknown term type");
			rc = 1;
			goto cleanup;
		}


		/* Store term */
		terms[i] = dest; term_lengths[i] = term_length;

		/* Whitespace must separate the terms */
		if(i < 2 && !isspace((int)*p)) {
			//raptor_parser_error("Missing whitespace after term '%s'", terms[i]);
			rc = 1;
			goto cleanup;
		}

		/* Skip whitespace after terms */
		while(len > 0 && isspace((int)*p)) {
			p++;
			len--;
			column++;
			byte++;
		}
	}

	if(len) {
		//raptor_parser_error("Junk before terminating \".\"");
		return 0;
	}


	if(object_literal_language) {
		unsigned char *q;
		/* Normalize language to lowercase
		 * http://www.w3.org/TR/rdf-concepts/#dfn-language-identifier
		 */
		for(q = object_literal_language; *q; q++) {
			if(IS_ASCII_UPPER(*q))
				*q = TO_ASCII_LOWER(*q);
		}
	}


	/* Just to be sure */
	if(!isNQuad)
		terms[3] = NULL;


	{
		string subj	= getTermString(terms[0], term_lengths[0], term_types[0], NULL, NULL);
		string pred	= getTermString(terms[1], term_lengths[1], term_types[1], NULL, NULL);
		string obj	= getTermString(terms[2], term_lengths[2], term_types[2], object_literal_language, object_literal_datatype);
		ts.setAll(subj, pred, obj);
	}

	/*
  raptor_ntriples_generate_statement(
                                     terms[0], term_types[0],
                                     terms[1], term_types[1],
                                     terms[2], term_types[2],
                                     object_literal_language,
                                     object_literal_datatype,
                                     terms[3], term_types[3]);
	 */

	byte += len;

	cleanup:

	return rc;
}

bool RDFParserNtriples::hasNext() {
	getline(*input, lineStr);
	return lineStr!="";
}

TripleString *RDFParserNtriples::next() {
	unsigned char *buf = new unsigned char [lineStr.length()+1];
	strcpy((char *)buf, lineStr.c_str());
	parse_line(buf, lineStr.length(), 3);
	delete [] buf;
	return &ts;
}

void RDFParserNtriples::reset() {
	input->clear(); // Resets EOF
	input->seekg(0, std::ios::beg);
	lineNum=1;
	column=0;
	byte=0;
}

uint64_t RDFParserNtriples::getPos(){
	return input->tellg();
}

uint64_t RDFParserNtriples::getSize() {
	return size;
}


}
