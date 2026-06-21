QT += core gui widgets sql charts
CONFIG += c++17

TARGET = FitnessTracker
TEMPLATE = app

INCLUDEPATH += headers

SOURCES += \
    src/database.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/addfooddialog.cpp \
    src/datasync.cpp \
    src/statisticsdialog.cpp \
    src/logindialog.cpp \
    src/profiledialog.cpp \
    src/addexercisedialog.cpp

HEADERS += \
    headers/mainwindow.h \
    headers/database.h \
    headers/addfooddialog.h \
    headers/datasync.h \
    headers/statisticsdialog.h \
    headers/logindialog.h \
    headers/profiledialog.h \
    headers/addexercisedialog.h

FORMS += \
    forms/mainwindow.ui \
    forms/addfooddialog.ui

RESOURCES += \
    resources/resources.qrc

DESTDIR = bin
OBJECTS_DIR = obj
MOC_DIR = moc
RCC_DIR = rcc
UI_DIR = ui_generated
