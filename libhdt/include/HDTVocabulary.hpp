/*
 * File: HDTVocabulary.hpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
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
 * Lesser General Public License for more details.
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

#ifndef HDT_HDTVOCABULARY_HPP_
#define HDT_HDTVOCABULARY_HPP_

#include <string>
#include <HDTVersion.hpp>

namespace hdt {
namespace HDTVocabulary {
	// Base
	const std::string HDT_BASE = "<http://purl.org/HDT/hdt#";
	const std::string HDT_CONTAINER = HDT_BASE+"HDTv" + HDT_VERSION + ">";
	const std::string HDT_HEADER = HDT_BASE+"header";
	const std::string HDT_DICTIONARY_BASE = HDT_BASE+"dictionary";
	const std::string HDT_DICTIONARY = HDT_DICTIONARY_BASE+">";
	const std::string HDT_TRIPLES_BASE = HDT_BASE+"triples";
	const std::string HDT_TRIPLES = HDT_TRIPLES_BASE+">";
	const std::string HDT_SEQ_BASE = HDT_BASE+"seq";
	const std::string HDT_BITMAP_BASE = HDT_BASE+"bitmap";

    // External vocabularies
	const std::string RDF = "<http://www.w3.org/1999/02/22-rdf-syntax-ns#";
	const std::string RDF_TYPE = RDF+"type>";
	const std::string DUBLIN_CORE = "<http://purl.org/dc/terms/";
	const std::string DUBLIN_CORE_ISSUED = DUBLIN_CORE+"issued>";

	// VOID
	const std::string VOID_BASE ="<http://rdfs.org/ns/void#";
	const std::string VOID_DATASET = VOID_BASE + "Dataset>";
	const std::string VOID_TRIPLES = VOID_BASE + "triples>";
	const std::string VOID_ENTITIES = VOID_BASE +"entities>";
	const std::string VOID_CLASSES = VOID_BASE +"classes>";
	const std::string VOID_PROPERTIES = VOID_BASE +"properties>";
	const std::string VOID_DISTINCT_SUBJECTS = VOID_BASE +"distinctSubjects>";
	const std::string VOID_DISTINCT_OBJECTS = VOID_BASE +"distinctObjects>";

	// Header
	const std::string HEADER_NTRIPLES = "ntriples";
	const std::string HDT_DATASET = HDT_BASE+"Dataset>";
	const std::string HDT_FORMAT_INFORMATION = HDT_BASE+"formatInformation>";
	const std::string HDT_STATISTICAL_INFORMATION = HDT_BASE+"statisticalInformation>";
	const std::string HDT_PUBLICATION_INFORMATION = HDT_BASE+"publicationInformation>";

	// Dictionary
	const std::string DICTIONARY_TYPE = DUBLIN_CORE+"format>";
	const std::string DICTIONARY_NUMSUBJECTS = HDT_DICTIONARY_BASE+"numSubjects>";
	const std::string DICTIONARY_NUMPREDICATES = HDT_DICTIONARY_BASE+"numPredicates>";
	const std::string DICTIONARY_NUMOBJECTS = HDT_DICTIONARY_BASE+"numObjects>";
	const std::string DICTIONARY_NUMSHARED = HDT_DICTIONARY_BASE+"numSharedSubjectObject>";
	const std::string DICTIONARY_MAXSUBJECTID = HDT_DICTIONARY_BASE+"maxSubjectID>";
	const std::string DICTIONARY_MAXPREDICATEID = HDT_DICTIONARY_BASE+"maxPredicateID>";
	const std::string DICTIONARY_MAXOBJECTTID = HDT_DICTIONARY_BASE+"maxObjectID>";
	const std::string DICTIONARY_MAPPING = HDT_DICTIONARY_BASE+"mapping>";
	const std::string DICTIONARY_SIZE_STRINGS = HDT_DICTIONARY_BASE+"sizeStrings>";
	const std::string DICTIONARY_BLOCK_SIZE = HDT_DICTIONARY_BASE+"blockSize>";

	// Dictionary Types
	const std::string DICTIONARY_TYPE_PLAIN = HDT_DICTIONARY_BASE+"Plain>";
	const std::string DICTIONARY_TYPE_FOUR = HDT_DICTIONARY_BASE+"Four>";
	const std::string DICTIONARY_TYPE_HTFC = HDT_DICTIONARY_BASE+"FrontCodingHuTucker>";
	const std::string DICTIONARY_TYPE_LITERAL = HDT_DICTIONARY_BASE+"Literal>";

	// Triples
	const std::string TRIPLES_TYPE = DUBLIN_CORE+"format>";
	const std::string TRIPLES_NUM_TRIPLES = HDT_TRIPLES_BASE+"numTriples>";
	const std::string TRIPLES_ORDER = HDT_TRIPLES_BASE+"Order>";
	const std::string TRIPLES_SEQX_TYPE = HDT_TRIPLES_BASE+"seqX>";
	const std::string TRIPLES_SEQY_TYPE = HDT_TRIPLES_BASE+"seqY>";
	const std::string TRIPLES_SEQZ_TYPE = HDT_TRIPLES_BASE+"seqZ>";
	const std::string TRIPLES_SEQX_SIZE = HDT_TRIPLES_BASE+"seqXsize>";
	const std::string TRIPLES_SEQY_SIZE = HDT_TRIPLES_BASE+"seqYsize>";
	const std::string TRIPLES_SEQZ_SIZE = HDT_TRIPLES_BASE+"seqZsize>";
	const std::string TRIPLES_BITMAPX_SIZE = HDT_TRIPLES_BASE+"bitmapXsize>";
	const std::string TRIPLES_BITMAPY_SIZE = HDT_TRIPLES_BASE+"bitmapYsize>";
	const std::string TRIPLES_BITMAPZ_SIZE = HDT_TRIPLES_BASE+"bitmapZsize>";

	// Triples types
	const std::string TRIPLES_TYPE_TRIPLESLIST = HDT_TRIPLES_BASE+"List>";
	const std::string TRIPLES_TYPE_TRIPLESLISTDISK = HDT_TRIPLES_BASE+"ListDisk>";
	const std::string TRIPLES_TYPE_PLAIN = HDT_TRIPLES_BASE+"Plain>";
	const std::string TRIPLES_TYPE_BITMAP = HDT_TRIPLES_BASE+"Bitmap>";
	const std::string TRIPLES_TYPE_KYOTO = HDT_TRIPLES_BASE+"KYOTO>";

	// Index types
	const std::string INDEX_TYPE_FOQ = HDT_BASE+"indexFoQ>";

	// Sequences
	const std::string SEQ_TYPE_INT32 = HDT_SEQ_BASE+"Int32>";
	const std::string SEQ_TYPE_INT64 = HDT_SEQ_BASE+"Int64>";
	const std::string SEQ_TYPE_LOG = HDT_SEQ_BASE+"Log>";
	const std::string SEQ_TYPE_LOG2 = HDT_SEQ_BASE+"Log2>";
	const std::string SEQ_TYPE_HUFFMAN = HDT_SEQ_BASE+"Huffman>";
	const std::string SEQ_TYPE_WAVELET = HDT_SEQ_BASE+"Wavelet>";

	// Bitmaps
	const std::string BITMAP_TYPE_PLAIN = HDT_BITMAP_BASE+"Plain>";

	// Misc
	const std::string ORIGINAL_SIZE = HDT_BASE+"originalSize>";
	const std::string HDT_SIZE = HDT_BASE+"hdtSize>";
}
}

#endif /* HDT_HDTVOCABULARY_HPP_ */
