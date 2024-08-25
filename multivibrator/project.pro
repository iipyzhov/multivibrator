QT += core gui serialport
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyApp
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp

HEADERS += mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    translations/project_en_US.ts \
    translations/project_ru_RU.ts \
    translations/project_de_DE.ts
