#-------------------------------------------------
#
# Project created by QtCreator 2014-08-02T10:23:04
#
#-------------------------------------------------
CONFIG += c++11

QT     += core gui widgets network script scripttools

TARGET = Server
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    server.cpp \
    maplelauncher.cpp \
    maplemanager.cpp \
    managerwidget.cpp \
    ../Common/ScriptFunctions.cpp

HEADERS  += mainwindow.h \
    server.h \
    maplelauncher.h \
    maplemanager.h \
    managerwidget.h \
    ../Common/ScriptFunctions.h \
    ../Common/Tags.h

FORMS    += mainwindow.ui \
    managerwidget.ui


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

RESOURCES += \
    resource.qrc
