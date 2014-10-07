# PICCANTE
# The hottest HDR imaging library!
# http://vcg.isti.cnr.it/piccante
# 
# Copyright (C) 2014
# Visual Computing Laboratory - ISTI CNR
# http://vcg.isti.cnr.it
# First author: Francesco Banterle
# 
# PICCANTE is free software; you can redistribute it and/or modify
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 3.0 of
# the License, or (at your option) any later version.
# 
# PICCANTE is distributed in the hope that it will be useful, but
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU Lesser General Public License
# ( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

TARGET = corners_extraction

QT       += core
TEMPLATE = app
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += C++11
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

INCLUDEPATH += ../../include

SOURCES += main.cpp

win32-msvc*{
    DEFINES += _CRT_SECURE_NO_DEPRECATE
}

win32{
	DEFINES += NOMINMAX
}
