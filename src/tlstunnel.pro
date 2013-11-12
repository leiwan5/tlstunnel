TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    server.cpp \
    connection.cpp \
    config.cpp

HEADERS += \
    server.h \
    connection.h \
    pch.h \
    config.h

PRECOMPILED_HEADER = pch.h

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

LIBS += -luv
