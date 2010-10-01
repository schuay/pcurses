#-------------------------------------------------
#
# Project created by QtCreator 2010-09-27T23:22:42
#
#-------------------------------------------------

QT       -= core gui

TARGET = pacman-curses
TEMPLATE = app

SOURCES += \
    lib/package.cpp \
    frontend/main.cpp

HEADERS += \
    lib/alpmexception.h \
    lib/package.h \

LIBS += -lalpm \
    -lncursesw
