#-------------------------------------------------
#
# Project created by QtCreator 2011-06-07T18:38:15
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = hdt-it
TEMPLATE = app


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
    hdtmanager.cpp

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
    hdtmanager.hpp

FORMS    += hdtit.ui \
    hdtspecform.ui

INCLUDEPATH += ../hdt-lib/include/

LIBS += ../libcds-v1.0.7/lib/libcds.a ../hdt-lib/libhdt.a

RESOURCES += \
    hdtresources.qrc
