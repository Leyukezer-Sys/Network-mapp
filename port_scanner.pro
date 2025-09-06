QT += core gui network widgets

TARGET = port_scanner
TEMPLATE = app

SOURCES += main.cpp \
           scanner_thread.cpp

HEADERS += \
    scanner_thread.h
