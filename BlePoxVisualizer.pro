#-------------------------------------------------
#
# Project created by QtCreator 2016-03-11T10:37:39
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BlePoxVisualizer
TEMPLATE = app

DEFINES -= UNICODE

LIBS += -lsetupapi
LIBS += -lhid
LIBS += -luuid
LIBS += -lUser32

CONFIG += c++11

SOURCES += main.cpp \
    mainwindow.cpp \
    HidDevice.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    HidDevice.h \
    qcustomplot.h

FORMS    += mainwindow.ui
