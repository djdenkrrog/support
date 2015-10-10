#-------------------------------------------------
#
# Project created by QtCreator 2015-09-05T12:42:24
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
QT       += xml
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = supportClient
TEMPLATE = app


SOURCES += main.cpp\
    frmMain.cpp \
    frmAbout.cpp \
    frmLogin.cpp

HEADERS  += \
    frmMain.h \
    frmAbout.h \
    frmLogin.h

FORMS    += \
    frmMain.ui \
    frmAbout.ui \
    frmLogin.ui
RESOURCES += \
    supportClient.qrc

RC_FILE = supportClient.rc

CONFIG += c++11
