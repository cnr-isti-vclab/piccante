/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

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
#include <QtCore/QTextStream>

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

