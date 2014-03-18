#-------------------------------------------------
#
# Project created by QtCreator 2014-03-18T16:03:05
#
#-------------------------------------------------

include(../QCSV2MATRIX.pri)

QT       += core
QT       -= gui

TARGET = csv2matrix
CONFIG   += console
CONFIG   -= app_bundle

MOC_DIR = tmp
OBJECTS_DIR = obj
DESTDIR = ../bin

TEMPLATE = app


SOURCES += main.cpp
