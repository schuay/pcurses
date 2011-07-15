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
    filter.cpp \
    attributeinfo.cpp \
    history.cpp \
    inputbuffer.cpp

HEADERS += \
    package.h \
    cursesframe.h \
    curseslistbox.h \
    program.h \
    config.h \
    filter.h \
    attributeinfo.h \
    history.h \
    pcursesexception.h \
    inputbuffer.h

LIBS += -lalpm \
    -lncursesw
