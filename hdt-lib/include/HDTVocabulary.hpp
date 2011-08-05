/*
 * HDTVocabulary.hpp
 *
 *  Created on: 05/06/2011
 *      Author: mck
 */

#ifndef HDTVOCABULARY_HPP_
#define HDTVOCABULARY_HPP_

#include <string>

namespace hdt {
namespace HDTVocabulary {
	const std::string HDT_BASE = "<http://purl.org/HDT/hdt#";
	const std::string HDT_HEADER = HDT_BASE+"header";
	const std::string HDT_DICTIONARY_BASE = HDT_BASE+"dictionary";
	const std::string HDT_TRIPLES_BASE = HDT_BASE+"triples";
	const std::string HDT_STREAM_BASE = HDT_BASE+"stream";

	const std::string HDT_DATASET = HDT_BASE+"Dataset>";
	const std::string HDT_FORMAT_INFORMATION = HDT_BASE+"formatInformation>";
	const std::string HDT_STATISTICAL_INFORMATION = HDT_BASE+"statisticalInformation>";
	const std::string HDT_PUBLICATION_INFORMATION = HDT_BASE+"publicationInformation>";
	const std::string HDT_DICTIONARY = HDT_DICTIONARY_BASE+">";
	const std::string HDT_TRIPLES = HDT_TRIPLES_BASE+">";

	const std::string RDF = "<http://www.w3.org/1999/02/22-rdf-syntax-ns#";
	const std::string RDF_TYPE = RDF+"type>";
	const std::string DUBLIN_CORE = "<http://purl.org/dc/terms/";

	const std::string DUBLIN_CORE_ISSUED = DUBLIN_CORE+"issued>";


	// Header
	const std::string HEADER_PLAIN = HDT_HEADER+"plain>";

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
	const std::string DICTIONARY_BLOCK_SIZE = HDT_DICTIONARY_BASE+"numBlocks>";

	// Dictionary Types
	const std::string DICTIONARY_TYPE_PLAIN = HDT_DICTIONARY_BASE+"Plain>";
	const std::string DICTIONARY_TYPE_PFC = HDT_DICTIONARY_BASE+"FrontCoding>";

	// Triples
	const std::string TRIPLES_TYPE = DUBLIN_CORE+"format>";
	const std::string TRIPLES_NUM_TRIPLES = HDT_TRIPLES_BASE+"numTriples>";
	const std::string TRIPLES_ORDER = HDT_TRIPLES_BASE+"streamOrder>";
	const std::string TRIPLES_STREAMX_TYPE = HDT_TRIPLES_BASE+"streamX>";
	const std::string TRIPLES_STREAMY_TYPE = HDT_TRIPLES_BASE+"streamY>";
	const std::string TRIPLES_STREAMZ_TYPE = HDT_TRIPLES_BASE+"streamZ>";
	const std::string TRIPLES_STREAMX_SIZE = HDT_TRIPLES_BASE+"streamXsize>";
	const std::string TRIPLES_STREAMY_SIZE = HDT_TRIPLES_BASE+"streamYsize>";
	const std::string TRIPLES_STREAMZ_SIZE = HDT_TRIPLES_BASE+"streamZsize>";
	const std::string TRIPLES_BITMAPX_SIZE = HDT_TRIPLES_BASE+"bitmapXsize>";
	const std::string TRIPLES_BITMAPY_SIZE = HDT_TRIPLES_BASE+"bitmapYsize>";
	const std::string TRIPLES_BITMAPZ_SIZE = HDT_TRIPLES_BASE+"bitmapZsize>";

	// Triples types
	const std::string TRIPLES_TYPE_TRIPLESLIST = HDT_TRIPLES_BASE+"List>";
	const std::string TRIPLES_TYPE_TRIPLESLISTDISK = HDT_TRIPLES_BASE+"ListDisk>";
	const std::string TRIPLES_TYPE_PLAIN = HDT_TRIPLES_BASE+"Plain>";
	const std::string TRIPLES_TYPE_COMPACT = HDT_TRIPLES_BASE+"Compact>";
	const std::string TRIPLES_TYPE_BITMAP = HDT_TRIPLES_BASE+"Bitmap>";
	const std::string TRIPLES_TYPE_FOQ = HDT_TRIPLES_BASE+"FOQ>";

	// Streams
	const std::string STREAM_TYPE_INTEGER = HDT_STREAM_BASE+"Integer>";
	const std::string STREAM_TYPE_LOG = HDT_STREAM_BASE+"Log>";
	const std::string STREAM_TYPE_HUFFMAN = HDT_STREAM_BASE+"Huffman>";
	const std::string STREAM_TYPE_WAVELET = HDT_STREAM_BASE+"Wavelet>";

        // OTHER
	const std::string ORIGINAL_SIZE = HDT_BASE+"originalSize>";
	const std::string HDT_SIZE = HDT_BASE+"hdtSize>";
}
}


#endif /* HDTVOCABULARY_HPP_ */
