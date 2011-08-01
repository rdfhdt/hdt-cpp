/*
 * RDFParserNtriples.hpp
 *
 *  Created on: 08/07/2011
 *      Author: mck
 */

#ifndef RDFPARSERNTRIPLES_HPP_
#define RDFPARSERNTRIPLES_HPP_

#include "RDFParser.hpp"

namespace hdt {

typedef enum {
  TERM_TYPE_UNKNOWN = 0,
  TERM_TYPE_URI     = 1,
  TERM_TYPE_LITERAL = 2,
  /* unused type 3 */
  TERM_TYPE_BLANK   = 4
} term_type;

typedef enum {
  TERM_CLASS_URI,      /* ends on > */
  TERM_CLASS_BNODEID,  /* ends on first non [A-Za-z][A-Za-z0-9]* */
  TERM_CLASS_STRING,   /* ends on non-escaped " */
  TERM_CLASS_LANGUAGE, /* ends on first non [a-z0-9]+ ('-' [a-z0-9]+ )? */
  TERM_CLASS_FULL      /* the entire string is used */
} term_class;

class RDFParserNtriples : public RDFParser {
private:
	uint64_t size;
	std::string lineStr;
	TripleString ts;

	unsigned int byte;
	unsigned int column;
	unsigned int lineNum;
	unsigned int line_length;
	bool isNQuad;

private:

	int ntriples_term_valid(unsigned char c, int position,
	                           term_class term_class);

	int	ntriples_term(const unsigned char **start, unsigned char *dest,
	                     unsigned int *lenp, unsigned int *dest_lenp,
	                     char end_char,
	                     term_class term_class,
	                     int allow_utf8);

	int parse_line(unsigned char *buffer, unsigned int len, int max_terms);

public:
	RDFParserNtriples(std::istream &istream, RDFNotation notation);
	virtual ~RDFParserNtriples();

	bool hasNext();
	TripleString *next();
	void reset();
	uint64_t getPos();
	uint64_t getSize();
};

}

#endif /* RDFPARSERNTRIPLES_HPP_ */
