TEMPLATE = lib
TARGET = gato
VERSION = 1.0.0

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
    helpers.cpp \
    gatoservice.cpp \
    gatocharacteristic.cpp \
    gatodescriptor.cpp \
    gatoattclient.cpp

HEADERS += libgato_global.h gato.h \
    gatocentralmanager.h \
    gatouuid.h \
    gatoperipheral.h \
    gatoaddress.h \
    gatosocket.h \
    helpers.h \
    gatoperipheral_p.h \
    gatocentralmanager_p.h \
    gatoservice.h \
    gatocharacteristic.h \
    gatodescriptor.h \
    gatoattclient.h

target.path = /usr/lib
INSTALLS += target

publicheaders.files = libgato_global.h gato.h \
	gatocentralmanager.h gatoperipheral.h \
	gatoservice.h gatocharacteristic.h gatodescriptor.h \
	gatouuid.h gatoaddress.h
publicheaders.path = /usr/include/gato
INSTALLS += publicheaders

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog
