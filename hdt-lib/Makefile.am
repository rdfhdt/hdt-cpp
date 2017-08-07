AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libhdt.la
libhdt_la_SOURCES = \
src/hdt/HDTSpecification.cpp \
src/hdt/HDTFactory.cpp \
src/hdt/HDTManager.cpp \
src/hdt/ControlInformation.cpp \
src/hdt/BasicHDT.cpp \
src/hdt/BasicModifiableHDT.hpp \
src/util/StopWatch.cpp \
src/util/propertyutil.cpp \
src/triples/TriplesList.cpp \
src/triples/TriplesComparator.cpp \
src/triples/TripleOrderConvert.cpp \
src/triples/TripleIterators.cpp \
src/triples/PlainTriples.cpp \
src/triples/CompactTriples.cpp \
src/triples/BitmapTriples.cpp \
src/triples/BitmapTriplesIterators.cpp \
src/triples/TripleListDisk.cpp \
src/triples/TriplesKyoto.cpp \
src/huffman/Huffman.cpp \
src/huffman/huff.cpp \
src/header/PlainHeader.cpp \
src/libdcs/VByte.cpp \
src/libdcs/CSD.cpp \
src/libdcs/CSD_PFC.cpp \
src/libdcs/CSD_HTFC.cpp \
src/libdcs/CSD_FMIndex.cpp \
src/libdcs/CSD_Cache2.cpp \
src/libdcs/CSD_Cache.cpp \
src/libdcs/fmindex/SuffixArray.cpp \
src/libdcs/fmindex/SSA.cpp \
src/dictionary/PlainDictionary.cpp \
src/dictionary/FourSectionDictionary.cpp \
src/dictionary/KyotoDictionary.cpp \
src/dictionary/LiteralDictionary.cpp \
src/rdf/RDFParserNtriples.cpp \
src/rdf/RDFParser.cpp \
src/rdf/RDFSerializerNTriples.cpp \
src/rdf/RDFSerializerRaptor.cpp \
src/rdf/RDFSerializer.cpp \
src/util/fileUtil.cpp \
src/rdf/RDFParserRaptorCallback.cpp \
src/rdf/RDFParserSerd.cpp \
src/sparql/TriplePatternBinding.cpp \
src/sparql/MergeJoinBinding.cpp \
src/sparql/JoinAlgorithms.cpp \
src/sparql/IndexJoinBinding.cpp \
src/sparql/SortBinding.cpp \
src/sequence/WaveletSequence.cpp \
src/sequence/LogSequence2.cpp \
src/sequence/LogSequence.cpp \
src/sequence/IntSequence.cpp \
src/sequence/HuffmanSequence.cpp \
src/sequence/ArraySequence.cpp \
src/sequence/AdjacencyList.cpp \
src/hdt/TripleIDStringIterator.cpp \
src/hdt/BasicModifiableHDT.cpp \
src/sparql/QueryProcessor.cpp \
src/bitsequence/BitSequence375.cpp \
src/util/crc32.cpp \
src/util/crc16.cpp \
src/util/crc8.cpp \
src/util/bitutil.cpp \
src/util/filemap.cpp \
third/gzstream.cpp \
src/rdf/RDFParserNtriplesCallback.cpp \
src/triples/predicateindex.cpp
 
libhdt_la_CPPFLAGS = -I$(builddir)/include $(LIBCDS_HDR) $(LIBCDS_CFLAGS) $(RAPTOR2_HDR) $(RAPTOR2_CFLAGS) $(SERD_HDR) $(SERD_CFLAGS) $(ZLIB_CFLAGS)
libhdt_la_LIBADD = $(LIBCDS_LIB) $(RAPTOR2_LIB) $(SERD_LIB) $(ZLIB_LIB)

bin_PROGRAMS = hdt2rdf hdtInfo hdtSearch rdf2hdt replaceHeader
hdt2rdf_SOURCES = tools/hdt2rdf.cpp
hdt2rdf_CPPFLAGS = -I$(builddir)/include
hdt2rdf_LDADD = libhdt.la $(LIBCDS_LIB) $(RAPTOR2_LIB) $(SERD_LIB)

hdtInfo_SOURCES = tools/hdtInfo.cpp
hdtInfo_CPPFLAGS = -I$(builddir)/include
hdtInfo_LDADD = libhdt.la $(LIBCDS_LIB) $(RAPTOR2_LIB) $(SERD_LIB)

hdtSearch_SOURCES = tools/hdtSearch.cpp
hdtSearch_CPPFLAGS = -I$(builddir)/include
hdtSearch_LDADD = libhdt.la $(LIBCDS_LIB) $(RAPTOR2_LIB) $(SERD_LIB)

rdf2hdt_SOURCES = tools/rdf2hdt.cpp
rdf2hdt_CPPFLAGS = -I$(builddir)/include
rdf2hdt_LDADD = libhdt.la $(LIBCDS_LIB) $(RAPTOR2_LIB) $(SERD_LIB)

replaceHeader_SOURCES = tools/replaceHeader.cpp
replaceHeader_CPPFLAGS = -I$(builddir)/include
replaceHeader_LDADD = libhdt.la $(LIBCDS_LIB) $(RAPTOR2_LIB) $(SERD_LIB)

pkginclude_HEADERS = include/*
