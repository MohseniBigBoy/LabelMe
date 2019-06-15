#-------------------------------------------------
#
# Project created by QtCreator 2018-07-16T07:42:48
#
#-------------------------------------------------

QT       += core gui testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = labelMe2.0
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    customscene.cpp \
    defs.cpp \
    roi_selection.cpp \ 
    help_window.cpp


HEADERS  += mainwindow.h \
    customscene.h \
    defs.h \
    roi_selection.h \  
    help_window.h

FORMS    += mainwindow.ui \
    roi_selection.ui \ 
    help_window.ui

