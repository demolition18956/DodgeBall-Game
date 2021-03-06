#-------------------------------------------------
#
# Project created by QtCreator 2020-11-02T12:30:01
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DodgeBall
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        gameserver.cpp \
        hostlobbydialog.cpp \
        joinlobbydialog.cpp \
        lobby.cpp \
        main.cpp \
        mainwindow.cpp \
        mapdialog.cpp \
        player.cpp \
        quitdialog.cpp \
    ball.cpp \
    scoredialog.cpp

HEADERS += \
    gameserver.h \
        hostlobbydialog.h \
        joinlobbydialog.h \
    lobby.h \
        mainwindow.h \
    mapdialog.h \
    player.h \
        quitdialog.h \
    defs.h \
    ball.h \
    scoredialog.h

FORMS += \
        hostlobbydialog.ui \
        joinlobbydialog.ui \
        lobby.ui \
        mainwindow.ui \
        mapdialog.ui \
        quitdialog.ui \
        scoredialog.ui

RESOURCES += \
        pixmaps.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=
