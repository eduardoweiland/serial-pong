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
           src/gameoptions.cpp

HEADERS += src/mainwindow.h \
           src/ball.h \
           src/gameoptions.h \
           src/globals.h

FORMS += src/mainwindow.ui \
         src/gameoptions.ui

RESOURCES += resources/icons.qrc
