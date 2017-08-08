check_PROGRAMS = \
bit375 \
bitutiltest \
cmp \
confm \
conops \
conpfc \
convert \
conwav \
csd \
dic \
filterSearch \
genCache \
genIndex \
getobj \
hdt2rdfNotMapping \
hdtExtract \
iter \
joinsearch \
jointest \
kyoto \
listener \
logarr \
mergeHDT \
mincor \
naiveComplete \
opendic \
parse \
patsearch \
popcnt \
rdfprint \
streamtest \
testmax \
wav

AM_DEFAULT_SOURCE_EXT = .cpp

AM_CPPFLAGS = -I@top_srcdir@/hdt-lib/include $(WARN_CFLAGS)
AM_LDFLAGS = $(LIBCDS_LIB) $(RAPTOR2_LIB) $(SERD_LIB) $(ZLIB_LIB)
LDADD = ../libhdt.la

TESTS = $(check_PROGRAMS)

