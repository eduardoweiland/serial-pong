# Trabalho de Programação Orientada a Objetos I
# Serial Pong (Futebol Simplificado)
#
# Eduardo Weiland
# Wellington Camara Lopes


include(3rdparty/qextserialport/src/qextserialport.pri)

QT += core gui

TARGET = serial-pong
TEMPLATE = app

SOURCES += src/main.cpp\
           src/mainwindow.cpp \
           src/ball.cpp \
           src/gameoptions.cpp \
           src/game.cpp \
           src/scoreboard.cpp \
           src/player.cpp

HEADERS += src/mainwindow.h \
           src/ball.h \
           src/gameoptions.h \
           src/globals.h \
           src/game.h \
           src/scoreboard.h \
           src/player.h

FORMS += src/mainwindow.ui \
         src/gameoptions.ui

RESOURCES += resources/icons.qrc \
    resources/pixmaps.qrc \
    resources/fonts.qrc
