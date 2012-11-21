
!include(../tools.pri)

QT       += core

QT       -= gui

TARGET = rdf2hdt
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += ../../../tools/rdf2hdt.cpp
