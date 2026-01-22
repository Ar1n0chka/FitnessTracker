QT += core gui widgets #sql

CONFIG += c++17

TARGET = FitnessTracker
TEMPLATE = app

INCLUDEPATH += headers

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    #src/database.cpp \
    src/addfooddialog.cpp

HEADERS += \
    headers/mainwindow.h \
   # headers/database.h \
    headers/addfooddialog.h

FORMS += \
    forms/mainwindow.ui \
    forms/addfooddialog.ui

RESOURCES += \
    resources/resources.qrc \
    resources/resources.qrc

DESTDIR = bin
OBJECTS_DIR = obj
MOC_DIR = moc
RCC_DIR = rcc
UI_DIR = ui_generated
