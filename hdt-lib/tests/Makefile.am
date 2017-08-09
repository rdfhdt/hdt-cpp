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
AM_LDFLAGS = $(RAPTOR2_LIB) $(SERD_LIB) $(ZLIB_LIB) $(KYOTO_LIB)
LDADD = ../libhdt.la

if WANTS_LIBCDS
AM_CPPFLAGS += -DHAVE_CDS -I$(top_builddir)/libcds/include
LDADD += $(top_builddir)/libcds/libcds.la
endif

TESTS = $(check_PROGRAMS)

