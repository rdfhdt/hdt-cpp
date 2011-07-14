#-------------------------------------------------
#
# Project created by QtCreator 2011-06-07T18:38:15
#
#-------------------------------------------------

QT       += core gui opengl

CONFIG += debug_and_release
//macx:CONFIG += x86

TARGET = hdt-it
TEMPLATE = app

win32:OutputDir = 'win32'
unix:OutputDir = 'unix'
macx:OutputDir = 'macx'

DESTDIR = $${OutputDir}
OBJECTS_DIR = $${OutputDir}
MOC_DIR = $${OutputDir}
RCC_DIR = $${OutputDir}
UI_DIR = $${OutputDir}

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O9
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE += -O9

SOURCES += main.cpp\
        hdtit.cpp \
    matrixviewwidget.cpp \
    hdtspecform.cpp \
    fps.cpp \
    Color.cpp \
    Camera.cpp \
    StopWatch.cpp \
    qclearlineedit.cpp \
    qresulttableview.cpp \
    searchresultsmodel.cpp \
    triplecomponentmodel.cpp \
    hdtmanager.cpp \
    hdtoperation.cpp \
    abouthdt.cpp \
    hdtcachedinfo.cpp \
    stringutils.cpp

HEADERS  += hdtit.hpp \
    matrixviewwidget.hpp \
    hdtspecform.hpp \
    fps.hpp \
    Color.h \
    Camera.h \
    constants.h \
    colors.hpp \
    StopWatch.hpp \
    qclearlineedit.hpp \
    qresulttableview.hpp \
    searchresultsmodel.hpp \
    triplecomponentmodel.hpp \
    hdtmanager.hpp \
    hdtoperation.hpp \
    abouthdt.hpp \
    hdtcachedinfo.hpp \
    stringutils.hpp

FORMS    += hdtit.ui \
    hdtspecform.ui \
    abouthdt.ui

INCLUDEPATH += ../hdt-lib/include/ .

# Using Traditional Makefile
#LIBS += ../libcds-v1.0.7/lib/libcds.a ../hdt-lib/libhdt.a

# Using Qt Projects
win32:LIBS += ../hdt-lib/qmake/win32/libhdt.a ../libcds-v1.0.7/qmake/win32/libcds.a
win32:LIBS += c:/mingw/lib/libraptor2.a c:/mingw/lib/libexpat.a
#win32:LIBS += ../raptor/qmake/raptor/debug/libraptor.a

unix:!macx:LIBS += ../libcds-v1.0.7/qmake/unix/libcds.a ../hdt-lib/qmake/unix/libhdt.a
macx:LIBS += ../libcds-v1.0.7/qmake/macx/libcds.a ../hdt-lib/qmake/macx/libhdt.a

PRE_TARGETDEPS += $$LIBS

macx:LIBS += -lraptor2

RESOURCES += \
    hdtresources.qrc

win32:RC_FILE = hdtico.rc

ICON = hdtico.icns
