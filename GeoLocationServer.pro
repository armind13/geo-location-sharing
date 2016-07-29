QT += core network
QT -= gui

CONFIG += c++11

TARGET = GeoLocationServer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    GeoLocationServer.cpp

HEADERS += \
    GeoLocationServer.h
