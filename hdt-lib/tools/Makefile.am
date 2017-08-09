bin_PROGRAMS = hdt2rdf hdtInfo hdtSearch modifyHeader rdf2hdt replaceHeader searchHeader

AM_DEFAULT_SOURCE_EXT = .cpp

AM_CPPFLAGS = -I@top_srcdir@/hdt-lib/include $(WARN_CFLAGS)
AM_LDFLAGS = $(RAPTOR2_LIB) $(SERD_LIB) $(ZLIB_LIB) $(KYOTO_LIB)
LDADD = ../libhdt.la

if WANTS_LIBCDS
AM_CPPFLAGS += -DHAVE_CDS -I$(top_builddir)/libcds/include
LDADD += $(top_builddir)/libcds/libcds.la
endif

