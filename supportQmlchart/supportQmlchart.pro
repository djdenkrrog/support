TEMPLATE = app

DESTDIR     += build/bin
MOC_DIR     += build/mocs
UI_DIR      += build/uis
RCC_DIR     += build/rccs
OBJECTS_DIR += build/objs
CONFIG      -= debug_and_release debug_and_release_target


QT += qml quick
CONFIG += c++11

SOURCES += main.cpp

RESOURCES += supportQml.qrc

RC_FILE = supportQmlchart.rc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

DISTFILES += \
    content/ChartStatic.qml \
    content/CountCallsDelegate.qml \
    content/Logo.qml

