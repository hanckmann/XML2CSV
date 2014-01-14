#-------------------------------------------------
#
# Project created by QtCreator 2014-01-08T17:26:40
#
#-------------------------------------------------

include(../QXML2CSV.pri)

QT       += core xml
QT       -= gui

TARGET = x2c
CONFIG   += console
CONFIG   -= app_bundle

MOC_DIR = tmp
OBJECTS_DIR = obj
DESTDIR = ../bin

TEMPLATE = app


SOURCES += main.cpp
