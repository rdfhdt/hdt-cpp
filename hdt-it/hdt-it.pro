#-------------------------------------------------
#
# Project created by QtCreator 2011-06-07T18:38:15
#
#-------------------------------------------------

QT       += core gui opengl

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

macx:QMAKE_CXXFLAGS += -msse4.2

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
    hdtmanager.cpp \
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
    regexmodel.cpp

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
    hdtmanager.hpp \
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
    regexmodel.hpp

FORMS    += hdtit.ui \
    hdtspecform.ui \
    abouthdt.ui \
    sparqlform.ui

TRANSLATIONS += hdt-it_es.ts

INCLUDEPATH += ../hdt-lib/include/ .

LIBCDS = ../libcds-v1.0.12

# Using Traditional Makefile
#LIBS += $${LIBCDS}/lib/libcds.a ../hdt-lib/libhdt.a

# Using Qt Projects
win32:LIBS += ../hdt-lib/qmake/win32/libhdt.a $${LIBCDS}/qmake/win32/libcds.a
win32:LIBS += c:/mingw/lib/libraptor2.a c:/mingw/lib/libexpat.a

unix:!macx:LIBS += ../hdt-lib/qmake/unix/libhdt.a $${LIBCDS}/qmake/unix/libcds.a
macx:LIBS += $${LIBCDS}/qmake/macx/libcds.a ../hdt-lib/qmake/macx/libhdt.a

PRE_TARGETDEPS += $$LIBS

unix:LIBS += -lraptor2 -lkyotocabinet

RESOURCES += \
    hdtresources.qrc

win32:RC_FILE = hdtico.rc

QMAKE_INFO_PLIST = Info.plist

ICON = hdtico.icns
