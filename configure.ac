AC_INIT([libhdt], [1.1.2], [some@email.com], [hdt], [https://github.com/rdfhdt/hdt-cpp])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_HEADERS([build/config.h])
AM_INIT_AUTOMAKE([foreign])
AM_SILENT_RULES([yes])

CXXFLAGS="$CXXFLAGS -g -O2 -std=c++0x"
AC_PROG_CXX
gl_EARLY
AC_LANG([C++])
AC_PROG_INSTALL
AC_PROG_CPP
AM_PROG_AR
LT_INIT([shared static])

# Checks for libraries
AC_ARG_ENABLE([libcds],
  AS_HELP_STRING([--enable-libcds],[Turn on libcds [default: yes]]),
  [case "${enableval}" in
    yes) wants_cds=true ;;
    no)  wants_cds=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-libcds]) ;;
   esac],[wants_cds=true])
AM_CONDITIONAL([WANTS_LIBCDS], [test x$wants_cds = xtrue])
AM_COND_IF([WANTS_LIBCDS],
  AC_MSG_NOTICE([LIBCDS enabled]))

AC_ARG_ENABLE([zlib],
  AS_HELP_STRING([--enable-zlib], [Turn on z library [default: yes] ]),
  [case "${enableval}" in
    yes) wants_z=true ;;
    no)  wants_z=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-zlib]) ;;
   esac],[wants_z=true])
AM_CONDITIONAL([WANTS_Z], [test x$wants_z = xtrue])
AM_COND_IF([WANTS_Z],
  [PKG_CHECK_MODULES([ZLIB],[zlib],
    [AC_DEFINE([HAVE_LIBZ],[1],[zlib available])
  ])])

AC_ARG_WITH([kyoto],
  AS_HELP_STRING([--with-kyoto], [Enables kyoto library]),
  [PKG_CHECK_MODULES([KYOTO], [kyotocabinet],
     [AC_DEFINE([HAVE_KYOTO],[1],[kyoto available])
  ])])

AC_ARG_WITH([serd],
  AS_HELP_STRING([--with-serd], [Enables Serd library]),
  [PKG_CHECK_MODULES([SERD], [serd-0],
    [AC_DEFINE([HAVE_SERD],[1],[Serd available])
  ])])

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
