QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenGL_3
TEMPLATE = app

CONFIG += c++14

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mainview.cpp \
    object.cpp \
    solarsystem.cpp \
    user_input.cpp \
    model.cpp \
    utility.cpp

HEADERS += \
    camera.h \
    mainwindow.h \
    mainview.h \
    model.h \
    object.h \
    solarsystem.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc
