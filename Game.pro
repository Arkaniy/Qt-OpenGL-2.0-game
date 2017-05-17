#-------------------------------------------------
#
# Project created by QtCreator 2014-06-07T15:21:49
#
#-------------------------------------------------

QT       += core gui opengl
#QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Game
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    map.cpp \
    game.cpp \
    interface.cpp \
    allobjects.cpp \
    objects.cpp \
    mainhero.cpp \
    units.cpp \
    enemy.cpp \
    textures.cpp

HEADERS  += mainwindow.h \
    interface.h \
    gameplay.h \
    textures.h
