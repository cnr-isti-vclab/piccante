TARGET = opengl_grow_cut

QT += core
QT += gui
QT += opengl
QT += widgets

TEMPLATE = app
CONFIG   += console
CONFIG   -= app_bundle

SOURCES += main.cpp

CONFIG   += C++11
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

INCLUDEPATH += ../../include

win32-msvc*{
    DEFINES += _CRT_SECURE_NO_DEPRECATE
    SOURCES += ../common_code/gl_core_4_0.c
}

win32{
        DEFINES += NOMINMAX
}

linux-g++*{
    QMAKE_CXXFLAGS += -fopenmp -pthread
    QMAKE_LFLAGS += -fopenmp
    SOURCES += ../common_code/gl_core_4_0.c
}
