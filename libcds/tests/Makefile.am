check_PROGRAMS = \
testArray \
testBitSequence \
testHuffman \
testLCP \
testNPR \
testQuantile \
testSequence \
testSuffixTree \
testTextIndex \
timeSequence \
toArray2 \
toArray

AM_DEFAULT_SOURCE_EXT = .cpp
AM_CPPFLAGS = \
-I../src/static/bitsequence\
-I../src/static/coders\
-I../src/static/mapper\
-I../src/static/permutation\
-I../src/static/sequence\
-I../src/static/suffixtree\
-I../src/static/textindex\
-I../src/utils
LDADD = ../libcds.la

TESTS = $(check_PROGRAMS)
