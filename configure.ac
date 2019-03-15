AC_INIT([libhdt], [1.3.3], [some@email.com], [hdt], [https://github.com/rdfhdt/hdt-cpp])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_HEADERS([build/config.h])
AM_INIT_AUTOMAKE([foreign])
AM_SILENT_RULES([yes])

# CXXFLAGS is cleaned with user-defined flags.
# configure includes by default some flags.
AC_SUBST(CXXFLAGS,$CXXFLAGS)
AC_PROG_CXX
gl_EARLY
AC_LANG([C++])
AC_PROG_INSTALL
AC_PROG_CPP
AM_PROG_AR
LT_INIT([shared static])
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])

# Friendly optimisations
# When --enable-optiization is set, CXXFLAGS includes -g -O2 and the flags
# defined by the user.
# If --disable-optimization is set, then CXXFLAGS is taken directly from the
# user.

# Building options
AC_MSG_CHECKING(whether to enable optimizations)
AC_ARG_ENABLE([optimization],
  AS_HELP_STRING([--enable-optimization],[Build library with optimization parameters [default=yes]]),
  [AC_MSG_RESULT(${enableval})],
  [CXXFLAGS="${CXXFLAGS} -g -O2 -std=c++11"]
  [AC_MSG_RESULT(yes)])

AC_MSG_CHECKING(whether to build libcds)
AC_ARG_ENABLE([libcds],
  AS_HELP_STRING([--enable-libcds],[Build libcds [default=yes]]),
  [AC_MSG_RESULT(${enableval})],
  [enable_cds=yes]
  [AC_MSG_RESULT(yes)])
AM_CONDITIONAL([WANTS_LIBCDS], [test x$enable_cds != xno])

# Dependencies
AC_ARG_WITH([zlib],
  AS_HELP_STRING([--with-zlib], [Use z library [default=yes] ]),
   [],[with_zlib=true])
AS_IF([test x$with_zlib != xno],
  [PKG_CHECK_MODULES([ZLIB],[zlib],
    [AC_DEFINE([HAVE_LIBZ],[1],[zlib available])]
    [EXTRAFLAGS="${EXTRAFLAGS} -DHAVE_LIBZ"]
    [AC_SUBST(EXTRAFLAGS)])
  ])

AC_ARG_WITH([serd],
  AS_HELP_STRING([--with-serd], [Use serd library [default=yes] ]),
  [],[with_serd=true])
AS_IF([test x$with_serd != xno],
  [PKG_CHECK_MODULES([SERD], [serd-0 >= 0.28.0],
    [AC_DEFINE([HAVE_SERD],[1],[Serd available])]
    [EXTRAFLAGS="${EXTRAFLAGS} -DHAVE_SERD"]
    [AC_SUBST(EXTRAFLAGS)])
  ])

AC_ARG_WITH([kyoto],
  AS_HELP_STRING([--with-kyoto], [Use kyoto library [default=no]]),
  [], [with_kyoto=no])
AS_IF([test x$with_kyoto != xno],
  [PKG_CHECK_MODULES([KYOTO], [kyotocabinet],
     [AC_DEFINE([HAVE_KYOTO],[1],[kyoto available])]
     [EXTRAFLAGS="${EXTRAFLAGS} -DHAVE_KYOTO"]
     [AC_SUBST(EXTRAFLAGS)])
  ])

# Check for typedefs, structures, and compiler characteristics
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([select])

gl_INIT
gl_WARN_ADD([-Wall], [WARN_CFLAGS])
gl_WARN_ADD([-Wextra], [WARN_CFLAGS])
gl_WARN_ADD([-Wno-sign-compare], [WARN_CFLAGS])
gl_WARN_ADD([-Wno-unused-but-set-variable], [WARN_CFLAGS])
gl_WARN_ADD([-Wno-unused-parameter], [WARN_CFLAGS])
gl_WARN_ADD([-Wno-unknown-warning-option], [WARN_CFLAGS])

AC_CONFIG_FILES([
  Makefile
  libhdt/hdt.pc
  libhdt/Makefile
  libhdt/tools/Makefile
  libhdt/tests/Makefile
  libcds/cds.pc
  libcds/Makefile
  libcds/tests/Makefile
])
AC_OUTPUT
