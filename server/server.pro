TEMPLATE = app
CONFIG -= app_bundle
CONFIG += console
CONFIG -= qt

SOURCES += \
        main.c \
        server.c

HEADERS += \
    server.h

LIBS += -lpthread
