#-------------------------------------------------
#
# Project created by QtCreator 2015-09-11T16:40:19
#
#-------------------------------------------------

DESTDIR     += build/bin
MOC_DIR     += build/mocs
UI_DIR      += build/uis 
RCC_DIR     += build/rccs
OBJECTS_DIR += build/objs
CONFIG      -= debug_and_release debug_and_release_target

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = supportServer
TEMPLATE = app


SOURCES += main.cpp \
    frmMain.cpp

HEADERS  += frmMain.h

FORMS    += frmMain.ui

RC_FILE = supportServer.rc
