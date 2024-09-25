bin_PROGRAMS = hdt2rdf hdtInfo hdtSearch modifyHeader rdf2hdt replaceHeader searchHeader

AM_DEFAULT_SOURCE_EXT = .cpp

AM_CPPFLAGS = -I@top_srcdir@/libhdt/include $(WARN_CFLAGS) $(EXTRAFLAGS)
AM_LDFLAGS = $(SERD_LIBS) $(ZLIB_LIBS) $(KYOTO_LIBS)
LDADD = ../libhdt.la
