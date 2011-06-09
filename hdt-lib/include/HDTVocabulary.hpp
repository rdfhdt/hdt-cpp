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
	const std::string HDT_DICTIONARY = HDT_BASE+"dictionary";
	const std::string HDT_TRIPLES = HDT_BASE+"triples";
	const std::string HDT_STREAM = HDT_BASE+"stream";

	// Header
	const std::string HEADER_PLAIN = HDT_HEADER+"plain>";

	// Dictionary
	const std::string DICTIONARY_TYPE = HDT_DICTIONARY+"codification>";
	const std::string DICTIONARY_NUMSUBJECTS = HDT_DICTIONARY+"numSubjects>";
	const std::string DICTIONARY_NUMPREDICATES = HDT_DICTIONARY+"numPredicates>";
	const std::string DICTIONARY_NUMOBJECTS = HDT_DICTIONARY+"numObjects>";
	const std::string DICTIONARY_NUMSHARED = HDT_DICTIONARY+"numSharedSubjectObject>";
	const std::string DICTIONARY_MAXSUBJECTID = HDT_DICTIONARY+"maxSubjectID>";
	const std::string DICTIONARY_MAXPREDICATEID = HDT_DICTIONARY+"maxPredicateID>";
	const std::string DICTIONARY_MAXOBJECTTID = HDT_DICTIONARY+"maxObjectID>";
	const std::string DICTIONARY_MAPPING = HDT_DICTIONARY+"mapping>";

	// Dictionary Types
	const std::string DICTIONARY_TYPE_PLAIN = HDT_DICTIONARY+"Plain>";

	// Triples
	const std::string TRIPLES_TYPE = HDT_TRIPLES+"codification>";
	const std::string TRIPLES_NUM_TRIPLES = HDT_TRIPLES+"numTriples>";
	const std::string TRIPLES_ORDER = HDT_TRIPLES+"streamOrder>";
	const std::string TRIPLES_STREAMX_TYPE = HDT_TRIPLES+"streamX>";
	const std::string TRIPLES_STREAMY_TYPE = HDT_TRIPLES+"streamY>";
	const std::string TRIPLES_STREAMZ_TYPE = HDT_TRIPLES+"streamZ>";

	// Triples types
	const std::string TRIPLES_TYPE_TRIPLESLIST = HDT_TRIPLES+"List>";
	const std::string TRIPLES_TYPE_TRIPLESLISTDISK = HDT_TRIPLES+"ListDisk>";
	const std::string TRIPLES_TYPE_PLAIN = HDT_TRIPLES+"Plain>";
	const std::string TRIPLES_TYPE_COMPACT = HDT_TRIPLES+"Compact>";
	const std::string TRIPLES_TYPE_BITMAP = HDT_TRIPLES+"Bitmap>";
	const std::string TRIPLES_TYPE_FOQ = HDT_TRIPLES+"FOQ>";

	// Streams
	const std::string STREAM_TYPE_INTEGER = HDT_STREAM+"Integer>";
	const std::string STREAM_TYPE_LOG = HDT_STREAM+"Log>";
	const std::string STREAM_TYPE_HUFFMAN = HDT_STREAM+"Huffman>";
	const std::string STREAM_TYPE_WAVELET = HDT_STREAM+"Wavelet>";
}
}


#endif /* HDTVOCABULARY_HPP_ */
