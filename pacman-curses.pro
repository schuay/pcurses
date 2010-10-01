#-------------------------------------------------
#
# Project created by QtCreator 2010-09-27T23:22:42
#
#-------------------------------------------------

QT       -= core gui

TARGET = pacman-curses
TEMPLATE = app

SOURCES += \
    package.cpp \
    main.cpp

HEADERS += \
    alpmexception.h \
    package.h \

LIBS += -lalpm \
    -lncursesw
