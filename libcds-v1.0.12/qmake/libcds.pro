#-------------------------------------------------
#
# Project created by QtCreator 2011-06-16T15:23:01
#
#-------------------------------------------------

QT       -= core gui

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

macx:QMAKE_CXXFLAGS_RELEASE += -msse4.2

CONFIG += debug_and_release
macx:CONFIG += x86_64

TARGET = cds 
TEMPLATE = lib
CONFIG += staticlib

win32:OutputDir = 'win32'
unix:OutputDir = 'unix'
macx:OutputDir = 'macx'

DESTDIR = $${OutputDir}
OBJECTS_DIR = $${OutputDir}
MOC_DIR = $${OutputDir}
RCC_DIR = $${OutputDir}
UI_DIR = $${OutputDir}

SOURCES += \ 
    ../src/utils/cppUtils.cpp \
    ../src/utils/BitString.cpp \
    ../src/utils/Array.cpp \
    ../src/static/bitsequence/TableOffsetRRR.cpp \
    ../src/static/bitsequence/sdarraySadakane.cpp \
    ../src/static/bitsequence/BitSequenceSDArray.cpp \
    ../src/static/bitsequence/BitSequenceRRR.cpp \
    ../src/static/bitsequence/BitSequenceRG.cpp \
    ../src/static/bitsequence/BitSequenceDArray.cpp \
    ../src/static/bitsequence/BitSequenceBuilderSDArray.cpp \
    ../src/static/bitsequence/BitSequenceBuilderRRR.cpp \
    ../src/static/bitsequence/BitSequenceBuilderRG.cpp \
    ../src/static/bitsequence/BitSequenceBuilderDArray.cpp \
    ../src/static/bitsequence/BitSequence.cpp \
    ../src/static/mapper/MapperNone.cpp \
    ../src/static/mapper/MapperCont.cpp \
    ../src/static/mapper/Mapper.cpp \
    ../src/static/coders/HuffmanCoder.cpp \
    ../src/static/coders/huff.cpp \
    ../src/static/permutation/PermutationMRRR.cpp \
    ../src/static/permutation/PermutationBuilderMRRR.cpp \
    ../src/static/permutation/Permutation.cpp \
    ../src/static/permutation/perm.cpp \
    ../src/static/sequence/wt_node.cpp \
    ../src/static/sequence/wt_node_leaf.cpp \
    ../src/static/sequence/wt_node_internal.cpp \
    ../src/static/sequence/wt_coder.cpp \
    ../src/static/sequence/wt_coder_huff.cpp \
    ../src/static/sequence/wt_coder_binary.cpp \
    ../src/static/sequence/WaveletTreeNoptrs.cpp \
    ../src/static/sequence/WaveletTree.cpp \
    ../src/static/sequence/SequenceGMRChunk.cpp \
    ../src/static/sequence/SequenceGMR.cpp \
    ../src/static/sequence/SequenceBuilderWaveletTreeNoptrs.cpp \
    ../src/static/sequence/SequenceBuilderWaveletTree.cpp \
    ../src/static/sequence/SequenceBuilderStr.cpp \
    ../src/static/sequence/SequenceBuilderGMRChunk.cpp \
    ../src/static/sequence/SequenceBuilderGMR.cpp \
    ../src/static/sequence/SequenceBuilderAlphPart.cpp \
    ../src/static/sequence/SequenceAlphPart.cpp \
    ../src/static/sequence/Sequence.cpp \
    ../src/static/sequence/BitmapsSequence.cpp \
    ../src/static/bitsequence/BitSequence375.cpp \
    ../src/utils/timing.cpp \
    ../src/static/textindex/TextIndexCSA.cpp \
    ../src/static/textindex/TextIndex.cpp \
    ../src/static/textindex/suftest3.test.cpp \
    ../src/static/textindex/qsufsort.cpp \
    ../src/static/textindex/mmap.cpp \
    ../src/static/textindex/comparray4.cpp \
    ../src/static/suffixtree/SuffixTreeY.cpp \
    ../src/static/suffixtree/SuffixTree.cpp \
    ../src/static/suffixtree/RMQ_succinct.cpp \
    ../src/static/suffixtree/RMQ_succinct_lcp.cpp \
    ../src/static/suffixtree/PSV.cpp \
    ../src/static/suffixtree/NSV.cpp \
    ../src/static/suffixtree/NPR.cpp \
    ../src/static/suffixtree/NPR_FMN.cpp \
    ../src/static/suffixtree/NPR_CN.cpp \
    ../src/static/suffixtree/LCP.cpp \
    ../src/static/suffixtree/LCP_Sad.cpp \
    ../src/static/suffixtree/LCP_PT.cpp \
    ../src/static/suffixtree/LCP_PhiSpare.cpp \
    ../src/static/suffixtree/LCP_naive.cpp \
    ../src/static/suffixtree/LCP_FMN.cpp \
    ../src/static/suffixtree/LCP_DAC.cpp \
    ../src/static/suffixtree/LCP_DAC_VAR.cpp \
    ../src/static/suffixtree/factorization.cpp \
    ../src/static/suffixtree/factorization_var.cpp \
    ../src/static/sequence/WaveletTreeNoptrsS.cpp \
    ../src/static/sequence/WaveletMatrix.cpp \
    ../src/static/sequence/SequenceBuilderWaveletTreeNoptrsS.cpp \
    ../src/static/sequence/SequenceBuilderWaveletMatrix.cpp \
    ../src/static/permutation/PermutationWT.cpp \
    ../src/static/permutation/PermutationBuilderWT.cpp \
    ../src/static/mapper/MapperRev.cpp

