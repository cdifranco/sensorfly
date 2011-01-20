#-------------------------------------------------
#
# Project created by QtCreator 2010-12-19T15:35:54
#
#-------------------------------------------------

QT       += core gui
QT       += dbus

TARGET = Sensorfly
TEMPLATE = app

#LIBS += -lbluetooth

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

CONFIG += mobility
unix:!symbian {
    CONFIG += qdbus
}
MOBILITY = 

symbian {
    TARGET.UID3 = 0xe489b003
    # TARGET.CAPABILITY += 
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
}

RESOURCES += \
    Sensorfly.qrc
