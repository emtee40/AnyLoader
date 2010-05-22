#CONFIG += ripping
QT += network
QT -= gui
TARGET = anyloader
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    Movie.cpp \
    Task.cpp \
    EncodeTask.cpp \
    UploadTask.cpp \
    Controller.cpp \
    TitleInformation.cpp \
    Listener.cpp
HEADERS += Movie.h \
    Task.h \
    EncodeTask.h \
    UploadTask.h \
    Controller.h \
    TitleInformation.h \
    Listener.h
ripping {
    QT += dbus
    LIBS += -ldvdcss \
	-ldvdread
    DEFINES += ENABLE_RIPPING
    HEADERS += RipTask.h \
	DVDDrive.h
    SOURCES += RipTask.cpp \
	DVDDrive.cpp
}

OTHER_FILES += ../Protocol.txt

QMAKE_STRIP = echo
mainfiles.path = /usr/local/bin
mainfiles.files = ./anyloader ./HandBrakeCLI
init.path = /etc/init.d
init.files = ./init.d/anyloader
INSTALLS = mainfiles init

