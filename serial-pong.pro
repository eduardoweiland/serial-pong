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
           src/gameoptions.h

FORMS += src/mainwindow.ui \
         src/gameoptions.ui
