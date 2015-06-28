#-------------------------------------------------
#
# Project created by QtCreator 2014-11-18T10:52:15
#
#-------------------------------------------------

QT       += core gui webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageMap
TEMPLATE = app
LIBS += /home/jamo/SWProjects/QT\ Projets/ImageMap/libfreeimageplus.a


SOURCES += main.cpp\
        mainwindow.cpp \
    imagedata.cpp \
    fileparser.cpp

HEADERS  += mainwindow.h \
    imagedata.h \
    fileparser.h

FORMS    += mainwindow.ui
