
!include(../tools.pri)

QT       += core

QT       -= gui

TARGET = hdtsearch
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += ../../../tools/hdtSearch.cpp

target.path = $${INSTALL_ROOT}/bin

INSTALLS += target