HEADERS += \ 
    ../src/utils/libcdsTrees.h \
    ../src/utils/libcdsSDArray.h \
    ../src/utils/libcdsBasics.h \
    ../src/utils/cppUtils.h \
    ../src/utils/BitString.h \
    ../src/utils/Array.h \
    ../src/static/bitsequence/TableOffsetRRR.h \
    ../src/static/bitsequence/sdarraySadakane.h \
    ../src/static/bitsequence/BitSequenceSDArray.h \
    ../src/static/bitsequence/BitSequenceRRR.h \
    ../src/static/bitsequence/BitSequenceRG.h \
    ../src/static/bitsequence/BitSequenceDArray.h \
    ../src/static/bitsequence/BitSequenceBuilderSDArray.h \
    ../src/static/bitsequence/BitSequenceBuilderRRR.h \
    ../src/static/bitsequence/BitSequenceBuilderRG.h \
    ../src/static/bitsequence/BitSequenceBuilderDArray.h \
    ../src/static/bitsequence/BitSequenceBuilder.h \
    ../src/static/bitsequence/BitSequence.h \
    ../src/static/mapper/MapperNone.h \
    ../src/static/mapper/MapperCont.h \
    ../src/static/mapper/Mapper.h \
    ../src/static/coders/HuffmanCoder.h \
    ../src/static/coders/huff.h \
    ../src/static/coders/Coder.h \
    ../src/static/permutation/PermutationMRRR.h \
    ../src/static/permutation/PermutationBuilderMRRR.h \
    ../src/static/permutation/PermutationBuilder.h \
    ../src/static/permutation/Permutation.h \
    ../src/static/permutation/perm.h \
    ../src/static/sequence/wt_node.h \
    ../src/static/sequence/wt_node_leaf.h \
    ../src/static/sequence/wt_node_internal.h \
    ../src/static/sequence/wt_coder.h \
    ../src/static/sequence/wt_coder_huff.h \
    ../src/static/sequence/wt_coder_binary.h \
    ../src/static/sequence/WaveletTreeNoptrs.h \
    ../src/static/sequence/WaveletTree.h \
    ../src/static/sequence/SequenceGMRChunk.h \
    ../src/static/sequence/SequenceGMR.h \
    ../src/static/sequence/SequenceBuilderWaveletTreeNoptrs.h \
    ../src/static/sequence/SequenceBuilderWaveletTree.h \
    ../src/static/sequence/SequenceBuilderStr.h \
    ../src/static/sequence/SequenceBuilderGMRChunk.h \
    ../src/static/sequence/SequenceBuilderGMR.h \
    ../src/static/sequence/SequenceBuilderAlphPart.h \
    ../src/static/sequence/SequenceBuilder.h \
    ../src/static/sequence/SequenceAlphPart.h \
    ../src/static/sequence/Sequence.h \
    ../src/static/sequence/BitmapsSequence.h \
    ../src/static/bitsequence/BitSequence375.h \
    ../src/utils/timing.h \
    ../src/static/textindex/TextIndexCSA.h \
    ../src/static/textindex/TextIndex.h \
    ../src/static/textindex/mmap.h \
    ../src/static/textindex/interface.h \
    ../src/static/textindex/comparray4.h \
    ../src/static/suffixtree/SuffixTreeY.h \
    ../src/static/suffixtree/SuffixTree.h \
    ../src/static/suffixtree/RMQ_succinct.h \
    ../src/static/suffixtree/RMQ_succinct_lcp.h \
    ../src/static/suffixtree/PSV.h \
    ../src/static/suffixtree/NSV.h \
    ../src/static/suffixtree/NPR.h \
    ../src/static/suffixtree/NPR_FMN.h \
    ../src/static/suffixtree/NPR_CN.h \
    ../src/static/suffixtree/LCP.h \
    ../src/static/suffixtree/LCP_Sad.h \
    ../src/static/suffixtree/LCP_PT.h \
    ../src/static/suffixtree/LCP_PhiSpare.h \
    ../src/static/suffixtree/LCP_naive.h \
    ../src/static/suffixtree/LCP_FMN.h \
    ../src/static/suffixtree/LCP_DAC.h \
    ../src/static/suffixtree/LCP_DAC_VAR.h \
    ../src/static/suffixtree/factorization.h \
    ../src/static/suffixtree/factorization_var.h \
    ../src/static/sequence/WaveletTreeNoptrsS.h \
    ../src/static/sequence/WaveletMatrix.h \
    ../src/static/sequence/SequenceBuilderWaveletTreeNoptrsS.h \
    ../src/static/sequence/SequenceBuilderWaveletMatrix.h \
    ../src/static/permutation/PermutationWT.h \
    ../src/static/permutation/PermutationBuilderWT.h \
    ../src/static/mapper/MapperRev.h

SRCBASE = ../../libcds-v1.0.12/src

INCLUDEPATH += $${SRCBASE}/utils \
    $${SRCBASE}/static/bitsequence \
    $${SRCBASE}/static/sequence \
    $${SRCBASE}/static/coders \
    $${SRCBASE}/static/mapper \
    $${SRCBASE}/static/permutation \
    $${SRCBASE}/static/textindex \
    $${SRCBASE}/static/suffixtree



