#-------------------------------------------------
#
# Project created by QtCreator 2010-09-27T23:22:42
#
#-------------------------------------------------

QT       -= core gui

TARGET = pcurses
TEMPLATE = app

SOURCES += \
    package.cpp \
    main.cpp \
    cursesframe.cpp \
    curseslistbox.cpp \
    program.cpp \
    config.cpp \
    filter.cpp

HEADERS += \
    alpmexception.h \
    package.h \
    cursesframe.h \
    curseslistbox.h \
    program.h \
    config.h \
    filter.h

LIBS += -lalpm \
    -lncursesw
