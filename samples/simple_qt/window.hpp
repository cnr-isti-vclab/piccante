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
