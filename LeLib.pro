#-------------------------------------------------
#
# Project created by QtCreator 2018-02-19T14:21:59
#
#-------------------------------------------------

QT       += network opengl

TARGET = LeLib
TEMPLATE = lib
CONFIG += staticlib


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        lelib.cpp \
    source/ltiff/ltiff.cpp \
    source/util/downloadmanager.cpp \
    source/util/filedownloader.cpp \
    source/util/lmessage.cpp \
    source/util/updater.cpp \
    source/util/util.cpp \
    source/limage/lcolorlist.cpp \
    source/limage/limage.cpp \
    source/limage/limagefactory.cpp \
    source/limage/limageio.cpp \
    source/limage/limageqimage.cpp \
    source/limage/multicolorimage.cpp \
    source/limage/standardcolorimage.cpp \
    source/data.cpp

HEADERS += \
        lelib.h \
    source/ltiff/ltiff.h \
    source/util/downloadmanager.h \
    source/util/filedownloader.h \
    source/util/lmessage.h \
    source/util/random.h \
    source/util/updater.h \
    source/util/util.h \
    source/util/updater.h \
    source/util/counter.h \
    source/limage/lcolorlist.h \
    source/limage/limage.h \
    source/limage/limage.h.Z10876 \
    source/limage/limagefactory.h \
    source/limage/limageio.h \
    source/limage/limageqimage.h \
    source/limage/multicolorimage.h \
    source/limage/standardcolorimage.h \
    source/data.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix|win32: LIBS += -L$$PWD/lib/ -llibtiff

INCLUDEPATH += $$PWD/lib/libtiff
INCLUDEPATH += $$PWD/lib
DEPENDPATH += $$PWD/lib/libtiff
