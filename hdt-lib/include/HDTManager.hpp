/*
 * File: HDTManager.hpp
 * Last modified: $Date: 2012-08-21 21:15:01 +0100 (mar, 21 ago 2012) $
 * Revision: $Revision: 250 $
 * Last modified by: $Author: mario.arias $
 *
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
 * Lesser General License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#ifndef HDT_HDTMANAGER_HPP_
#define HDT_HDTMANAGER_HPP_

#include "HDTSpecification.hpp"
#include "HDT.hpp"
#include "HDTListener.hpp"

#include <string>

namespace hdt {

/**
 * Manager of HDT classes
 */
class HDTManager {

public:
	/**
	 * Reads an HDT file into main memory.
	 */
	static HDT *loadHDT(const char *file, ProgressListener *listener=NULL);

	/**
	 * Load an HDT File, and load/create additional indexes to support all kind of queries efficiently.
	 * @param hdtFileName
	 * @param listener Listener to get notified of loading progress. Can be null if no notifications needed.
	 * @return
	 * @throws IOException
	 */
	static HDT *loadIndexedHDT(const char *hdtFileName, ProgressListener *listener=NULL);

	/**
	 * Maps an HDT file. Faster to load, uses less memory, but may result in delays at access time.
	 */
	static HDT *mapHDT(const char *file, ProgressListener *listener=NULL);

	/**
	 * Maps an HDT file and load/generate additional indexes to solve efficiently all queries.
	 */
	static HDT *mapIndexedHDT(const char *file, ProgressListener *listener=NULL);

	/**
	 * Return an indexed HDT that is efficient for all kind of queries, given a (possibly) not indexed HDT.
	 * @param hdt
	 * @param listener Listener to get notified of loading progress. Can be null if no notifications needed.
	 * @return
	 */
	static HDT *indexedHDT(HDT *hdt, ProgressListener *listener=NULL);

	/**
	 * Create an HDT file from an RDF file.
	 * @param rdfFileName File name.
	 * @param baseURI Base URI for the dataset.
	 * @param rdfNotation Format of the source RDF File (NTriples, N3, RDF-XML...)
	 * @param hdtFormat Parameters to tune the generated HDT.
	 * @param listener Listener to get notified of loading progress. Can be null if no notifications needed.
	 * @return
	 * @throws IOException
	 * @throws ParserException
	 */
	static HDT *generateHDT(const char *rdfFileName, const char *baseURI, RDFNotation rdfNotation, HDTSpecification &hdtFormat, ProgressListener *listener=NULL);
};
}

#endif /* HDT_HDTMANAGER_HPP_ */
