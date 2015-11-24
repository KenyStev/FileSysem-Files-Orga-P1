#-------------------------------------------------
#
# Project created by QtCreator 2015-11-04T16:29:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileSystem-Files-Orga-P1
TEMPLATE = app


SOURCES += main.cpp\
        filesys.cpp \
    disk.cpp \
    blocksbox.cpp \
    tree.cpp

HEADERS  += filesys.h \
    disk.h \
    blocksbox.h \
    tree.h \
    nodo.h

FORMS    += filesys.ui \
    blocksbox.ui \
    tree.ui
