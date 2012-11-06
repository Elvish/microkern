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
    ../BinProtocol/BinProtocol.c

HEADERS  += mainwindow.h \
    ../BinProtocol/Types.h \
    ../BinProtocol/BinProtocol.h \
    PackTypes.h

FORMS    += mainwindow.ui
