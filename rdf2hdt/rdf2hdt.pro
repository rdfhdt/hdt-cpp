TEMPLATE = app
CONFIG += console c++11

SOURCES += \
    rdf2hdt.cpp

LIBCDS = ../libcds-v1.0.12

# Using Hard-coded Makefile
INCLUDEPATH += $${LIBCDS}/includes ../hdt-lib/include/ .
#LIBS += $${LIBCDS}/lib/libcds.a ../hdt-lib/libhdt.a

    win32:LIBS += ../hdt-lib/qmake/win32/hdt.lib $${LIBCDS}/qmake/win32/cds.lib "F:\git\zlib\bin\zlib.lib" "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10586.0\um\x64\opengl32.lib" "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10586.0\um\x64\glu32.lib" "F:\git\serd\bin\serd.lib"

PRE_TARGETDEPS += $$LIBS
