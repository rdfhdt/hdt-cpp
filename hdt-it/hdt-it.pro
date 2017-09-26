#-------------------------------------------------
#
# Project created by QtCreator 2011-06-07T18:38:15
#
#-------------------------------------------------

QT       += core gui opengl

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

#macx:QMAKE_CXXFLAGS += -msse4.2

win32-g++:contains(QMAKE_HOST.arch, x86_64):{
        CONFIG += exceptions rtti
}

CONFIG += debug_and_release
macx:CONFIG += x86_64

TARGET = HDT-it
TEMPLATE = app

win32:OutputDir = 'win32'
unix:OutputDir = 'unix'
macx:OutputDir = 'macx'

DESTDIR = $${OutputDir}
OBJECTS_DIR = $${OutputDir}
MOC_DIR = $${OutputDir}
RCC_DIR = $${OutputDir}
UI_DIR = $${OutputDir}

SOURCES += main.cpp\
        hdtit.cpp \
    matrixviewwidget.cpp \
    hdtspecform.cpp \
    Color.cpp \
    Camera.cpp \
    StopWatch.cpp \
    qclearlineedit.cpp \
    searchresultsmodel.cpp \
    triplecomponentmodel.cpp \
    hdtoperation.cpp \
    abouthdt.cpp \
    hdtcachedinfo.cpp \
    stringutils.cpp \
    predicatestatus.cpp \
    hdtsummarygenerator.cpp \
    resultcounter.cpp \
    headermodel.cpp \
    dictionarysuggestions.cpp \
    myapplication.cpp \
    sparqlmodel.cpp \
    sparqlform.cpp \
    regexmodel.cpp \
    hdtcontroller.cpp

HEADERS  += hdtit.hpp \
    matrixviewwidget.hpp \
    hdtspecform.hpp \
    Color.h \
    Camera.h \
    constants.h \
    colors.hpp \
    StopWatch.hpp \
    qclearlineedit.hpp \
    searchresultsmodel.hpp \
    triplecomponentmodel.hpp \
    hdtoperation.hpp \
    abouthdt.hpp \
    hdtcachedinfo.hpp \
    stringutils.hpp \
    predicatestatus.hpp \
    hdtsummarygenerator.hpp \
    resultcounter.hpp \
    headermodel.hpp \
    dictionarysuggestions.hpp \
    myapplication.hpp \
    sparqlmodel.hpp \
    sparqlform.hpp \
    regexmodel.hpp \
    hdtcontroller.hpp

FORMS    += hdtit.ui \
    hdtspecform.ui \
    abouthdt.ui \
    sparqlform.ui

TRANSLATIONS += hdt-it_es.ts

LIBCDS = ../libcds-v1.0.12

# Using Hard-coded Makefile
INCLUDEPATH += $${LIBCDS}/includes ../hdt-lib/include/ .
#LIBS += $${LIBCDS}/lib/libcds.a ../hdt-lib/libhdt.a

# Using autotools
#LIBS += $${LIBCDS}/src/.libs/libcds.a ../hdt-lib/.libs/libhdt.a

# Using Qt Projects
#win32:LIBS += ../hdt-lib/qmake/win32/libhdt.a $${LIBCDS}/qmake/win32/libcds.a

win32:LIBS += ../hdt-lib/qmake/win32/hdt.lib $${LIBCDS}/qmake/win32/cds.lib "F:\git\zlib\bin\zlib.lib" "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10586.0\um\x64\opengl32.lib" "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10586.0\um\x64\glu32.lib" "F:\serd-0.26.0\bin\serd.lib"

unix:!macx:LIBS += ../hdt-lib/qmake/unix/libhdt.a $${LIBCDS}/qmake/unix/libcds.a -lGLU
macx:LIBS += $${LIBCDS}/qmake/macx/libcds.a ../hdt-lib/qmake/macx/libhdt.a
#PRE_TARGETDEPS += $$LIBS

# Installed in the system
#LIBS += -lcds -lhdt


#External libs

#Windows
win32-g++:contains(QMAKE_HOST.arch, x86_64):{
    #win32:LIBS += -L"C:/msys/local/lib/" -lxml2 -lws2_32
} else {
   # win32:LIBS += -L"C:/MinGW/msys/1.0/local/lib/" -lxml2 -lws2_32
}

win32:LIBS += -L"C:/msys/local/lib/" -L"/usr/local/lib" -L"C:/MinGW/msys/1.0/local/lib/"

#Unix (Linux & Mac)
unix:LIBS += -L"/usr/local/lib" -lz -lserd-0

RESOURCES += \
    hdtresources.qrc

win32:RC_FILE = hdtico.rc

QMAKE_INFO_PLIST = Info.plist

ICON = hdtico.icns
