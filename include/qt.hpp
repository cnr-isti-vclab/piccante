/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

/**
  Check if QT is not disabled, and includes the necessary headers.
  Works with QT version 4 or version 5
  */

#ifndef PIC_QT_HPP
#define PIC_QT_HPP

#ifndef PIC_DISABLE_QT

#include <QtCore/QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

/* not ready for future versions */

#elif (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
/* we got Qt 5 */
#define PIC_QT

#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qfile.h>
#include <QtCore/qtimer.h>
#include <QtCore/qvariant.h>

#include <QtCore/qdir.h>
#include <QtCore/QTextStream.h>
//#include <QtCore/QTime>

#include <QtGui/QColor>
#include <QtGui/QImage>

#elif (QT_VERSION >= QT_VERSION_CHECK(4, 0, 0))
/* we got Qt 4 */
#define PIC_QT

#include <QString>
#include <QStringList>
#include <QFile>
#include <QTimer>
#include <QVariant.h>
#include <QColor>
#include <QDir.h>
#include <QTextStream>
#include <QTime>
#include <QImage>
#include <QDebug>

#else /* unsupported qt version */

/* we got Qt 3 or below, unsupported */

#endif /* qt version selection */

#endif /* PIC_DISABLE_QT */

#endif /* PIC_QT_HPP */

