#-------------------------------------------------
#
# Project created by QtCreator 2011-06-16T15:23:01
#
#-------------------------------------------------

QT       -= core gui

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

#macx:QMAKE_CXXFLAGS += -msse4.2

win32-g++:contains(QMAKE_HOST.arch, x86_64):{
        CONFIG += exceptions rtti
}

CONFIG += debug_and_release
macx:CONFIG += x86_64

TARGET = hdt
TEMPLATE = lib
CONFIG += staticlib

DEFINES += USE_LIBZ USE_RAPTOR RAPTOR_STATIC 
#USE_SERD

win32:OutputDir = 'win32'
unix:OutputDir = 'unix'
macx:OutputDir = 'macx'

DESTDIR = $${OutputDir}
OBJECTS_DIR = $${OutputDir}
MOC_DIR = $${OutputDir}
RCC_DIR = $${OutputDir}
UI_DIR = $${OutputDir}

SOURCES += \
    ../src/hdt/HDTSpecification.cpp \
    ../src/hdt/HDTFactory.cpp \
    ../src/hdt/HDTManager.cpp \
    ../src/hdt/ControlInformation.cpp \
    ../src/hdt/BasicHDT.cpp \
    ../src/hdt/BasicModifiableHDT.hpp \
    ../src/util/StopWatch.cpp \
    ../src/util/propertyutil.cpp \
    ../src/triples/TriplesList.cpp \
    ../src/triples/TriplesComparator.cpp \
    ../src/triples/TripleOrderConvert.cpp \
    ../src/triples/TripleIterators.cpp \
    ../src/triples/PlainTriples.cpp \
    ../src/triples/CompactTriples.cpp \
    ../src/triples/BitmapTriples.cpp \
    ../src/triples/BitmapTriplesIterators.cpp \
    ../src/triples/TripleListDisk.cpp \
    ../src/triples/TriplesKyoto.cpp \
    ../src/huffman/Huffman.cpp \
    ../src/huffman/huff.cpp \
    ../src/header/PlainHeader.cpp \
    ../src/libdcs/VByte.cpp \
    ../src/libdcs/CSD.cpp \
    ../src/libdcs/CSD_PFC.cpp \
    ../src/libdcs/CSD_HTFC.cpp \
    ../src/libdcs/CSD_FMIndex.cpp \
    ../src/libdcs/CSD_Cache2.cpp \
    ../src/libdcs/CSD_Cache.cpp \
    ../src/libdcs/fmindex/SuffixArray.cpp \
    ../src/libdcs/fmindex/SSA.cpp \
    ../src/dictionary/PlainDictionary.cpp \
    ../src/dictionary/FourSectionDictionary.cpp \
    ../src/dictionary/KyotoDictionary.cpp \
    ../src/dictionary/LiteralDictionary.cpp \
    ../src/rdf/RDFParserNtriples.cpp \
    ../src/rdf/RDFParser.cpp \
    ../src/rdf/RDFSerializerNTriples.cpp \
    ../src/rdf/RDFSerializerRaptor.cpp \
    ../src/rdf/RDFSerializer.cpp \
    ../src/util/fileUtil.cpp \
    ../src/rdf/RDFParserRaptorCallback.cpp \
    ../src/rdf/RDFParserSerd.cpp \
    ../src/sparql/TriplePatternBinding.cpp \
    ../src/sparql/MergeJoinBinding.cpp \
    ../src/sparql/JoinAlgorithms.cpp \
    ../src/sparql/IndexJoinBinding.cpp \
    ../src/sparql/CachedBinding.cpp \
    ../src/sparql/BaseJoinBinding.cpp \
    ../src/sparql/SortBinding.cpp \
    ../src/sequence/WaveletSequence.cpp \
    ../src/sequence/LogSequence2.cpp \
    ../src/sequence/LogSequence.cpp \
    ../src/sequence/IntSequence.cpp \
    ../src/sequence/HuffmanSequence.cpp \
    ../src/sequence/ArraySequence.cpp \
    ../src/sequence/AdjacencyList.cpp \
    ../src/hdt/TripleIDStringIterator.cpp \
    ../src/hdt/BasicModifiableHDT.cpp \
    ../src/sparql/QueryProcessor.cpp \
    ../src/bitsequence/BitSequence375.cpp \
    ../src/util/crc32.cpp \
    ../src/util/crc16.cpp \
    ../src/util/crc8.cpp \
    ../src/util/bitutil.cpp \
    ../src/util/filemap.cpp \
    ../third/gzstream.cpp \
    ../src/rdf/RDFParserNtriplesCallback.cpp

