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
    Camera.cpp

HEADERS  += hdtit.hpp \
    matrixviewwidget.hpp \
    hdtspecform.hpp \
    fps.hpp \
    Color.h \
    Camera.h \
    constants.h \
    colors.hpp

FORMS    += hdtit.ui \
    hdtspecform.ui

INCLUDEPATH += ../hdt-lib/include/

LIBS += ../libcds-v1.0.7/lib/libcds.a ../hdt-lib/libhdt.a
