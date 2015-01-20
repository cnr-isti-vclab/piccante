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

#include "window.hpp"

#include <QtWidgets>

#include <QFileDialog>
#include <QImage>

//This means that OpenGL acceleration layer is disable
#define PIC_DISABLE_OPENGL

#include "piccante.hpp"

Window::Window(QMainWindow *parent) :
    QMainWindow(parent),
    image(NULL),
    last_filename(NULL)
{
    image_label = new QLabel;
    image_label->setBackgroundRole(QPalette::Base);
    image_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    image_label->setScaledContents(true);

    scroll_area = new QScrollArea;
    scroll_area->setBackgroundRole(QPalette::Dark);
    scroll_area->setWidget(image_label);
    setCentralWidget(scroll_area);


    create_actions();
    create_menus();
    create_toolbars();

    setWindowTitle("Simple QT");
}

Window::~Window()
{
    if(image != NULL){
        delete image;
    }
    if(last_filename != NULL){
        delete last_filename;
    }
}

void Window::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"),
                                                    QDir::currentPath());
    if(fileName.isEmpty()) {
        return;
    }

    pic::Image *pic_im = new pic::Image;
    pic_im->Read(fileName.toStdString());

    if(!pic_im->isValid()) {
        QMessageBox::information(this, tr("Simple QT"),
                                 tr("Cannot load %1.").arg(fileName));
        delete pic_im;
        save_as_action->setEnabled(false);
        zoom_fit_action->setEnabled(false);
        zoom_in_action->setEnabled(false);
        zoom_out_action->setEnabled(false);
        zoom_original_action->setEnabled(false);
        gaussian_blur_action->setEnabled(false);
        scale_factor = 1.0;
        return;
    }

    if(last_filename != NULL) {
        delete last_filename;
    }

    last_filename = new QString(fileName);
    if(image != NULL) {
        delete image;
    }
    image = pic_im;

    update_pixmap();

    save_as_action->setEnabled(true);
    zoom_fit_action->setEnabled(true);
    gaussian_blur_action->setEnabled(true);
    scale_factor = 1.0;
    update_actions();

    if (!zoom_fit_action->isChecked())
        image_label->adjustSize();
}

void Window::save_as()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save File"),
                                                    *last_filename,
                                                    tr("Images (*.bmp *.tga *.ppm *.pgm *.pfm *.hdr)"));

    bool success = image->Write(fileName.toStdString());
    if(!success) {
        QMessageBox::information(this, tr("Simple QT"),
                                 tr("Cannot save %1.").arg(fileName));
        return;
    }
}

void Window::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void Window::create_actions()
{
    open_action = new QAction(QIcon(":/fileopen.ico"), tr("Open"), this);
    open_action->setShortcuts(QKeySequence::Open);
    connect(open_action, SIGNAL(triggered()), this, SLOT(open()));

    save_as_action = new QAction(QIcon(":/filesaveas.ico"), tr("Save As"), this);
    save_as_action->setShortcuts(QKeySequence::SaveAs);
    connect(save_as_action, SIGNAL(triggered()), this, SLOT(save_as()));
    save_as_action->setEnabled(false);

    exit_action = new QAction(QIcon(":/exit.ico"), tr("Quit"), this);
    exit_action->setShortcuts(QKeySequence::Quit);
    connect(exit_action, SIGNAL(triggered()), this, SLOT(close()));

    zoom_in_action = new QAction(QIcon(":/zoom_in.ico"),tr("Zoom &In (25%)"), this);
    zoom_in_action->setShortcuts(QList<QKeySequence>() << QKeySequence::ZoomIn << QKeySequence(tr("Ctrl++")));
    zoom_in_action->setEnabled(false);
    connect(zoom_in_action, SIGNAL(triggered()), this, SLOT(zoom_in()));

    zoom_out_action = new QAction(QIcon(":/zoom_out.ico"),tr("Zoom &Out (25%)"), this);
    zoom_out_action->setShortcuts(QList<QKeySequence>() << QKeySequence::ZoomOut << QKeySequence(tr("Ctrl+-")));
    zoom_out_action->setEnabled(false);
    connect(zoom_out_action, SIGNAL(triggered()), this, SLOT(zoom_out()));

    zoom_original_action = new QAction(QIcon(":/zoom_original.ico"),tr("&Normal Size"), this);
    zoom_original_action->setShortcut(tr("Ctrl+S"));
    zoom_original_action->setEnabled(false);
    connect(zoom_original_action, SIGNAL(triggered()), this, SLOT(zoom_original()));

    zoom_fit_action = new QAction(QIcon(":/zoom_fit_best.ico"),tr("&Fit to Window"), this);
    zoom_fit_action->setShortcut(tr("Ctrl+F"));
    zoom_fit_action->setEnabled(false);
    zoom_fit_action->setCheckable(true);
    connect(zoom_fit_action, SIGNAL(triggered()), this, SLOT(zoom_fit()));

    gaussian_blur_action = new QAction(tr("Gaussian blur"), this);
    gaussian_blur_action->setShortcut(tr("Ctrl+g"));
    gaussian_blur_action->setEnabled(false);
    connect(gaussian_blur_action, SIGNAL(triggered()), this, SLOT(gaussian_blur()));
}

