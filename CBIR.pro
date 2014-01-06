#-------------------------------------------------
#
# Project created by QtCreator 2014-01-05T09:03:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CBIR
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imagescollectionview.cpp \
    imagelabel.cpp \
    imageproccesser.cpp

HEADERS  += mainwindow.h \
    imagescollectionview.h \
    imagelabel.h \
    imageproccesser.h

RESOURCES += \
    cbir.qrc
