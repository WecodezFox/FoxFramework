#-------------------------------------------------
#
# Project created by QtCreator 2014-08-21T13:04:55
#
#-------------------------------------------------

QT       += script
QT       -= gui

TARGET = StatHook
TEMPLATE = lib

SOURCES += stathook.cpp

HEADERS += stathook.h

Debug:DESTDIR = ../../output_d/script
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui

Release:DESTDIR = ../../output/script
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui

Debug: LIBS += -L$$OUT_PWD/../../output_d/ -lCore
Release: LIBS += -L$$OUT_PWD/../../output/ -lCore

INCLUDEPATH += $$PWD/../Core
DEPENDPATH += $$PWD/../Core
