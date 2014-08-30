#-------------------------------------------------
#
# Project created by QtCreator 2014-08-02T10:23:34
#
#-------------------------------------------------
CONFIG += c++11 shared

QT     += core gui widgets network script scripttools

TARGET = Client

#TEMPLATE = app
TEMPLATE = lib


SOURCES += main.cpp\
        mainwindow.cpp \
    connection.cpp \
    ../Common/ScriptFunctions.cpp

HEADERS  += mainwindow.h \
    connection.h \
    singleton.h \
    ../Common/ScriptFunctions.h \
    ../Common/Tags.h

FORMS    += mainwindow.ui


Debug:DESTDIR = ../../output_d
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui

Release:DESTDIR = ../../output
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui

win32: LIBS += -L$$PWD/../detours/ -ldetours

INCLUDEPATH += $$PWD/../detours
DEPENDPATH += $$PWD/../detours

win32:PRE_TARGETDEPS += $$PWD/../detours/detours.lib

Debug: LIBS += -L$$OUT_PWD/../../output_d/ -lCore
Release: LIBS += -L$$OUT_PWD/../../output/ -lCore

INCLUDEPATH += $$PWD/../Core
DEPENDPATH += $$PWD/../Core
