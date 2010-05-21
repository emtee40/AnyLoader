# -------------------------------------------------
# Project created by QtCreator 2010-05-21T23:30:18
# -------------------------------------------------

CONFIG += ripping

QT += network
QT -= gui
TARGET = AnyLoader
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    Movie.cpp \
    Task.cpp \
    EncodeTask.cpp
HEADERS += Movie.h \
    Task.h \
    EncodeTask.h

ripping {
	QT += dbus
	LIBS += -ldvdcss -ldvdread
	DEFINES += ENABLE_RIPPING
	HEADERS += RipTask.h DVDDrive.h
	SOURCES += RipTask.cpp DVDDrive.cpp
}
