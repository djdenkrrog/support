#-------------------------------------------------
#
# Project created by QtCreator 2015-11-18T12:52:10
#
#-------------------------------------------------

DESTDIR     += build/bin
MOC_DIR     += build/mocs
UI_DIR      += build/uis
RCC_DIR     += build/rccs
OBJECTS_DIR += build/objs
CONFIG      -= debug_and_release debug_and_release_target

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = controlWorkingTime
TEMPLATE = app


SOURCES += main.cpp\
        frmMain.cpp \
    frmCalcTime.cpp \
    frmAbout.cpp

HEADERS  += frmMain.h \
    frmCalcTime.h \
    frmAbout.h

FORMS    += frmMain.ui \
    frmCalcTime.ui \
    frmAbout.ui

RESOURCES += controlWorkingTime.qrc

