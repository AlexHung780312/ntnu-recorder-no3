#-------------------------------------------------
#
# Project created by QtCreator 2015-12-07T22:02:08
#
#-------------------------------------------------

QT       += core gui xlsx printsupport multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NTNU_Recoder3
TEMPLATE = app
CONFIG += warn_off
SOURCES += main.cpp\
        widget.cpp \
    qcustomplot.cpp \
    audioinput.cpp

HEADERS  += widget.h \
    qcustomplot.h \
    audioinput.h \
    wavheader.h

FORMS    += widget.ui

RESOURCES += \
    icons.qrc
