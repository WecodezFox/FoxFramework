#-------------------------------------------------
#
# Project created by QtCreator 2014-08-21T13:49:37
#
#-------------------------------------------------
CONFIG += c++11
QT       += script

TARGET = Core
TEMPLATE = lib

DEFINES += CORE_LIBRARY

SOURCES += player.cpp \
    memorymanager.cpp \
    keyboard.cpp \
    injector.cpp \    
    PatternFind.cpp \
    patternfinder.cpp \
    botmanager.cpp \
    botthread.cpp

HEADERS += player.h\
        core_global.h \
    memorymanager.h \
    keyboard.h \
    injector.h \    
    PatternFind.h \
    patternfinder.h \
    botmanager.h \
    botthread.h

win32: LIBS += -L$$PWD/../detours/ -ldetours

INCLUDEPATH += $$PWD/../detours
DEPENDPATH += $$PWD/../detours

win32:PRE_TARGETDEPS += $$PWD/../detours/detours.lib

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

OTHER_FILES += \
    ../FoxFrameworkApi.txt