void Window::create_menus()
{
    file_menu = new QMenu(tr("&File"), this);
    file_menu->addAction(open_action);
    file_menu->addAction(save_as_action);
    file_menu->addSeparator();
    file_menu->addAction(exit_action);

    view_menu = new QMenu(tr("&View"), this);
    view_menu->addAction(zoom_in_action);
    view_menu->addAction(zoom_out_action);
    view_menu->addAction(zoom_original_action);
    view_menu->addSeparator();
    view_menu->addAction(zoom_fit_action);

    edit_menu = new QMenu(tr("&Edit"), this);
    edit_menu->addAction(gaussian_blur_action);

    menuBar()->addMenu(file_menu);
    menuBar()->addMenu(view_menu);
    menuBar()->addMenu(edit_menu);
}

void Window::create_toolbars()
{
    file_toolbar = addToolBar(tr("File"));
    file_toolbar->addAction(open_action);
    file_toolbar->addAction(save_as_action);
    file_toolbar->addSeparator();
    file_toolbar->addAction(exit_action);

    view_toolbar = addToolBar(tr("View"));
    view_toolbar->addAction(zoom_in_action);
    view_toolbar->addAction(zoom_out_action);
    view_toolbar->addAction(zoom_original_action);
    view_toolbar->addSeparator();
    view_toolbar->addAction(zoom_fit_action);

    edit_toolbar = addToolBar(tr("Edit"));
    edit_toolbar->addAction(gaussian_blur_action);
}


void Window::update_actions()
{
    zoom_in_action->setEnabled(!zoom_fit_action->isChecked());
    zoom_out_action->setEnabled(!zoom_fit_action->isChecked());
    zoom_original_action->setEnabled(!zoom_fit_action->isChecked());
}

void Window::zoom_in()
{
    scale_image(1.25);
}

void Window::zoom_out()
{
    scale_image(0.8);
}

void Window::zoom_original()
{
    image_label->adjustSize();
    scale_factor = 1.0;
}

void Window::zoom_fit()
{
    bool fitToWindow = zoom_fit_action->isChecked();
    scroll_area->setWidgetResizable(fitToWindow);
    if (!fitToWindow) {
        zoom_original();
    }
    update_actions();
}

void Window::gaussian_blur()
{
    pic::Image *output = pic::FilterGaussian2D::Execute(image, NULL, 4.0f);
    delete image;
    image = output;
    update_pixmap();
}

void Window::scale_image(double factor)
{
    scale_factor *= factor;
    image_label->resize(scale_factor * image_label->pixmap()->size());

    adjust_scrollbar(scroll_area->horizontalScrollBar(), factor);
    adjust_scrollbar(scroll_area->verticalScrollBar(), factor);

    zoom_in_action->setEnabled(scale_factor < 3.0);
    zoom_out_action->setEnabled(scale_factor > 0.333);
}

void Window::adjust_scrollbar(QScrollBar *scrollbar, double factor)
{
    scrollbar->setValue(int(factor * scrollbar->value() + ((factor - 1) * scrollbar->pageStep()/2)));
}

void Window::update_pixmap()
{
    image_label->clear();

    if(image == NULL) {
        return;
    }

    *image /=(image->getMeanVal()[0] * 4.0f);
    QImage *qimage = image->ConvertToQImage();

    if(qimage->isNull()) {
        exit(EXIT_FAILURE);
    }
    image_label->setPixmap(QPixmap::fromImage(*qimage));
    delete qimage;
}