HEADERS += \
    ../include/Triples.hpp \
    ../include/SingleTriple.hpp \
    ../include/RDF.hpp \
    ../include/Header.hpp \
    ../include/HDTVocabulary.hpp \
    ../include/HDTSpecification.hpp \
    ../include/HDTListener.hpp \
    ../include/HDTManager.hpp \
    ../include/HDTEnums.hpp \
    ../include/HDT.hpp \
    ../include/Iterator.hpp \
    ../include/Dictionary.hpp \
    ../include/ControlInformation.hpp \
    ../include/RDFParser.hpp \
    ../include/RDFSerializer.hpp \
    ../src/hdt/BasicHDT.hpp \
    ../src/hdt/BasicModifiableHDT.hpp \
    ../src/hdt/HDTFactory.hpp \
    ../src/dictionary/PlainDictionary.hpp \
    ../src/dictionary/KyotoDictionary.hpp \
    ../src/dictionary/FourSectionDictionary.hpp \
    ../src/dictionary/LiteralDictionary.hpp \
    ../src/triples/TriplesList.hpp \
    ../src/triples/TriplesComparator.hpp \
    ../src/triples/TripleOrderConvert.hpp \
    ../src/triples/TripleListDisk.hpp \
    ../src/triples/TripleIterators.hpp \
    ../src/triples/PlainTriples.hpp \
    ../src/triples/CompactTriples.hpp \
    ../src/triples/BitmapTriples.hpp \
    ../src/triples/TriplesKyoto.hpp \
    ../src/huffman/Huffman.h \
    ../src/huffman/huff.h \
    ../src/header/PlainHeader.hpp \
    ../src/libdcs/VByte.h \
    ../src/libdcs/CSD.h \
    ../src/libdcs/CSD_PFC.h \
    ../src/libdcs/CSD_HTFC.h \
    ../src/libdcs/CSD_FMIndex.h \
    ../src/libdcs/CSD_Cache2.h \
    ../src/libdcs/CSD_Cache.h \
    ../src/libdcs/fmindex/SuffixArray.h \
    ../src/libdcs/fmindex/SSA.h \
    ../src/rdf/RDFParserNtriples.hpp \
    ../src/rdf/RDFSerializerNTriples.hpp \
    ../src/rdf/RDFSerializerRaptor.hpp \
    ../src/rdf/RDFParserRaptorCallback.hpp \
    ../src/rdf/RDFParserSerd.hpp \
    ../src/sparql/VarBindingInterface.hpp \
    ../src/sparql/TriplePatternBinding.hpp \
    ../src/sparql/MergeJoinBinding.hpp \
    ../src/sparql/joins.hpp \
    ../src/sparql/JoinAlgorithms.hpp \
    ../src/sparql/IndexJoinBinding.hpp \
    ../src/sparql/CachedBinding.hpp \
    ../src/sparql/BaseJoinBinding.hpp \
    ../src/sparql/VarFilterBinding.hpp \
    ../src/sparql/SortBinding.hpp \
    ../src/sequence/WaveletSequence.hpp \
    ../src/sequence/LogSequence2.hpp \
    ../src/sequence/LogSequence.hpp \
    ../src/sequence/IntSequence.hpp \
    ../src/sequence/HuffmanSequence.hpp \
    ../src/sequence/ArraySequence.hpp \
    ../src/sequence/AdjacencyList.hpp \
    ../src/hdt/TripleIDStringIterator.hpp \
    ../src/sparql/QueryProcessor.hpp \
    ../src/util/fileUtil.hpp \
    ../src/util/StopWatch.hpp \
    ../src/util/fdstream.hpp \
    ../src/util/propertyutil.h \
    ../src/util/Histogram.h \
    ../src/bitsequence/BitSequence375.h \
    ../src/bitsequence/BitSeq.h \
    ../src/util/crc32.h \
    ../src/util/crc16.h \
    ../src/util/crc8.h \
    ../src/util/bitutil.h \
    ../src/util/filemap.h \
    ../third/fdstream.hpp \
    ../third/gzstream.h \
    ../src/rdf/RDFParserNtriplesCallback.hpp

#For hdt-lib
INCLUDEPATH += ../include

#For libcds
LIBCDSBASE = ../../libcds-v1.0.12/src

INCLUDEPATH += $${LIBCDSBASE}/utils \
    $${LIBCDSBASE}/static/bitsequence \
    $${LIBCDSBASE}/static/sequence \
    $${LIBCDSBASE}/static/coders \
    $${LIBCDSBASE}/static/mapper \
    $${LIBCDSBASE}/static/permutation \
    $${LIBCDSBASE}/static/textindex \
    $${LIBCDSBASE}/static/suffixtree


#For raptor
macx:INCLUDEPATH += /usr/local/include

#Windows
win32-g++:contains(QMAKE_HOST.arch, x86_64):{
    win32:INCLUDEPATH += C:/mingw64/include C:/msys/local/include
} else {
    win32:INCLUDEPATH += C:/mingw/include C:/MinGW/msys/1.0/include C:/MinGW/msys/1.0/local/include
}
