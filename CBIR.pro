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
    imageproccesser.cpp \
    imfeature.cpp \
    kdtree.cpp \
    min_pq.cpp \
    sift.cpp \
    utils.cpp \
    xform.cpp

HEADERS  += mainwindow.h \
    imagescollectionview.h \
    imagelabel.h \
    imageproccesser.h \
    imgfeatures.h \
    kdtree.h \
    min_pq.h \
    sift.h \
    utils.h \
    xform.h

RESOURCES += \
    cbir.qrc

INCLUDEPATH += /usr/local/include\


LIBS += -L/usr/local/lib \
     -lopencv_core \
     -lopencv_imgproc \
     -lopencv_features2d \
     -lopencv_highgui
