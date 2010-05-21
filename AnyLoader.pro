# -------------------------------------------------
# Project created by QtCreator 2010-05-21T23:30:18
# -------------------------------------------------
QT += network
QT -= gui
TARGET = AnyLoader
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    Movie.cpp \
    Task.cpp \
    EncodeTask.cpp \
    RipTask.cpp \
    DVDDrive.cpp
HEADERS += Movie.h \
    Task.h \
    EncodeTask.h \
    RipTask.h \
    DVDDrive.h
QT += dbus
LIBS += -ldvdcss \
    -ldvdread
