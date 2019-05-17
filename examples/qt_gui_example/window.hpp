/*

PICCANTE Examples
The hottest examples of Piccante:
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3.0 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See the GNU Lesser General Public License
    ( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.
*/

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QLabel;
class QToolBar;
class QMenu;
class QAction;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

namespace pic{
class Image;
}

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(QMainWindow *parent = 0);
    virtual ~Window();
protected slots:
    void open();
    void save_as();
    void zoom_in();
    void zoom_out();
    void zoom_original();
    void zoom_fit();
    void gaussian_blur();

protected:
    void closeEvent(QCloseEvent *event);
    void create_actions();
    void create_menus();
    void create_toolbars();
    void update_actions();
    void scale_image(double factor);
    void adjust_scrollbar(QScrollBar *scrollbar, double factor);
    void update_pixmap();

    pic::Image *image;
    QString *last_filename;

    QLabel *image_label;
    QScrollArea *scroll_area;
    double scale_factor;

    QAction *open_action;
    QAction *save_as_action;
    QAction *exit_action;

    QAction *zoom_fit_action;
    QAction *zoom_original_action;
    QAction *zoom_out_action;
    QAction *zoom_in_action;

    QAction *gaussian_blur_action;

    QMenu *file_menu;
    QMenu *view_menu;
    QMenu *edit_menu;

    QToolBar *file_toolbar;
    QToolBar *view_toolbar;
    QToolBar *edit_toolbar;

};

#endif // WINDOW_HPP
