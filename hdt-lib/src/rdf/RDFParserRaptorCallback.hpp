/*
 * RDFParserN3.h
 *
 *  Created on: 05/03/2011
 *      Author: mck
 */

#ifndef RDFPARSERRAPTORCALLBACK_H_
#define RDFPARSERRAPTORCALLBACK_H_

#include <stdint.h>

#include <raptor2/raptor2.h>

#include <HDTEnums.hpp>

#include "RDFParser.hpp"

namespace hdt {

class RDFParserRaptorCallback {

private:
	raptor_parser *rdf_parser;

	RDFCallback *callback;

	const char *getParserType(RDFNotation notation);
public:
	RDFParserRaptorCallback();
	virtual ~RDFParserRaptorCallback();

	void doParse(const char *fileName, RDFNotation notation, RDFCallback *callback);

	friend void raptor_callback_process_triple(void *user_data, raptor_statement *triple);
	friend void raptor_callback_log_handler(void *user_data, raptor_log_message *message);
};



}

#endif
