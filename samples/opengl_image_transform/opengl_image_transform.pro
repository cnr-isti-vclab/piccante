TARGET = opengl_image_transform

QT       += core
TEMPLATE = app
CONFIG   += console
CONFIG   -= app_bundle

SOURCES += main.cpp


CONFIG   += C++11
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

INCLUDEPATH += ../../include

win32-msvc*{
    DEFINES += _CRT_SECURE_NO_DEPRECATE
}

win32{
        DEFINES += NOMINMAX
}


