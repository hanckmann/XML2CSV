#-------------------------------------------------
#
# Project created by QtCreator 2014-01-08T17:26:40
#
#-------------------------------------------------

QT       += core xml
QT       -= gui

TARGET = qxml2csv
TEMPLATE = lib
CONFIG += staticlib

MOC_DIR = tmp
OBJECTS_DIR = obj
DESTDIR = ../bin

DEFINES += QXML2CSV_LIBRARY


SOURCES += \
    qxml2csv.cpp

HEADERS += \
    qxml2csv.h \
    QTuple.h \
    QIndexMap.h
