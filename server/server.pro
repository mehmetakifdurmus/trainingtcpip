TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c

HEADERS += \
    threadlist.h

LIBS += -lpthread
