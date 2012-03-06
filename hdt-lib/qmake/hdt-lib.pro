#-------------------------------------------------
#
# Project created by QtCreator 2011-06-16T15:23:01
#
#-------------------------------------------------

QT       -= core gui

CONFIG += debug_and_release
macx:CONFIG += x86 x86_64

TARGET = hdt
TEMPLATE = lib
CONFIG += staticlib

DEFINES += USE_RAPTOR RAPTOR_STATIC USE_FOQ
#DEFINES += USE_FOQ

win32:OutputDir = 'win32'
unix:OutputDir = 'unix'
macx:OutputDir = 'macx'

DESTDIR = $${OutputDir}
OBJECTS_DIR = $${OutputDir}
MOC_DIR = $${OutputDir}
RCC_DIR = $${OutputDir}
UI_DIR = $${OutputDir}

SOURCES += \
    ../src/rdf/RDFSerializerN3.cpp \
    ../src/HDTSpecification.cpp \
    ../src/HDTFactory.cpp \
    ../src/ControlInformation.cpp \
    ../src/BasicHDT.cpp \
    ../src/dictionary/PlainDictionary.cpp \
    ../src/util/StopWatch.cpp \
    ../src/util/propertyutil.cpp \
    ../src/triples/TriplesList.cpp \
    ../src/triples/TriplesComparator.cpp \
    ../src/triples/TripleOrderConvert.cpp \
    ../src/triples/TripleIterators.cpp \
    ../src/triples/PlainTriples.cpp \
    ../src/triples/FOQTriples.cpp \
    ../src/triples/CompactTriples.cpp \
    ../src/triples/BitmapTriples.cpp \
    ../src/triples/BitmapTriplesIterators.cpp \
    ../src/stream/WaveletStream.cpp \
    ../src/stream/UintStream.cpp \
    ../src/stream/StreamElements.cpp \
    ../src/stream/LogStream.cpp \
    ../src/stream/LogStream2.cpp \
    ../src/stream/HuffmanStream.cpp \
    ../src/stream/AdjacencyList.cpp \
    ../src/huffman/Huffman.cpp \
    ../src/huffman/huff.cpp \
    ../src/header/PlainHeader.cpp \
    ../src/header/BasicHeader.cpp \
    ../src/triples/TripleListDisk.cpp \
    ../src/libdcs/VByte.cpp \
    ../src/libdcs/CSD.cpp \
    ../src/libdcs/CSD_PFC.cpp \
    ../src/libdcs/CSD_HTFC.cpp \
    ../src/dictionary/PFCDictionary.cpp \
    ../src/rdf/RDFParserRaptor.cpp \
    ../src/rdf/RDFParserNtriples.cpp \
    ../src/rdf/RDFParser.cpp \
    ../src/rdf/RDFParserRaptorLine.cpp \
    ../src/rdf/RDFSerializerRaptor.cpp \
    ../src/rdf/RDFSerializer.cpp \
    ../src/util/fileUtil.cpp \
    ../src/rdf/RDFParserRaptorCallback.cpp \
    ../src/sparql/TriplePatternBinding.cpp \
    ../src/sparql/MergeJoinBinding.cpp \
    ../src/sparql/JoinAlgorithms.cpp \
    ../src/sparql/IndexJoinBinding.cpp \
    ../src/sparql/CachedBinding.cpp \
    ../src/sparql/BaseJoinBinding.cpp \
    ../src/sparql/SortBinding.cpp

HEADERS += \
    ../include/Triples.hpp \
    ../include/SingleTriple.hpp \
    ../include/RDF.hpp \
    ../include/Header.hpp \
    ../include/HDTVocabulary.hpp \
    ../include/HDTSpecification.hpp \
    ../include/HDTListener.hpp \
    ../include/HDTFactory.hpp \
    ../include/HDTEnums.hpp \
    ../include/HDT.hpp \
    ../include/Dictionary.hpp \
    ../include/ControlInformation.hpp \
    ../include/RDFParser.hpp \
    ../include/RDFSerializer.hpp \
    ../src/rdf/RDFSerializerN3.hpp \
    ../src/BasicHDT.hpp \
    ../src/dictionary/PlainDictionary.hpp \
    ../src/util/StopWatch.hpp \
    ../src/util/fdstream.hpp \
    ../src/util/propertyutil.h \
    ../src/util/Histogram.h \
    ../src/triples/TriplesList.hpp \
    ../src/triples/TriplesComparator.hpp \
    ../src/triples/TripleOrderConvert.hpp \
    ../src/triples/TripleListDisk.hpp \
    ../src/triples/TripleIterators.hpp \
    ../src/triples/PlainTriples.hpp \
    ../src/triples/FOQTriples.hpp \
    ../src/triples/CompactTriples.hpp \
    ../src/triples/BitmapTriples.hpp \
    ../src/stream/WaveletStream.hpp \
    ../src/stream/UintStream.hpp \
    ../src/stream/StreamElements.hpp \
    ../src/stream/LogStream.hpp \
    ../src/stream/LogStream2.hpp \
    ../src/stream/HuffmanStream.hpp \
    ../src/stream/AdjacencyList.hpp \
    ../src/huffman/Huffman.h \
    ../src/huffman/huff.h \
    ../src/header/PlainHeader.hpp \
    ../src/header/EmptyHeader.hpp \
    ../src/header/BasicHeader.hpp \
    ../src/libdcs/VByte.h \
    ../src/libdcs/CSD.h \
    ../src/libdcs/CSD_PFC.h \
    ../src/libdcs/CSD_HTFC.h \
    ../src/dictionary/PFCDictionary.hpp \
    ../src/rdf/RDFParserRaptor.hpp \
    ../src/rdf/RDFParserNtriples.hpp \
    ../src/rdf/RDFSerializerRaptor.hpp \
    ../src/rdf/RDFParserRaptorLine.hpp \
    ../src/util/fileUtil.hpp \
    ../src/rdf/RDFParserRaptorCallback.hpp \
    ../src/sparql/VarBindingInterface.hpp \
    ../src/sparql/TriplePatternBinding.hpp \
    ../src/sparql/MergeJoinBinding.hpp \
    ../src/sparql/joins.hpp \
    ../src/sparql/JoinAlgorithms.hpp \
    ../src/sparql/IndexJoinBinding.hpp \
    ../src/sparql/CachedBinding.hpp \
    ../src/sparql/BaseJoinBinding.hpp \
    ../src/sparql/VarFilterBinding.hpp \
    ../src/sparql/SortBinding.hpp

#For hdt-lib
INCLUDEPATH += ../include

#For libcds
LIBCDSBASE = ../../libcds-v1.0.7/src
INCLUDEPATH += $${LIBCDSBASE}/utils $${LIBCDSBASE}/static/bitsequence $${LIBCDSBASE}/static/sequence $${LIBCDSBASE}/static/coders $${LIBCDSBASE}/static/mapper $${LIBCDSBASE}/static/permutation

#For raptor
macx:INCLUDEPATH += /usr/local/include
win32:INCLUDEPATH += C:/mingw/include







