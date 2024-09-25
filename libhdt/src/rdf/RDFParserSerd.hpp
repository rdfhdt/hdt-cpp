/*
 * File: RDFParserSerd.hpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 * Copyright (C) 2012, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
 * All rights reserved.
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
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */
#ifndef RDFPARSERSERD_HPP
#define RDFPARSERSERD_HPP

#ifdef HAVE_SERD

#include <stdint.h>

#include <serd/serd.h>

#include <HDTEnums.hpp>

#include "RDFParser.hpp"

namespace hdt {

class RDFParserSerd : public RDFParserCallback {

private:
	SerdEnv *env;
	RDFCallback *callback;
	string error;
	uint64_t numByte;
	std::string bnode_prefix;

	string getString(const SerdNode *term);
	string getStringObject(const SerdNode *term, const SerdNode *dataType, const SerdNode *lang);
	SerdSyntax getParserType(RDFNotation notation);

public:
	RDFParserSerd(const std::string& bnode_prefix);
	virtual ~RDFParserSerd();

	void doParse(const char *fileName, const char *baseUri, RDFNotation notation, bool ignoreErrors, RDFCallback *callback);

	friend SerdStatus hdtserd_on_statement(void               *handle,
	                                       SerdStatementFlags  flags,
	                                       const SerdNode     *graph,
	                                       const SerdNode     *subject,
	                                       const SerdNode     *predicate,
	                                       const SerdNode     *object,
	                                       const SerdNode     *datatype,
	                                       const SerdNode     *lang);

	friend SerdStatus hdtserd_on_prefix(void           *handle,
	                                    const SerdNode *name,
	                                    const SerdNode *uri);

	friend SerdStatus hdtserd_on_base(void *handle, const SerdNode *uri);
	friend SerdStatus hdtserd_on_error(void *handle, const SerdError *e);
};

}

#endif

#endif // RDFPARSERSERD_HPP
