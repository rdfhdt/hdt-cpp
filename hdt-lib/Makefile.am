SUBDIRS = . tools tests 

AUTOMAKE_OPTIONS = subdir-objects

include_HEADERS = \
	include/ControlInformation.hpp\
	include/Dictionary.hpp\
	include/HDTEnums.hpp\
	include/HDT.hpp\
	include/HDTListener.hpp\
	include/HDTManager.hpp\
	include/HDTSpecification.hpp\
	include/HDTVersion.hpp\
	include/HDTVocabulary.hpp\
	include/Header.hpp\
	include/Iterator.hpp\
	include/RDF.hpp\
	include/RDFParser.hpp\
	include/RDFSerializer.hpp\
	include/SingleTriple.hpp\
	include/Triples.hpp

lib_LTLIBRARIES = libhdt.la
libhdt_la_SOURCES = \
src/bitsequence/BitSequence375.cpp \
src/dictionary/FourSectionDictionary.cpp \
src/dictionary/KyotoDictionary.cpp \
src/dictionary/LiteralDictionary.cpp \
src/dictionary/PlainDictionary.cpp \
src/hdt/BasicHDT.cpp \
src/hdt/BasicModifiableHDT.cpp \
src/hdt/ControlInformation.cpp \
src/hdt/HDTFactory.cpp \
src/hdt/HDTManager.cpp \
src/hdt/HDTSpecification.cpp \
src/hdt/TripleIDStringIterator.cpp \
src/header/PlainHeader.cpp \
src/huffman/huff.cpp \
src/huffman/Huffman.cpp \
src/libdcs/CSD_Cache2.cpp \
src/libdcs/CSD_Cache.cpp \
src/libdcs/CSD.cpp \
src/libdcs/CSD_FMIndex.cpp \
src/libdcs/CSD_HTFC.cpp \
src/libdcs/CSD_PFC.cpp \
src/libdcs/VByte.cpp \
src/libdcs/fmindex/SSA.cpp \
src/libdcs/fmindex/SuffixArray.cpp \
src/rdf/RDFParser.cpp \
src/rdf/RDFParserNtriples.cpp \
src/rdf/RDFParserRaptorCallback.cpp \
src/rdf/RDFParserSerd.cpp \
src/rdf/RDFSerializer.cpp \
src/rdf/RDFSerializerNTriples.cpp \
src/rdf/RDFSerializerRaptor.cpp \
src/sequence/AdjacencyList.cpp \
src/sequence/ArraySequence.cpp \
src/sequence/HuffmanSequence.cpp \
src/sequence/IntSequence.cpp \
src/sequence/LogSequence2.cpp \
src/sequence/LogSequence.cpp \
src/sequence/WaveletSequence.cpp \
src/sparql/IndexJoinBinding.cpp \
src/sparql/JoinAlgorithms.cpp \
src/sparql/MergeJoinBinding.cpp \
src/sparql/QueryProcessor.cpp \
src/sparql/SortBinding.cpp \
src/sparql/TriplePatternBinding.cpp \
src/triples/BitmapTriples.cpp \
src/triples/BitmapTriplesIterators.cpp \
src/triples/CompactTriples.cpp \
src/triples/PlainTriples.cpp \
src/triples/predicateindex.cpp \
src/triples/TripleIterators.cpp \
src/triples/TripleListDisk.cpp \
src/triples/TripleOrderConvert.cpp \
src/triples/TriplesComparator.cpp \
src/triples/TriplesKyoto.cpp \
src/triples/TriplesList.cpp \
src/util/bitutil.cpp \
src/util/crc16.cpp \
src/util/crc32.cpp \
src/util/crc8.cpp \
src/util/filemap.cpp \
src/util/fileUtil.cpp \
src/util/propertyutil.cpp \
src/util/StopWatch.cpp \
third/gzstream.cpp 

AM_CPPFLAGS = $(WARN_CFLAGS) \
-I$(top_builddir)/lib -I$(builddir)/include \
$(RAPTOR2_HDR) $(RAPTOR2_CFLAGS) \
$(KYOTO_HDR) $(KYOTO_CFLAGS) \
$(SERD_HDR) $(SERD_CFLAGS) \
$(ZLIB_CFLAGS)

AM_LDFLAGS = $(RAPTOR2_LIB) $(SERD_LIB) $(ZLIB_LIB) $(KYOTO_LIB)

if WANTS_LIBCDS
AM_CPPFLAGS += -DHAVE_CDS -I$(top_builddir)/libcds/include
endif
