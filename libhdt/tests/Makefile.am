check_PROGRAMS = \
bit375 \
bitutiltest \
c11 \
dictionaries \
listener \
logarr \
properties \
serd \
streamtest \
testmax \
dumpDictionary
#cmp \
#confm \
#conops \
#conpfc \
#convert \
#conwav \
#csd \
#dic \
#filterSearch \
#genCache \
#genIndex \
#getobj \
#hdt2rdfNotMapping \
#hdtExtract \
#iter \
#joinsearch \
#jointest \
#kyoto \
#mergeHDT \
#mincor \
#naiveComplete \
#opendic \
#parse \
#patsearch \
#popcnt \
#randomSolution \
#wav

AM_DEFAULT_SOURCE_EXT = .cpp

AM_CPPFLAGS = -I@top_srcdir@/libhdt/include $(WARN_CFLAGS) $(EXTRAFLAGS)
AM_LDFLAGS = $(SERD_LIBS) $(ZLIB_LIBS) $(KYOTO_LIBS)
LDADD = ../libhdt.la

TESTS = $(check_PROGRAMS)
