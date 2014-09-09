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

#ifndef CLASS_OPENGL_WINDOW_HPP
#define CLASS_OPENGL_WINDOW_HPP

namespace pic {

#ifndef PIC_DISABLE_QT

#include <QOpenGLFunctions>

/**
 * @brief The OpenGLWindow class
 */
class OpenGLWindow : public QWindow, protected QOpenGLFunctions
{
protected:

    /**
     * @brief event
     * @param event
     * @return
     */
    bool event(QEvent *event)
    {
        switch (event->type()) {
                
        case QEvent::UpdateRequest:{
            update_pending = false;
            renderNow();
            return true;
        } break;
                
        default: {
            return QWindow::event(event);
        }
        }
    }

    /**
     * @brief exposeEvent
     * @param event
     */
    void exposeEvent(QExposeEvent *event)
    {
        Q_UNUSED(event);

        if (isExposed())
            renderNow();
    }

    bool update_pending;
    bool animating;

    QOpenGLContext *context;
    QOpenGLPaintDevice *device;

public:

    /**
     * @brief OpenGLWindow
     * @param parent
     */
    OpenGLWindow(QWindow *parent)
        : QWindow(parent)
        , update_pending(false)
        , animating(false)
        , context(0)
        , device(0)
    {
        setSurfaceType(QWindow::OpenGLSurface);
    }

    /**
     * @brief render
     * @param painter
     */
    void render(QPainter *painter)
    {
        Q_UNUSED(painter);
    }

    /**
     * @brief render
     */
    virtual void render()
    {
    }

    /**
     * @brief init
     */
    virtual void init()
    {

    }

    /**
     * @brief setAnimating
     * @param animating
     */
    void setAnimating(bool animating)
    {
        this->animating = animating;

        if (animating)
            renderLater();

    }

public slots:

    /**
     * @brief renderLater
     */
    void renderLater()
    {
        if (!update_pending) {
            update_pending = true;
            QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
        }
    }

    /**
     * @brief renderNow
     */
    void renderNow()
    {
        if (!isExposed())
            return;

        bool toInit = false;

        if (!context) {
            context = new QOpenGLContext(this);
            QSurfaceFormat tmp = requestedFormat();
            tmp.setMajorVersion(4);
            tmp.setMinorVersion(0);
            tmp.setProfile(QSurfaceFormat::CoreProfile);
            context->setFormat(tmp);
            context->create();

            toInit = true;
        }

        context->makeCurrent(this);

        if(toInit) {
            initializeOpenGLFunctions();
            init();
        }

        render();

        context->swapBuffers(this);

        if (animating)
            renderLater();
    }
};

#endif

}  // end namespace pic

#endif // CLASS_OPENGL_WINDOW_HPP
