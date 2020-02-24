#-------------------------------------------------
#
# Project created by QtCreator 2015-09-05T12:42:24
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += xml
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = support
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
    res.qrc

RC_FILE = support.rc
