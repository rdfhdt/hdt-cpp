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

bit375_SOURCES = bit375.cpp
bitutiltest_SOURCES = bitutiltest.cpp
cmp_SOURCES = cmp.cpp
confm_SOURCES = confm.cpp
conops_SOURCES = conops.cpp
conpfc_SOURCES = conpfc.cpp
convert_SOURCES = convert.cpp
conwav_SOURCES = conwav.cpp
csd_SOURCES = csd.cpp
dic_SOURCES = dic.cpp
filterSearch_SOURCES = filterSearch.cpp
genCache_SOURCES = genCache.cpp
genIndex_SOURCES = genIndex.cpp
getobj_SOURCES = getobj.cpp
hdt2rdfNotMapping_SOURCES = hdt2rdfNotMapping.cpp
hdtExtract_SOURCES = hdtExtract.cpp
iter_SOURCES = iter.cpp
joinsearch_SOURCES = joinsearch.cpp
jointest_SOURCES = jointest.cpp
kyoto_SOURCES = kyoto.cpp
listener_SOURCES = listener.cpp
logarr_SOURCES = logarr.cpp
mergeHDT_SOURCES = mergeHDT.cpp
mincor_SOURCES = mincor.cpp
naiveComplete_SOURCES = naiveComplete.cpp
opendic_SOURCES = opendic.cpp
parse_SOURCES = parse.cpp
patsearch_SOURCES = patsearch.cpp
popcnt_SOURCES = popcnt.cpp
rdfprint_SOURCES = rdfprint.cpp
streamtest_SOURCES = streamtest.cpp
testmax_SOURCES = testmax.cpp
wav_SOURCES = wav.cpp

AM_CPPFLAGS = -I@top_srcdir@/hdt-lib/include
AM_LDFLAGS = $(LIBCDS_LIB) $(RAPTOR2_LIB) $(SERD_LIB) $(ZLIB_LIB)
LDADD = ../libhdt.la

TESTS = $(check_PROGRAMS)

