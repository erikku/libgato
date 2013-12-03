TEMPLATE = lib
TARGET = gato

QT -= gui

DEFINES += LIBGATO_LIBRARY

CONFIG += link_pkgconfig
PKGCONFIG += bluez

SOURCES += \
    gatocentralmanager.cpp \
    gatouuid.cpp \
    gatoperipheral.cpp \
    gatoaddress.cpp \
    gatosocket.cpp \
    gatoatt.cpp \
    helpers.cpp \
    gatoservice.cpp \
    gatocharacteristic.cpp \
    gatodescriptor.cpp

HEADERS += libgato_global.h \
    gatocentralmanager.h \
    gatouuid.h \
    gatoperipheral.h \
    gatoaddress.h \
    gatosocket.h \
    gatoatt.h \
    helpers.h \
    gatoperipheral_p.h \
    gatocentralmanager_p.h \
    gatoservice.h \
    gatocharacteristic.h \
    gatodescriptor.h \
    gato.h

target.path = /usr/lib
INSTALLS += target

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog
