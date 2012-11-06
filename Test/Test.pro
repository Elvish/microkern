#-------------------------------------------------
#
# Project created by QtCreator 2012-11-04T16:16:23
#
#-------------------------------------------------

QT       += core gui network

TARGET = Test
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../BinProtocol/BinProtocol.c \
    paintbox.cpp

HEADERS  += mainwindow.h \
    ../BinProtocol/Types.h \
    ../BinProtocol/BinProtocol.h \
    PackTypes.h \
    paintbox.h

FORMS    += mainwindow.ui
