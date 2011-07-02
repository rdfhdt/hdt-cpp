#-------------------------------------------------
#
# Project created by QtCreator 2011-06-07T18:38:15
#
#-------------------------------------------------

QT       += core gui opengl

CONFIG += debug_and_release

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
    abouthdt.cpp

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
    abouthdt.hpp

FORMS    += hdtit.ui \
    hdtspecform.ui \
    abouthdt.ui

INCLUDEPATH += ../hdt-lib/include/ .

#RAPTOR
win32:LIBS += E:/win/libxml2-2.7.8.win32/lib/libxml2.lib E:/win/zlib-1.2.5/lib/zlib.lib
win32:LIBS += -L../raptor/qmake/raptor/debug/ -lraptor

# Using Traditional Makefile
#LIBS += ../libcds-v1.0.7/lib/libcds.a ../hdt-lib/libhdt.a

# Using Qt Projects

win32:LIBS += ../hdt-lib/qmake/win32/libhdt.a ../libcds-v1.0.7/qmake/win32/libcds.a

unix:!macx:LIBS += ../hdt-lib/qmake/unix/libhdt.a ../libcds-v1.0.7/qmake/unix/libcds.a
macx:LIBS += ../hdt-lib/qmake/macx/libhdt.a ../libcds-v1.0.7/qmake/macx/libcds.a

#PRE_TARGETDEPS += $$LIBS

macx:LIBS += -lz -lraptor2


RESOURCES += \
    hdtresources.qrc

win32:RC_FILE = hdtico.rc

ICON = hdtico.icns
